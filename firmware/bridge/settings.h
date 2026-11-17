// Project: settings stored on the ESP32 (firmware plan step 3a, R15). ESP32 only.
//
// The values in config.h and credentials.h are the defaults. The NVS
// namespace "bridge" holds only values changed on the settings page. At
// start-up every stored value is checked; an invalid one falls back to its
// default and is marked, so a bad entry can never stop the bridge.
//
// The table kSettings drives loading, display and (later) the settings page.
// Included by bridge.ino after config.h and before status_page.h.

#pragma once

#ifdef ESP32

#include <Preferences.h>

#define SETTINGS_NAMESPACE "bridge"
#define SETTINGS_FORMAT 1  // stored key "v"; another value means: ignore everything stored

enum SettingOrigin : uint8_t { ORIGIN_DEFAULT, ORIGIN_STORED, ORIGIN_INVALID };
enum SettingType : uint8_t { T_BAUD, T_FRAME, T_TCP, T_UDP, T_SSID, T_NETPASS, T_APPASS, T_SECONDS, T_IP };

struct PortSettings {
    uint32_t baud;
    char frame[4];  // data bits, parity, stop bits, for example "8N1"
    uint16_t tcpPort;
    uint16_t udpPort;  // 0 = no UDP
};

struct BridgeSettings {
    PortSettings port[NUM_COM];
    char netSsid[33];      // network to join
    char netPass[64];      // empty = open network
    uint16_t joinSeconds;  // network join time before the hotspot opens
    char hotspotSsid[33];
    char hotspotPass[64];
    uint32_t hotspotIp;    // IPAddress as uint32_t
};

static BridgeSettings settings;  // active values, fixed after settingsLoad()
static bool settingsFormatMismatch = false;

// Trial (confirm or revert): Wi-Fi, hotspot and port changes apply after a restart
// and must be kept on the settings page within SETTINGS_TRIAL_MS, otherwise the
// previous values come back from the backup namespace.
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
    {"b0", T_BAUD, &settings.port[0].baud, ORIGIN_DEFAULT},
    {"f0", T_FRAME, settings.port[0].frame, ORIGIN_DEFAULT},
    {"t0", T_TCP, &settings.port[0].tcpPort, ORIGIN_DEFAULT},
    {"u0", T_UDP, &settings.port[0].udpPort, ORIGIN_DEFAULT},
    {"b1", T_BAUD, &settings.port[1].baud, ORIGIN_DEFAULT},
    {"f1", T_FRAME, settings.port[1].frame, ORIGIN_DEFAULT},
    {"t1", T_TCP, &settings.port[1].tcpPort, ORIGIN_DEFAULT},
    {"u1", T_UDP, &settings.port[1].udpPort, ORIGIN_DEFAULT},
    {"b2", T_BAUD, &settings.port[2].baud, ORIGIN_DEFAULT},
    {"f2", T_FRAME, settings.port[2].frame, ORIGIN_DEFAULT},
    {"t2", T_TCP, &settings.port[2].tcpPort, ORIGIN_DEFAULT},
    {"u2", T_UDP, &settings.port[2].udpPort, ORIGIN_DEFAULT},
    {"ns", T_SSID, settings.netSsid, ORIGIN_DEFAULT},
    {"np", T_NETPASS, settings.netPass, ORIGIN_DEFAULT},
    {"nj", T_SECONDS, &settings.joinSeconds, ORIGIN_DEFAULT},
    {"hs", T_SSID, settings.hotspotSsid, ORIGIN_DEFAULT},
    {"hp", T_APPASS, settings.hotspotPass, ORIGIN_DEFAULT},
    {"ha", T_IP, &settings.hotspotIp, ORIGIN_DEFAULT},
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

static bool validBaud(uint32_t b) {
    static const uint32_t allowed[] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400};
    for (uint32_t a : allowed)
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

// Check one value of the given type at v; portNum is -1 for values that belong to no port.
static bool validAt(SettingType type, const void* v, int portNum) {
    switch (type) {
        case T_BAUD: return validBaud(*(const uint32_t*)v);
        case T_FRAME: return validFrame((const char*)v);
        case T_TCP: {
            const uint16_t p = *(const uint16_t*)v;
            return p != 0 && p != 80;  // 80 is the status page
        }
        case T_UDP: {
            const uint16_t p = *(const uint16_t*)v;
            if (p == 0) return true;                // no UDP
            if (portNum == RADIO_COM) return false;  // never UDP on the radio port
            return p != 53 && p != 67 && p != 68 && p != 3232 && p != 5353;  // DNS, DHCP, OTA, mDNS
        }
        case T_SSID: return validText((const char*)v, 1, 32);
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
    }
    return false;
}

static bool validValue(const SettingItem& s, int portNum) { return validAt(s.type, s.value, portNum); }

static int portOf(const SettingItem& s) {  // port number for per-port keys, -1 otherwise
    const char c = s.key[1];
    return (s.key[2] == 0 && c >= '0' && c < '0' + NUM_COM && strchr("bftu", s.key[0])) ? c - '0' : -1;
}

// ---- display -------------------------------------------------------------------

static bool settingSecret(const SettingItem& s) { return s.type == T_NETPASS || s.type == T_APPASS; }

// Short name of the value, without the port: "baud", "TCP port", "network" ...
static const char* settingName(const SettingItem& s) {
    switch (s.type) {
        case T_BAUD: return "baud";
        case T_FRAME: return "frame";
        case T_TCP: return "TCP port";
        case T_UDP: return "UDP port";
        case T_SSID: return s.value == settings.netSsid ? "network" : "hotspot name";
        case T_NETPASS: return "network password";
        case T_APPASS: return "hotspot password";
        case T_SECONDS: return "network join time, s";
        case T_IP: return "hotspot address";
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
        case T_UDP:
        case T_SECONDS: snprintf(out, n, "%u", (unsigned)*(uint16_t*)s.value); break;
        case T_IP: snprintf(out, n, "%s", IPAddress(*(uint32_t*)s.value).toString().c_str()); break;
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
    const uint16_t udp[NUM_COM] = {SERIAL0_UDP_PORT, SERIAL1_UDP_PORT, SERIAL2_UDP_PORT};
    for (int n = 0; n < NUM_COM; n++) {
        out.port[n].baud = baud[n];
        frameFromCode(frame[n], out.port[n].frame);
        out.port[n].tcpPort = tcp[n];
        out.port[n].udpPort = udp[n];
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
}

static size_t valueSize(SettingType t) {
    switch (t) {
        case T_BAUD:
        case T_IP: return 4;
        case T_TCP:
        case T_UDP:
        case T_SECONDS: return 2;
        case T_FRAME: return 4;
        case T_SSID: return 33;
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
            if (p.getType(s.key) != PT_U32) return false;
            *(uint32_t*)s.value = p.getUInt(s.key);
            return true;
        case T_TCP:
        case T_UDP:
        case T_SECONDS:
            if (p.getType(s.key) != PT_U16) return false;
            *(uint16_t*)s.value = p.getUShort(s.key);
            return true;
        default: {  // text
            const size_t cap = valueSize(s.type);
            const size_t len = p.getStringLength(s.key);  // includes the terminating zero
            if (p.getType(s.key) != PT_STR || len == 0 || len > cap) return false;
            return p.getString(s.key, (char*)s.value, cap) > 0;
        }
    }
}

// Two ports with the same TCP port, or two with the same UDP port, cannot all
// listen: put the clashing ones back to their defaults (which never clash) and mark them.
static void settingsCrossCheck(const BridgeSettings& defaults) {
    for (int round = 0; round < NUM_COM; round++) {
        bool changed = false;
        for (int a = 0; a < NUM_COM; a++) {
            for (int b = a + 1; b < NUM_COM; b++) {
                const PortSettings &pa = settings.port[a], &pb = settings.port[b];
                const bool tcpClash = pa.tcpPort == pb.tcpPort;
                const bool udpClash = pa.udpPort != 0 && pa.udpPort == pb.udpPort;
                if (!tcpClash && !udpClash) continue;
                for (size_t i = 0; i < kSettingCount; i++) {
                    SettingItem& s = kSettings[i];
                    const int n = portOf(s);
                    if ((n != a && n != b) || s.origin != ORIGIN_STORED) continue;
                    if ((tcpClash && s.type == T_TCP) || (udpClash && s.type == T_UDP)) {
                        restoreDefault(s, defaults);
                        s.origin = ORIGIN_INVALID;
                        changed = true;
                    }
                }
            }
        }
        if (!changed) return;
    }
}

// ---- storing (settings page) --------------------------------------------------------

static bool trialKey(const SettingItem& s) {  // changes that can cut the phone off
    return s.type == T_TCP || s.type == T_UDP || s.type == T_SSID || s.type == T_NETPASS ||
           s.type == T_APPASS || s.type == T_SECONDS || s.type == T_IP;
}

static void putValue(Preferences& p, SettingType type, const char* key, const void* v) {
    switch (type) {
        case T_BAUD:
        case T_IP: p.putUInt(key, *(const uint32_t*)v); break;
        case T_TCP:
        case T_UDP:
        case T_SECONDS: p.putUShort(key, *(const uint16_t*)v); break;
        default: p.putString(key, (const char*)v); break;
    }
}

// Copy every setting key (and the format key) from one namespace to another; keys
// missing in the source are removed in the target.
static void copyNamespace(const char* from, const char* to) {
    Preferences src, dst;
    const bool haveSrc = src.begin(from, true);
    dst.begin(to, false);
    dst.clear();
    if (haveSrc) {
        uint8_t buf[64];
        for (size_t i = 0; i < kSettingCount; i++) {
            const SettingItem& s = kSettings[i];
            if (!src.isKey(s.key)) continue;
            memset(buf, 0, sizeof(buf));
            switch (s.type) {
                case T_BAUD:
                case T_IP: *(uint32_t*)buf = src.getUInt(s.key); break;
                case T_TCP:
                case T_UDP:
                case T_SECONDS: *(uint16_t*)buf = src.getUShort(s.key); break;
                default: src.getString(s.key, (char*)buf, sizeof(buf)); break;
            }
            putValue(dst, s.type, s.key, buf);
        }
        if (src.isKey("v")) dst.putUInt("v", src.getUInt("v"));
        src.end();
    }
    dst.end();
}

// At start-up: count starts in a trial; after too many without Keep, restore the backup.
static void settingsTrialAtBoot() {
    Preferences p;
    if (!p.begin(SETTINGS_NAMESPACE, false)) return;
    if (!p.getBool("tr", false)) {
        p.end();
        return;
    }
    const uint8_t boots = p.getUChar("tc", 0) + 1;
    p.putUChar("tc", boots);
    p.end();
    if (boots > SETTINGS_TRIAL_BOOTS) {
        copyNamespace(SETTINGS_BACKUP, SETTINGS_NAMESPACE);  // also clears the trial keys
        Preferences b;
        if (b.begin(SETTINGS_BACKUP, false)) { b.clear(); b.end(); }
        return;
    }
    settingsTrial = true;
}

// Keep the trial settings: drop the backup and the trial marks.
static void settingsKeep() {
    Preferences p;
    if (p.begin(SETTINGS_NAMESPACE, false)) {
        p.remove("tr");
        p.remove("tc");
        p.end();
    }
    Preferences b;
    if (b.begin(SETTINGS_BACKUP, false)) { b.clear(); b.end(); }
    settingsTrial = false;
}

// Trial not kept in time: restore the backup (the caller restarts).
static void settingsRevert() {
    copyNamespace(SETTINGS_BACKUP, SETTINGS_NAMESPACE);
    Preferences b;
    if (b.begin(SETTINGS_BACKUP, false)) { b.clear(); b.end(); }
}

// Forget everything stored, including a trial (the caller restarts).
static void settingsReset() {
    Preferences p;
    if (p.begin(SETTINGS_NAMESPACE, false)) { p.clear(); p.end(); }
    Preferences b;
    if (b.begin(SETTINGS_BACKUP, false)) { b.clear(); b.end(); }
}

// Save a checked candidate. Values equal to the default are removed, so they show
// as default. Returns true if a trial starts with the next restart.
static bool settingsSave(const BridgeSettings& cand) {
    BridgeSettings defaults;
    fillDefaults(defaults);
    bool risky = false;
    for (size_t i = 0; i < kSettingCount; i++) {
        const SettingItem& s = kSettings[i];
        const size_t off = (const uint8_t*)s.value - (const uint8_t*)&settings;
        if (trialKey(s) && memcmp((const uint8_t*)&cand + off, s.value, valueSize(s.type)) != 0) risky = true;
    }
    Preferences p;
    if (!p.begin(SETTINGS_NAMESPACE, false)) return false;
    const bool trialRunning = p.getBool("tr", false);
    p.end();
    if (risky && !trialRunning) copyNamespace(SETTINGS_NAMESPACE, SETTINGS_BACKUP);  // keep the last good values
    p.begin(SETTINGS_NAMESPACE, false);
    const bool trial = risky || trialRunning;
    if (trial) {  // mark first: a power loss during the writes below still ends in the trial
        p.putBool("tr", true);
        p.putUChar("tc", 0);
    }
    for (size_t i = 0; i < kSettingCount; i++) {
        const SettingItem& s = kSettings[i];
        const size_t off = (const uint8_t*)s.value - (const uint8_t*)&settings;
        const void* v = (const uint8_t*)&cand + off;
        if (memcmp(v, (const uint8_t*)&defaults + off, valueSize(s.type)) == 0) p.remove(s.key);
        else putValue(p, s.type, s.key, v);
    }
    p.putUInt("v", SETTINGS_FORMAT);
    p.end();
    return trial;
}

static void settingsLoad() {
    BridgeSettings defaults;
    fillDefaults(defaults);
    settings = defaults;
    for (size_t i = 0; i < kSettingCount; i++) kSettings[i].origin = ORIGIN_DEFAULT;
#ifdef STORED_SETTINGS
    settingsTrialAtBoot();
    Preferences p;
    if (!p.begin(SETTINGS_NAMESPACE, true)) return;  // nothing stored yet
    if (p.isKey("v") && p.getUInt("v", 0) != SETTINGS_FORMAT) {
        settingsFormatMismatch = true;  // written by another firmware version: use the defaults
        p.end();
        return;
    }
    for (size_t i = 0; i < kSettingCount; i++) {
        SettingItem& s = kSettings[i];
#ifdef FALLBACK_TEST
        if (s.value == settings.netSsid) continue;  // the test build must miss the network
#endif
        if (!p.isKey(s.key)) continue;
        if (readStored(p, s) && validValue(s, portOf(s))) {
            s.origin = ORIGIN_STORED;
        } else {
            restoreDefault(s, defaults);
            s.origin = ORIGIN_INVALID;
        }
    }
    p.end();
    settingsCrossCheck(defaults);
#endif
}

#endif  // ESP32
