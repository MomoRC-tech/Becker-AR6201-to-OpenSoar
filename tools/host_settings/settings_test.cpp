// From the repository root (under WSL on Windows):
// g++ -std=c++17 -Itools/host_settings tools/host_settings/settings_test.cpp -o /tmp/bridge-settings-test
// /tmp/bridge-settings-test
//
// Runs the firmware's settings code with synthetic defaults and simulated NVS.
// It does not exercise the ESP32 flash driver, browser API or restart hardware.

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#ifdef NDEBUG
#error "Build the host settings test with assertions enabled"
#endif

#define ESP32
#define STORED_SETTINGS
#define MODE_STA_AP
#define SUPPLY_SENSE
#define NUM_COM 3
#define SERIAL_8N1 0x800001Cu
#define SERIAL_8E1 0x800001Eu
#define SERIAL_7O2 0x800003Bu
#define SERIAL_5N2 0x8000030u
#define SERIAL_PARAM0 SERIAL_8N1
#define SERIAL_PARAM1 SERIAL_8N1
#define SERIAL_PARAM2 SERIAL_8N1
#define SERIAL0_TCP_PORT 10110
#define SERIAL1_TCP_PORT 2000
#define SERIAL2_TCP_PORT 4352
#define UART_BAUD0 19200
#define UART_BAUD1 19200
#define UART_BAUD2 9600
#define PORT_PROFILES {"zs1", "flarm", "ar6201"}
#define PORT_NAMES {"Zander", "FLARM", "Becker"}
#define SSID "host-default"
#define PASSWD ""
#define HOTSPOT_SSID "host-hotspot"
#define HOTSPOT_PASSWORD "test-password-compiled-suffix"
#define STATIC_IP IPAddress(10, 10, 10, 10)
#define JOIN_TIMEOUT_MS 15000
#define SUPPLY_SENSOR_FITTED 1
#define SUPPLY_TCP_PORT 4353

template <typename... Args>
void discardLog(Args...) {}
#define LOGI(category, ...) discardLog(__VA_ARGS__)
#define LOGW(category, ...) discardLog(__VA_ARGS__)
#define LOGE(category, ...) discardLog(__VA_ARGS__)

struct IPAddress {
    uint32_t raw;
    explicit IPAddress(uint32_t value) : raw(value) {}
    IPAddress(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
        : raw(a | (b << 8) | (c << 16) | (d << 24)) {}

    explicit operator uint32_t() const { return raw; }
    uint8_t operator[](unsigned i) const { return raw >> (i * 8); }

    std::string toString() const {
        char text[16];
        snprintf(text, sizeof(text), "%u.%u.%u.%u",
                 unsigned((*this)[0]), unsigned((*this)[1]), unsigned((*this)[2]), unsigned((*this)[3]));
        return text;
    }
};

static size_t hostStrlcpy(char* out, const char* src, size_t size) {
    const size_t length = strlen(src);
    if (size) {
        const size_t count = length < size ? length : size - 1;
        memcpy(out, src, count);
        out[count] = '\0';
    }
    return length;
}
#define strlcpy hostStrlcpy

#include "../../firmware/bridge/settings.h"

static void freshDevice() {
    legacy.clear();
    flashState.clear();
    nextWriteFault = WriteFault::None;
    settingsLoad();
}

static BridgeSettings candidate(const char* network, uint32_t baud) {
    BridgeSettings next = settings;
    memset(next.netSsid, 0, sizeof(next.netSsid));
    strlcpy(next.netSsid, network, sizeof(next.netSsid));
    next.port[0].baud = baud;
    memset(next.port[0].profile, 0, sizeof(next.port[0].profile));
    strlcpy(next.port[0].profile, PROFILE_CUSTOM, sizeof(next.port[0].profile));
    return next;
}

static void expect(const char* network, uint32_t baud, bool trial) {
    assert(strcmp(settings.netSsid, network) == 0);
    assert(settings.port[0].baud == baud);
    assert(settingsTrial == trial);
}

static void save(const BridgeSettings& next, bool expectedTrial) {
    bool trial = !expectedTrial;
    assert(settingsSave(next, trial));
    assert(trial == expectedTrial);
}

static void lastGood() {
    freshDevice();
    save(candidate(SSID, 4800), false);
    settingsLoad();
    expect(SSID, 4800, false);
}

static void startTrial() {
    lastGood();
    save(candidate("host-trial", 9600), true);
    settingsLoad();
    expect("host-trial", 9600, true);
}

static void failedSaveAndRetry() {
    lastGood();
    const BridgeSettings next = candidate("host-trial", 9600);
    for (auto fault : {WriteFault::OpenFailure, WriteFault::SetFailure}) {
        bool trial = false;
        nextWriteFault = fault;
        assert(!settingsSave(next, trial));
        expect(SSID, 4800, false);
        settingsLoad();
        expect(SSID, 4800, false);
    }
    bool trial = true;
    nextWriteFault = WriteFault::SetFailure;
    assert(!settingsSave(candidate(SSID, 38400), trial));
    settingsLoad();
    expect(SSID, 4800, false);
    save(next, true);
    settingsLoad();
    expect("host-trial", 9600, true);
}

static void recoverUncertainSave() {
    lastGood();
    const BridgeSettings before = settings;
    bool trial = false;
    nextWriteFault = WriteFault::CommitFailure;
    assert(!settingsSave(candidate("host-trial", 9600), trial));
    expect(SSID, 4800, false);
    // Saving the currently active values must replace a possibly durable failed write.
    save(before, false);
    settingsLoad();
    expect(SSID, 4800, false);
}

static void reviseTrialAndRetryRevert() {
    startTrial();
    save(candidate("host-revised", 38400), true);
    settingsLoad();
    expect("host-revised", 38400, true);
    nextWriteFault = WriteFault::SetFailure;
    assert(!settingsRevert());
    expect("host-revised", 38400, true);
    settingsLoad();
    expect("host-revised", 38400, true);
    assert(settingsRevert());
    settingsLoad();
    expect(SSID, 4800, false);
}

static void interruptedRollback() {
    for (auto fault : {WriteFault::BeforeSet, WriteFault::AfterSet}) {
        startTrial();
        nextWriteFault = fault;
        try {
            settingsRevert();
            assert(false && "reboot expected at write");
        } catch (const PowerLoss&) {
            settingsLoad();
        }
        if (fault == WriteFault::BeforeSet) {
            expect("host-trial", 9600, true);
            assert(settingsRevert());
            settingsLoad();
        }
        expect(SSID, 4800, false);
    }
}

static void interruptedSave() {
    for (auto fault : {WriteFault::BeforeSet, WriteFault::AfterSet}) {
        lastGood();
        nextWriteFault = fault;
        try {
            save(candidate("host-trial", 9600), true);
            assert(false && "reboot expected at write");
        } catch (const PowerLoss&) {
            settingsLoad();
        }
        if (fault == WriteFault::AfterSet) {
            expect("host-trial", 9600, true);
            assert(settingsRevert());
            settingsLoad();
        }
        expect(SSID, 4800, false);
    }
}

static void failedBootWrite() {
    lastGood();
    save(candidate("host-trial", 9600), true);
    nextWriteFault = WriteFault::SetFailure;
    settingsLoad();
    expect(SSID, 4800, false);
    for (int boot = 0; boot < SETTINGS_TRIAL_BOOTS; ++boot) {
        settingsLoad();
        expect("host-trial", 9600, true);
    }
    nextWriteFault = WriteFault::SetFailure;
    settingsLoad();
    expect(SSID, 4800, false);
    settingsLoad();
    expect(SSID, 4800, false);
}

static void keepAndReset() {
    startTrial();
    nextWriteFault = WriteFault::SetFailure;
    assert(!settingsKeep());
    expect("host-trial", 9600, true);
    assert(settingsKeep());
    settingsLoad();
    expect("host-trial", 9600, false);
    nextWriteFault = WriteFault::SetFailure;
    assert(!settingsReset());
    settingsLoad();
    expect("host-trial", 9600, false);
    assert(settingsReset());
    settingsLoad();
    expect(SSID, UART_BAUD0, false);
}

static void legacySettingsAndTrial() {
    freshDevice();
    legacy["bridge"] = {
        {"v", uint32_t(1)}, {"ns", std::string("host-legacy")}, {"b0", uint32_t(4800)},
        {"b1", std::string("wrong-type")}, {"np", std::string("")}, {"hp", std::string("override")},
    };
    settingsLoad();
    expect("host-legacy", 4800, false);
    assert(settings.port[1].baud == UART_BAUD1);
    assert(strcmp(settings.port[0].profile, PROFILE_CUSTOM) == 0);

    legacy["bridge"]["tr"] = uint8_t(1);
    legacy["bridge"]["tc"] = uint8_t(SETTINGS_TRIAL_BOOTS - 1);
    legacy["bridge-prev"] = {
        {"v", uint32_t(1)}, {"ns", std::string("host-previous")}, {"b0", uint32_t(9600)},
    };
    nextWriteFault = WriteFault::SetFailure;
    settingsLoad();
    expect("host-previous", 9600, false);
    settingsLoad();
    expect("host-legacy", 4800, true);
    assert(strcmp(settings.hotspotPass, "override") == 0);
    assert(std::string(flashState.begin(), flashState.end()).find("compiled-suffix") == std::string::npos);
    settingsLoad();
    expect("host-previous", 9600, false);
    settingsLoad();
    expect("host-previous", 9600, false);
    nextWriteFault = WriteFault::EraseFailure;
    assert(!settingsReset());
    settingsLoad();
    expect("host-previous", 9600, false);
    assert(settingsReset());
    settingsLoad();
    expect(SSID, UART_BAUD0, false);
    assert(legacy.empty());
}

static void resetLegacySettings() {
    freshDevice();
    legacy["bridge"] = {{"ns", std::string("host-legacy")}, {"b0", uint32_t(4800)}};
    settingsLoad();
    expect("host-legacy", 4800, false);
    nextWriteFault = WriteFault::EraseFailure;
    assert(!settingsReset());
    settingsLoad();
    expect("host-legacy", 4800, false);
    assert(settingsReset());
    settingsLoad();
    expect(SSID, UART_BAUD0, false);
    assert(legacy.empty());
}

int main() {
    failedSaveAndRetry();
    recoverUncertainSave();
    reviseTrialAndRetryRevert();
    interruptedSave();
    interruptedRollback();
    failedBootWrite();
    keepAndReset();
    legacySettingsAndTrial();
    resetLegacySettings();
    puts("PASS settings persistence, rollback, restart and legacy data");
}
