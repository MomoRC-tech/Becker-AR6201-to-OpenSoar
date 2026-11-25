// Project: settings stored on the ESP32 (firmware plan step 3a, R15). ESP32 only.
//
// The values in config.h are the defaults. The NVS
// namespace "bridge-state" holds settings changed on the settings page. At
// start-up every stored value is checked; an invalid one falls back to its
// default and is marked, so a bad entry can never stop the bridge.
//
// The table kSettings drives loading, display and (later) the settings page.
// Included by bridge.ino after config.h and before status_page.h.

#pragma once

#ifdef ESP32

#include <Preferences.h>
#include <nvs.h>
#include <cstddef>

#define SETTINGS_NAMESPACE "bridge"
#define SETTINGS_FORMAT 1  // legacy format stored under "v"

enum SettingOrigin : uint8_t { ORIGIN_DEFAULT, ORIGIN_STORED, ORIGIN_INVALID };
enum SettingType : uint8_t { T_PROFILE, T_BAUD, T_FRAME, T_TCP, T_SSID, T_NETPASS, T_APPASS, T_SECONDS, T_IP, T_FLAG, T_CAL, T_NETSSID, T_OFFSET, T_PHONE };  // T_NETSSID: may be empty (no network); T_OFFSET: tenths of a degree; T_PHONE: address or 0 = automatic

// ---- device profiles (WP9) ------------------------------------------------------
// A profile fills in the baud rate and frame of one instrument. It is offered only on
// ports whose wiring fits its electrical interface. "custom" keeps manual values.
// Preset values are the current candidates (config.h comments, sources S22, S23, S108);
// the installed instrument settings are still to be confirmed.

enum PortLink : uint8_t { LINK_RS232_IN, LINK_RS422_BOTH };  // electrical interface of a port

static const char* linkText(PortLink l) {
    return l == LINK_RS422_BOTH ? "RS-422, both ways" : "RS-232, bridge receives only";
}

struct DeviceProfile {
    const char* id;  // stored value, at most 7 characters
    const char* name;
    PortLink link;
    uint32_t baud;
    const char* frame;
};

#define PROFILE_CUSTOM "custom"
static const DeviceProfile kProfiles[] = {
    {"zs1", "Zander ZS1", LINK_RS232_IN, 19200, "8N1"},
    {"flarm", "Classic FLARM", LINK_RS232_IN, 19200, "8N1"},
    {"ar6201", "Becker AR6201", LINK_RS422_BOTH, 9600, "8N1"},
};
static constexpr size_t kProfileCount = sizeof(kProfiles) / sizeof(kProfiles[0]);

// Wiring of COM 0, 1, 2 (docs/hardware/wiring.md): two RS-232 modules without return line, the RS-422 board.
static const PortLink kPortLink[] = {LINK_RS232_IN, LINK_RS232_IN, LINK_RS422_BOTH};
static_assert(sizeof(kPortLink) / sizeof(kPortLink[0]) == NUM_COM, "one link per port");

static const DeviceProfile* findProfile(const char* id) {
    for (const DeviceProfile& p : kProfiles)
        if (strcmp(p.id, id) == 0) return &p;
    return nullptr;
}

struct PortSettings {
    char profile[8];  // device profile id or "custom"
    uint32_t baud;
    char frame[4];  // data bits, parity, stop bits, for example "8N1"
    uint16_t tcpPort;
};

struct BridgeSettings {
    PortSettings port[NUM_COM];
    char netSsid[33];      // network to join
    char netPass[64];      // empty = open network
    uint16_t joinSeconds;  // network join time before the hotspot opens
    char hotspotSsid[33];
    char hotspotPass[64];
    uint32_t hotspotIp;    // IPAddress as uint32_t
    uint8_t supplySensor;  // Project (step 3d): 1 = voltage divider fitted on SUPPLY_PIN
    uint16_t supplyCal;    // calibration factor in thousandths, 1000 = 1.000
    uint16_t supplyTcp;    // TCP port of the OpenVario voltage sentences
    int16_t chipTempOffset;  // Project (step 3e): chip sensor reads this much too high, tenths of a degree C; former padding
    uint32_t phoneIp;        // Project (step 4a, 0.5.23): network mode, the phone the bridge dials; 0 = automatic (record format 3)
};

static BridgeSettings settings;  // active values, fixed after settingsLoad()
static bool settingsFormatMismatch = false;

// Trial (confirm or revert): Wi-Fi, hotspot and port changes apply after a restart
// and must be kept on the settings page within SETTINGS_TRIAL_MS, otherwise the
// previous values come back from the saved snapshot.
#define SETTINGS_BACKUP "bridge-prev"
#define SETTINGS_TRIAL_MS (10UL * 60UL * 1000UL)
#define SETTINGS_TRIAL_BOOTS 3  // starts without Keep before the backup is restored at once
static bool settingsTrial = false;  // this start runs on settings that are not yet kept

struct SettingItem {
    const char* key;  // NVS key, at most 15 characters
    SettingType type;
    void* value;
    SettingOrigin origin;
};

static SettingItem kSettings[] = {
    {"p0", T_PROFILE, settings.port[0].profile, ORIGIN_DEFAULT},
    {"b0", T_BAUD, &settings.port[0].baud, ORIGIN_DEFAULT},
    {"f0", T_FRAME, settings.port[0].frame, ORIGIN_DEFAULT},
    {"t0", T_TCP, &settings.port[0].tcpPort, ORIGIN_DEFAULT},
    {"p1", T_PROFILE, settings.port[1].profile, ORIGIN_DEFAULT},
    {"b1", T_BAUD, &settings.port[1].baud, ORIGIN_DEFAULT},
    {"f1", T_FRAME, settings.port[1].frame, ORIGIN_DEFAULT},
    {"t1", T_TCP, &settings.port[1].tcpPort, ORIGIN_DEFAULT},
    {"p2", T_PROFILE, settings.port[2].profile, ORIGIN_DEFAULT},
    {"b2", T_BAUD, &settings.port[2].baud, ORIGIN_DEFAULT},
    {"f2", T_FRAME, settings.port[2].frame, ORIGIN_DEFAULT},
    {"t2", T_TCP, &settings.port[2].tcpPort, ORIGIN_DEFAULT},
    {"ns", T_NETSSID, settings.netSsid, ORIGIN_DEFAULT},
    {"np", T_NETPASS, settings.netPass, ORIGIN_DEFAULT},
    {"nj", T_SECONDS, &settings.joinSeconds, ORIGIN_DEFAULT},
    {"hs", T_SSID, settings.hotspotSsid, ORIGIN_DEFAULT},
    {"hp", T_APPASS, settings.hotspotPass, ORIGIN_DEFAULT},
    {"ha", T_IP, &settings.hotspotIp, ORIGIN_DEFAULT},
#ifdef SUPPLY_SENSE
    {"vs", T_FLAG, &settings.supplySensor, ORIGIN_DEFAULT},  // supply voltage (step 3d)
    {"vc", T_CAL, &settings.supplyCal, ORIGIN_DEFAULT},
    {"vt", T_TCP, &settings.supplyTcp, ORIGIN_DEFAULT},
#endif
#ifdef CHIP_TEMP
    {"ct", T_OFFSET, &settings.chipTempOffset, ORIGIN_DEFAULT},  // chip temperature offset (step 3e)
#endif
    {"pa", T_PHONE, &settings.phoneIp, ORIGIN_DEFAULT},  // step 4a: phone address in network mode; new items go at the end
};
static constexpr size_t kSettingCount = sizeof(kSettings) / sizeof(kSettings[0]);

// ---- serial frame: "8N1" <-> Arduino-ESP32 config code -----------------------

static constexpr uint32_t frameCode(int data, char parity, int stop) {
    return 0x8000010u | (uint32_t(data - 5) << 2) | (stop == 2 ? 0x20u : 0u) |
           (parity == 'E' ? 2u : parity == 'O' ? 3u : 0u);
}
static_assert(frameCode(8, 'N', 1) == SERIAL_8N1, "frame code 8N1");
static_assert(frameCode(8, 'E', 1) == SERIAL_8E1, "frame code 8E1");
static_assert(frameCode(7, 'O', 2) == SERIAL_7O2, "frame code 7O2");
static_assert(frameCode(5, 'N', 2) == SERIAL_5N2, "frame code 5N2");

static bool validFrame(const char* f) {
    return strlen(f) == 3 && f[0] >= '5' && f[0] <= '8' && (f[1] == 'N' || f[1] == 'E' || f[1] == 'O') &&
           (f[2] == '1' || f[2] == '2');
}

static uint32_t settingsFrameCode(int num) {
    const char* f = settings.port[num].frame;
    return frameCode(f[0] - '0', f[1], f[2] - '0');
}

static void frameFromCode(uint32_t code, char* out) {
    const uint32_t parity = code & 3u;
    out[0] = char('5' + ((code >> 2) & 3u));
    out[1] = parity == 2u ? 'E' : parity == 3u ? 'O' : 'N';
    out[2] = (code & 0x20u) ? '2' : '1';
    out[3] = 0;
}

// ---- checks ------------------------------------------------------------------

// Scan order after the current setting and hardware estimate (ZS1 rates first).
static constexpr uint32_t kSerialRates[] = {4800, 19200, 9600, 38400, 57600, 115200, 230400, 2400, 1200};
static bool validBaud(uint32_t b) {
    for (uint32_t a : kSerialRates)
        if (a == b) return true;
    return false;
}

static bool validText(const char* s, size_t minLen, size_t maxLen) {
    const size_t n = strlen(s);
    if (n < minLen || n > maxLen) return false;
    for (size_t i = 0; i < n; i++)
        if ((uint8_t)s[i] < 0x20 || s[i] == 0x7f) return false;  // no control characters
    return true;
}

static bool validIp(uint32_t raw) {
    const IPAddress ip(raw);
    return ip[0] >= 1 && ip[0] <= 223 && ip[0] != 127 && ip[3] >= 1 && ip[3] <= 254;
}

// A profile fits a port if its interface matches the port's wiring; Custom fits every port.
static bool profileFits(const char* id, int portNum) {
    if (strcmp(id, PROFILE_CUSTOM) == 0) return true;
    const DeviceProfile* p = findProfile(id);
    return p && portNum >= 0 && portNum < NUM_COM && p->link == kPortLink[portNum];
}

// Check one value of the given type at v; portNum is -1 for values that belong to no port.
static bool validAt(SettingType type, const void* v, int portNum) {
    switch (type) {
        case T_PROFILE: return profileFits((const char*)v, portNum);
        case T_BAUD: return validBaud(*(const uint32_t*)v);
        case T_FRAME: return validFrame((const char*)v);
        case T_TCP: {
            const uint16_t p = *(const uint16_t*)v;
            return p != 0 && p != 80;  // 80 is the status page
        }
        case T_SSID: return validText((const char*)v, 1, 32);
        case T_NETSSID: return validText((const char*)v, 0, 32);  // empty: no network, hotspot at once
        case T_NETPASS: {
            const size_t n = strlen((const char*)v);
            return n == 0 || validText((const char*)v, 8, 63);
        }
        case T_APPASS: return validText((const char*)v, 8, 63);
        case T_SECONDS: {
            const uint16_t t = *(const uint16_t*)v;
            return t >= 5 && t <= 120;
        }
        case T_IP: return validIp(*(const uint32_t*)v);
        case T_PHONE: return *(const uint32_t*)v == 0 || validIp(*(const uint32_t*)v);
        case T_FLAG: return *(const uint8_t*)v <= 1;
        case T_CAL: {
            const uint16_t c = *(const uint16_t*)v;
            return c >= 900 && c <= 1100;
        }
        case T_OFFSET: {
            const int16_t o = *(const int16_t*)v;
            return o >= 0 && o <= 600;  // 0 to 60 degrees C
        }
    }
    return false;
}

// The group a value belongs to when it belongs to no port: "Wi-Fi", "Supply voltage" or "Chip temperature".
static bool supplyKey(const SettingItem& s) { return s.key[0] == 'v'; }
static bool chipKey(const SettingItem& s) { return s.key[0] == 'c'; }

static bool validValue(const SettingItem& s, int portNum) { return validAt(s.type, s.value, portNum); }

static int portOf(const SettingItem& s) {  // port number for per-port keys, -1 otherwise
    const char c = s.key[1];
    return (s.key[2] == 0 && c >= '0' && c < '0' + NUM_COM && strchr("pbft", s.key[0])) ? c - '0' : -1;
}

// A preset whose baud or frame was edited by hand becomes Custom. Returns the ports changed.
static int profilesMatchValues(BridgeSettings& b) {
    int changed = 0;
    for (int n = 0; n < NUM_COM; n++) {
        PortSettings& ps = b.port[n];
        const DeviceProfile* p = findProfile(ps.profile);
        if (!p || (p->baud == ps.baud && strcmp(p->frame, ps.frame) == 0)) continue;
        memset(ps.profile, 0, sizeof(ps.profile));  // whole-buffer comparisons in settingsSave()
        strlcpy(ps.profile, PROFILE_CUSTOM, sizeof(ps.profile));
        changed |= 1 << n;
    }
    return changed;
}

// ---- display -------------------------------------------------------------------

static bool settingSecret(const SettingItem& s) { return s.type == T_NETPASS || s.type == T_APPASS; }

static bool settingText(const SettingItem& s) {
    return s.type == T_PROFILE || s.type == T_FRAME || s.type == T_SSID || s.type == T_NETSSID || settingSecret(s);
}

// Short name of the value, without the port: "baud", "TCP port", "network" ...
static const char* settingName(const SettingItem& s) {
    switch (s.type) {
        case T_PROFILE: return "device";
        case T_BAUD: return "baud";
        case T_FRAME: return "frame";
        case T_TCP: return "TCP port";
        case T_SSID: return "hotspot name";
        case T_NETSSID: return "network";
        case T_NETPASS: return "network password";
        case T_APPASS: return "hotspot password";
        case T_SECONDS: return "network join time, s";
        case T_IP: return "hotspot address";
        case T_PHONE: return "phone address (network mode)";
        case T_FLAG: return "sensor fitted";
        case T_CAL: return "calibration factor";
        case T_OFFSET: return "offset, \xC2\xB0" "C";
    }
    return "";
}

static const char* settingOriginText(SettingOrigin o) {
    return o == ORIGIN_STORED ? "changed" : o == ORIGIN_INVALID ? "invalid" : "default";
}

// Active value as text; secrets are never written out.
static void settingValueText(const SettingItem& s, char* out, size_t n) {
    switch (s.type) {
        case T_BAUD: snprintf(out, n, "%lu", (unsigned long)*(uint32_t*)s.value); break;
        case T_TCP:
        case T_SECONDS: snprintf(out, n, "%u", (unsigned)*(uint16_t*)s.value); break;
        case T_IP: snprintf(out, n, "%s", IPAddress(*(uint32_t*)s.value).toString().c_str()); break;
        case T_PHONE:  // empty = automatic
            snprintf(out, n, "%s", *(uint32_t*)s.value ? IPAddress(*(uint32_t*)s.value).toString().c_str() : "");
            break;
        case T_FLAG: snprintf(out, n, "%s", *(uint8_t*)s.value ? "on" : "off"); break;
        case T_CAL: snprintf(out, n, "%u.%03u", (unsigned)(*(uint16_t*)s.value / 1000), (unsigned)(*(uint16_t*)s.value % 1000)); break;
        case T_OFFSET: snprintf(out, n, "%.1f", *(int16_t*)s.value / 10.0); break;
        case T_NETPASS:
        case T_APPASS: snprintf(out, n, "%s", ""); break;
        default: snprintf(out, n, "%s", (const char*)s.value); break;
    }
}

// ---- defaults and loading ----------------------------------------------------

static void fillDefaults(BridgeSettings& out) {
    memset(&out, 0, sizeof(out));  // whole-buffer comparisons in settingsSave() need clean padding
    const uint32_t baud[NUM_COM] = {UART_BAUD0, UART_BAUD1, UART_BAUD2};
    const uint32_t frame[NUM_COM] = {SERIAL_PARAM0, SERIAL_PARAM1, SERIAL_PARAM2};
    const uint16_t tcp[NUM_COM] = {SERIAL0_TCP_PORT, SERIAL1_TCP_PORT, SERIAL2_TCP_PORT};
    const char* const profile[NUM_COM] = PORT_PROFILES;
    for (int n = 0; n < NUM_COM; n++) {
        strlcpy(out.port[n].profile, profile[n], sizeof(out.port[n].profile));
        out.port[n].baud = baud[n];
        frameFromCode(frame[n], out.port[n].frame);
        out.port[n].tcpPort = tcp[n];
    }
    strlcpy(out.netSsid, SSID, sizeof(out.netSsid));
    strlcpy(out.netPass, PASSWD, sizeof(out.netPass));
    out.joinSeconds = JOIN_TIMEOUT_MS / 1000;
#ifdef MODE_STA_AP
    strlcpy(out.hotspotSsid, HOTSPOT_SSID, sizeof(out.hotspotSsid));
    strlcpy(out.hotspotPass, HOTSPOT_PASSWORD, sizeof(out.hotspotPass));
#else
    out.hotspotSsid[0] = 0;
    out.hotspotPass[0] = 0;
#endif
    out.hotspotIp = (uint32_t)STATIC_IP;
#ifdef SUPPLY_SENSE
    out.supplySensor = SUPPLY_SENSOR_FITTED;
#endif
    out.supplyCal = 1000;
    out.supplyTcp = SUPPLY_TCP_PORT;
}

static size_t valueSize(SettingType t) {
    switch (t) {
        case T_BAUD:
        case T_IP:
        case T_PHONE: return 4;
        case T_TCP:
        case T_SECONDS:
        case T_CAL:
        case T_OFFSET: return 2;
        case T_FLAG: return 1;
        case T_FRAME: return 4;
        case T_PROFILE: return 8;
        case T_SSID:
        case T_NETSSID: return 33;
        default: return 64;  // passwords
    }
}

// Put one item back to its default value.
static void restoreDefault(SettingItem& s, const BridgeSettings& defaults) {
    const size_t off = (const uint8_t*)s.value - (const uint8_t*)&settings;
    memcpy(s.value, (const uint8_t*)&defaults + off, valueSize(s.type));
}

// Read one stored value into the active settings; false if of the wrong type or size.
static bool readStored(Preferences& p, SettingItem& s) {
    switch (s.type) {
        case T_BAUD:
        case T_IP:
        case T_PHONE:
            if (p.getType(s.key) != PT_U32) return false;
            *(uint32_t*)s.value = p.getUInt(s.key);
            return true;
        case T_TCP:
        case T_SECONDS:
        case T_CAL:
            if (p.getType(s.key) != PT_U16) return false;
            *(uint16_t*)s.value = p.getUShort(s.key);
            return true;
        case T_OFFSET:
            if (p.getType(s.key) != PT_I16) return false;
            *(int16_t*)s.value = p.getShort(s.key);
            return true;
        case T_FLAG:
            if (p.getType(s.key) != PT_U8) return false;
            *(uint8_t*)s.value = p.getUChar(s.key);
            return true;
        default: {  // text
            const size_t cap = valueSize(s.type);
            const size_t len = p.getStringLength(s.key);  // includes the terminating zero
            if (p.getType(s.key) != PT_STR || len == 0 || len > cap) return false;
            memset(s.value, 0, cap);
            return p.getString(s.key, (char*)s.value, cap) > 0;
        }
    }
}

// TCP ports in use: the device ports, then the supply voltage port (index NUM_COM).
#define TCP_LISTENERS (NUM_COM + 1)
static uint16_t& tcpPortOf(BridgeSettings& b, int i) { return i < NUM_COM ? b.port[i].tcpPort : b.supplyTcp; }
static int tcpListenerOf(const SettingItem& s) {  // index for T_TCP items
    return s.value == &settings.supplyTcp ? NUM_COM : portOf(s);
}

// Two ports with the same TCP port cannot both listen: put the clashing ones back to their defaults (which never clash) and mark them.
static void settingsCrossCheck(const BridgeSettings& defaults) {
    for (int round = 0; round < TCP_LISTENERS; round++) {
        bool changed = false;
        for (int a = 0; a < TCP_LISTENERS; a++) {
            for (int b = a + 1; b < TCP_LISTENERS; b++) {
                if (tcpPortOf(settings, a) != tcpPortOf(settings, b)) continue;
                for (size_t i = 0; i < kSettingCount; i++) {
                    SettingItem& s = kSettings[i];
                    if (s.type != T_TCP || s.origin != ORIGIN_STORED) continue;
                    const int n = tcpListenerOf(s);
                    if (n != a && n != b) continue;
                    restoreDefault(s, defaults);
                    s.origin = ORIGIN_INVALID;
                    changed = true;
                }
            }
        }
        if (!changed) return;
    }
}

// ---- storing (settings page) --------------------------------------------------------

static bool trialKey(const SettingItem& s) {  // changes that can cut the phone off
    return s.type == T_TCP || s.type == T_SSID || s.type == T_NETSSID || s.type == T_NETPASS ||
           s.type == T_APPASS || s.type == T_SECONDS || s.type == T_IP;
}

#ifdef STORED_SETTINGS

// The record is the sole authority once present. NVS replaces a blob as one
// value, so active settings, rollback settings and trial metadata move together.
// Keep the field order, types and kSettings indices stable for this format.
#define SETTINGS_STATE_NAMESPACE "bridge-state"
#define SETTINGS_STATE_KEY "state"
static constexpr uint32_t kSettingsStateVersion = 3;  // 3 (0.5.23): phoneIp appended to BridgeSettings

struct SettingsSnapshot {
    uint32_t overrides;
    BridgeSettings values;
};

struct SettingsState {
    uint32_t version;
    uint8_t trial;
    uint8_t boots;
    uint16_t reserved;
    SettingsSnapshot active;
    SettingsSnapshot previous;
};

static_assert(sizeof(PortSettings) == 20 && sizeof(BridgeSettings) == 276 &&
              sizeof(SettingsState) == 568 && kSettingCount <= 32, "settings record layout changed");

// Format 2 (up to 0.5.22): the same record without phoneIp. Read once and carried over;
// the item indices (override bits) are unchanged, the new item comes last.
#define SETTINGS_V2_VALUES 272
struct SettingsSnapshotV2 {
    uint32_t overrides;
    uint8_t values[SETTINGS_V2_VALUES];
};
struct SettingsStateV2 {
    uint32_t version;
    uint8_t trial;
    uint8_t boots;
    uint16_t reserved;
    SettingsSnapshotV2 active;
    SettingsSnapshotV2 previous;
};
static_assert(sizeof(SettingsStateV2) == 560 && offsetof(BridgeSettings, phoneIp) == SETTINGS_V2_VALUES,
              "format 2 layout");

static void snapshotFromV2(const SettingsSnapshotV2& in, SettingsSnapshot& out) {
    out = {};
    out.overrides = in.overrides;
    memcpy(&out.values, in.values, SETTINGS_V2_VALUES);  // phoneIp stays 0 (automatic)
}

static SettingsState settingsState;
static bool settingsStorageReady = false;
static bool settingsHasRecord = false;
static bool settingsWriteUncertain = false;

static uint32_t settingBit(size_t i) { return uint32_t(1) << i; }

static size_t settingOffset(const SettingItem& s) {
    return (const uint8_t*)s.value - (const uint8_t*)&settings;
}

static void normalizeLoadedSettings(const BridgeSettings& defaults) {
    settingsCrossCheck(defaults);
    const int custom = profilesMatchValues(settings);
    for (SettingItem& s : kSettings) {
        if (s.type == T_PROFILE && ((custom >> portOf(s)) & 1) && s.origin == ORIGIN_DEFAULT)
            s.origin = ORIGIN_STORED;
    }
}

static void clearLoadedSettings(const BridgeSettings& defaults) {
    settings = defaults;
    for (SettingItem& s : kSettings) s.origin = ORIGIN_DEFAULT;
}

static void applySnapshot(const SettingsSnapshot& source, const BridgeSettings& defaults) {
    clearLoadedSettings(defaults);
    for (size_t i = 0; i < kSettingCount; i++) {
        SettingItem& s = kSettings[i];
        if (!(source.overrides & settingBit(i))) continue;
        const void* value = (const uint8_t*)&source.values + settingOffset(s);
        const size_t size = valueSize(s.type);
        const bool text = settingText(s);
        if ((!text || memchr(value, 0, size)) && validAt(s.type, value, portOf(s))) {
            memset(s.value, 0, size);
            memcpy(s.value, value, text ? strlen((const char*)value) + 1 : size);
            s.origin = ORIGIN_STORED;
        } else {
            s.origin = ORIGIN_INVALID;
        }
    }
    normalizeLoadedSettings(defaults);
}

static SettingsSnapshot snapshotOf(const BridgeSettings& values, const BridgeSettings& defaults) {
    SettingsSnapshot snapshot = {};
    for (size_t i = 0; i < kSettingCount; i++) {
        const SettingItem& s = kSettings[i];
        const size_t off = settingOffset(s);
        const void* value = (const uint8_t*)&values + off;
        const size_t size = valueSize(s.type);
        if (memcmp(value, (const uint8_t*)&defaults + off, size) == 0) continue;
        memcpy((uint8_t*)&snapshot.values + off, value, size);
        snapshot.overrides |= settingBit(i);
    }
    return snapshot;
}

// Prior firmware stored independent keys. Read them in place and leave them
// untouched until the single authoritative record is committed.
static esp_err_t loadLegacy(const char* name, const BridgeSettings& defaults,
                           bool& trial, uint8_t& boots) {
    clearLoadedSettings(defaults);
    nvs_handle_t nvs;
    const esp_err_t result = nvs_open(name, NVS_READONLY, &nvs);
    if (result == ESP_ERR_NVS_NOT_FOUND) return ESP_OK;
    if (result != ESP_OK) return result;
    nvs_close(nvs);
    Preferences p;
    if (!p.begin(name, true)) return ESP_FAIL;
    if (p.isKey("v") && p.getUInt("v", 0) != SETTINGS_FORMAT) {
        settingsFormatMismatch = true;
        LOGW(LOG_SETTINGS, "Legacy settings from another firmware version; defaults in use");
        return ESP_OK;
    }
    for (SettingItem& s : kSettings) {
        if (!p.isKey(s.key)) continue;
        if (readStored(p, s) && validValue(s, portOf(s))) {
            s.origin = ORIGIN_STORED;
        } else {
            restoreDefault(s, defaults);
            s.origin = ORIGIN_INVALID;
        }
    }
    normalizeLoadedSettings(defaults);
    trial = p.getBool("tr", false);
    boots = p.getUChar("tc", 0);
    return ESP_OK;
}

static esp_err_t readSettingsState(SettingsState& state) {
    nvs_handle_t nvs;
    esp_err_t result = nvs_open(SETTINGS_STATE_NAMESPACE, NVS_READONLY, &nvs);
    if (result != ESP_OK) return result;
    size_t size = sizeof(state);
    result = nvs_get_blob(nvs, SETTINGS_STATE_KEY, &state, &size);
    nvs_close(nvs);
    if (result == ESP_OK && size == sizeof(SettingsStateV2) && state.version == 2) {
        SettingsStateV2 old;
        memcpy(&old, &state, sizeof(old));
        state = {};
        state.version = kSettingsStateVersion;  // written in the new format with the next change
        state.trial = old.trial;
        state.boots = old.boots;
        state.reserved = old.reserved;
        snapshotFromV2(old.active, state.active);
        snapshotFromV2(old.previous, state.previous);
        return ESP_OK;
    }
    if (result == ESP_OK && size != sizeof(state)) return ESP_ERR_NVS_INVALID_LENGTH;
    return result;
}

static bool writeSettingsState(const SettingsState& next) {
    nvs_handle_t nvs;
    esp_err_t result = nvs_open(SETTINGS_STATE_NAMESPACE, NVS_READWRITE, &nvs);
    if (result == ESP_OK) {
        // A flash error can be reported after NVS has written the new value.
        // Require a successful write before treating a later no-op as saved.
        settingsWriteUncertain = true;
        result = nvs_set_blob(nvs, SETTINGS_STATE_KEY, &next, sizeof(next));
        if (result == ESP_OK) result = nvs_commit(nvs);
        nvs_close(nvs);
    }
    if (result != ESP_OK) {
        LOGW(LOG_SETTINGS, "Could not write settings to flash (0x%x)", unsigned(result));
        return false;
    }
    settingsState = next;
    settingsWriteUncertain = false;
    settingsHasRecord = true;
    settingsStorageReady = true;
    return true;
}

static SettingsState restoredState() {
    SettingsState next = {};
    next.version = kSettingsStateVersion;
    next.active = settingsState.previous;
    return next;
}

static void selectTrialAtBoot(const BridgeSettings& defaults) {
    const bool expired = settingsState.boots >= SETTINGS_TRIAL_BOOTS;
    SettingsState next = expired ? restoredState() : settingsState;
    if (!expired) next.boots++;
    if (!writeSettingsState(next)) {
        // We could not account for this boot. Run the last good settings so
        // repeated storage failures cannot strand the user on a trial.
        applySnapshot(settingsState.previous, defaults);
        settingsStorageReady = false;
        LOGW(LOG_SETTINGS, "Could not update trial in flash: previous settings in use for this start");
        return;
    }
    settingsTrial = next.trial;
    applySnapshot(next.active, defaults);
    if (expired) {
        LOGW(LOG_SETTINGS, "Trial not kept after %u starts: previous settings restored", SETTINGS_TRIAL_BOOTS);
    } else {
        LOGW(LOG_SETTINGS, "New settings on trial, start %u of %u: press Keep on the settings page within %lu min",
             unsigned(next.boots), SETTINGS_TRIAL_BOOTS, SETTINGS_TRIAL_MS / 60000UL);
    }
}

// Further starts that keep a trial running: each start without Keep, also one from
// the Restart button, counts towards SETTINGS_TRIAL_BOOTS.
static unsigned settingsTrialStartsLeft() {
    return settingsState.boots < SETTINGS_TRIAL_BOOTS ? SETTINGS_TRIAL_BOOTS - settingsState.boots : 0;
}

static bool settingsKeep() {
    if (!settingsStorageReady) return false;
    if (!settingsTrial) return !settingsWriteUncertain || writeSettingsState(settingsState);
    SettingsState next = settingsState;
    next.trial = 0;
    next.boots = 0;
    next.previous = {};
    if (!writeSettingsState(next)) return false;
    settingsTrial = false;
    LOGI(LOG_SETTINGS, "Settings on trial kept");
    return true;
}

static bool settingsRevert() {
    if (!settingsStorageReady || !settingsTrial || !writeSettingsState(restoredState())) return false;
    settingsTrial = false;
    LOGW(LOG_SETTINGS, "Trial not kept within %lu min: previous settings restored, restarting", SETTINGS_TRIAL_MS / 60000UL);
    return true;
}

static bool eraseLegacySettings(const char* name) {
    nvs_handle_t nvs;
    // An absent namespace needs no write.
    esp_err_t result = nvs_open(name, NVS_READONLY, &nvs);
    if (result == ESP_ERR_NVS_NOT_FOUND) return true;
    if (result == ESP_OK) {
        nvs_close(nvs);
        result = nvs_open(name, NVS_READWRITE, &nvs);
        if (result == ESP_OK) {
            result = nvs_erase_all(nvs);
            if (result == ESP_OK) result = nvs_commit(nvs);
            nvs_close(nvs);
        }
    }
    if (result != ESP_OK) {
        LOGW(LOG_SETTINGS, "Could not erase legacy settings (0x%x)", unsigned(result));
        return false;
    }
    return true;
}

static bool settingsReset() {
    // When still using independent legacy keys, commit their equivalent
    // record first. A failed erase then leaves the old settings usable.
    if (!settingsHasRecord) {
        if (!settingsStorageReady || !writeSettingsState(settingsState)) return false;
    }
    if (!eraseLegacySettings(SETTINGS_NAMESPACE) || !eraseLegacySettings(SETTINGS_BACKUP)) return false;
    SettingsState next = {};
    next.version = kSettingsStateVersion;
    if (!writeSettingsState(next)) return false;
    settingsTrial = false;
    LOGI(LOG_SETTINGS, "Stored settings forgotten, restarting with the defaults");
    return true;
}

static bool settingsSave(const BridgeSettings& cand, bool& trial) {
    if (!settingsStorageReady) return false;
    bool changed = false;
    bool risky = false;
    for (const SettingItem& s : kSettings) {
        const void* value = (const uint8_t*)&cand + settingOffset(s);
        if (memcmp(value, s.value, valueSize(s.type)) == 0) continue;
        changed = true;
        risky |= trialKey(s);
    }
    trial = risky || settingsTrial;
    if (!changed) return !settingsWriteUncertain || writeSettingsState(settingsState);

    BridgeSettings defaults;
    fillDefaults(defaults);
    SettingsState next = {};
    next.version = kSettingsStateVersion;
    next.active = snapshotOf(cand, defaults);
    next.trial = trial;
    if (trial) {
        next.previous = settingsTrial ? settingsState.previous : snapshotOf(settings, defaults);
    }
    if (!writeSettingsState(next)) return false;
    LOGI(LOG_SETTINGS, "Settings saved, restarting%s", trial ? " (on trial)" : "");
    return true;
}

#endif  // STORED_SETTINGS

static void settingsLoad() {
    BridgeSettings defaults;
    fillDefaults(defaults);
    settings = defaults;
    for (SettingItem& s : kSettings) s.origin = ORIGIN_DEFAULT;
#ifdef STORED_SETTINGS
    settingsState = {};
    settingsTrial = false;
    settingsFormatMismatch = false;
    settingsStorageReady = false;
    settingsWriteUncertain = false;
    const esp_err_t result = readSettingsState(settingsState);
    settingsHasRecord = result == ESP_OK || result == ESP_ERR_NVS_INVALID_LENGTH ||
                        result == ESP_ERR_NVS_TYPE_MISMATCH;
    if (result == ESP_ERR_NVS_NOT_FOUND) {
        bool trial = false;
        uint8_t boots = 0;
        esp_err_t legacyResult = loadLegacy(SETTINGS_NAMESPACE, defaults, trial, boots);
        if (legacyResult == ESP_OK) {
            settingsState.version = kSettingsStateVersion;
            settingsState.active = snapshotOf(settings, defaults);
            if (trial) {
                settingsState.trial = 1;
                settingsState.boots = boots;
                legacyResult = loadLegacy(SETTINGS_BACKUP, defaults, trial, boots);
                if (legacyResult == ESP_OK) {
                    settingsState.previous = snapshotOf(settings, defaults);
                    selectTrialAtBoot(defaults);
                }
            } else {
                settingsStorageReady = true;
            }
        }
        if (legacyResult != ESP_OK) LOGW(LOG_SETTINGS, "Could not read legacy settings (0x%x)", unsigned(legacyResult));
    } else if (result != ESP_OK || settingsState.version != kSettingsStateVersion ||
               settingsState.trial > 1 || settingsState.reserved != 0) {
        settingsFormatMismatch = settingsHasRecord;
        LOGW(LOG_SETTINGS, "Could not read stored settings (0x%x); defaults in use", unsigned(result));
    } else if (settingsState.trial) {
        selectTrialAtBoot(defaults);
    } else {
        settingsStorageReady = true;
        applySnapshot(settingsState.active, defaults);
    }
#ifdef FALLBACK_TEST
    for (SettingItem& s : kSettings) {
        if (s.value != settings.netSsid) continue;
        restoreDefault(s, defaults);  // the test build must miss the network
        s.origin = ORIGIN_DEFAULT;
    }
#endif
    static const char* const portNames[NUM_COM] = PORT_NAMES;  // for the web log
    int changed = 0, invalid = 0;
    for (size_t i = 0; i < kSettingCount; i++) {
        const SettingItem& s = kSettings[i];
        if (s.origin == ORIGIN_STORED) changed++;
        if (s.origin != ORIGIN_INVALID) continue;
        invalid++;
        const int n = portOf(s);
        const char* group = n >= 0 ? portNames[n] : supplyKey(s) ? "supply voltage" : chipKey(s) ? "chip temperature" : "";
        LOGW(LOG_SETTINGS, "Stored %s%s%s is invalid; default in use", group, *group ? " " : "", settingName(s));
    }
    LOGI(LOG_SETTINGS, "Settings loaded: %d changed, %d invalid", changed, invalid);
#endif
}

#endif  // ESP32
