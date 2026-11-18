// Project: web log (firmware plan step 3c, R17). ESP32 only.
//
// Lines from the whole firmware go into two ring buffers in RAM: one for
// Error, Warning and Info, one for Debug, so debug chatter cannot push out
// events. Each line is copied under a short spinlock; nothing blocks and
// nothing goes to a UART. The buffers sit in .noinit RAM, so they survive a
// restart, crash or watchdog reset (not a power cut): the next start continues
// them. log_page.h serves them on /log.
//
//   LOGE / LOGW / LOGI / LOGD(source, printf format, ...)
//
// Included by bridge.ino before settings.h, so every module can log. Without
// WEB_LOG the macros print to `debug` as before (nothing while DEBUG is off).

#pragma once

#include <stdarg.h>

enum LogSource : uint8_t { LOG_WIFI, LOG_PORTS, LOG_RADIO, LOG_SETTINGS, LOG_SYSTEM, LOG_SOURCES };

#if defined(ESP32) && defined(WEB_LOG)

#include <esp_attr.h>
#include <esp_log.h>
#include <esp_system.h>

static const char* const kLogSource[LOG_SOURCES] = {"wifi", "ports", "radio", "settings", "system"};

#define LOG_TEXT 100             // characters per line; longer lines are cut
#define LOG_EVENTS 96            // Error, Warning and Info lines kept
#define LOG_DEBUG 48             // Debug lines kept
#define LOG_MAGIC 0x4C4F4731u    // "LOG1"; change it whenever LogStore changes

struct LogLine {
    uint32_t seq;  // 1, 2, 3 ... across both rings and restarts; 0 = empty
    uint32_t ms;   // millis() of its start
    uint8_t boot;  // start number, counts up at every start
    char level;    // 'E', 'W', 'I', 'D'
    uint8_t src;   // LogSource
    uint8_t len;
    char text[LOG_TEXT];
};

template <size_t N>
struct LogRing {
    uint32_t head;     // lines written
    uint32_t dropped;  // highest seq overwritten, 0 = none
    LogLine line[N];
};

struct LogStore {
    uint32_t magic;
    uint32_t seq;  // last seq given out
    uint8_t boot;
    LogRing<LOG_EVENTS> ev;
    LogRing<LOG_DEBUG> dbg;
};

static __NOINIT_ATTR LogStore logStore;  // kept across a software restart
static portMUX_TYPE logLock = portMUX_INITIALIZER_UNLOCKED;
static bool logReady = false;

template <size_t N>
static void logCheckRing(LogRing<N>& r, uint32_t seq) {  // drop what a reset left damaged
    if (r.dropped > seq) r.dropped = 0;
    for (LogLine& l : r.line) {
        const bool ok = l.seq <= seq && l.len <= LOG_TEXT && l.src < LOG_SOURCES &&
                        (l.level == 'E' || l.level == 'W' || l.level == 'I' || l.level == 'D');
        if (!ok || l.seq == 0) memset(&l, 0, sizeof(l));
    }
}

// Once, before the first line: keep the lines of the previous start if they look sound.
static void logBegin() {
    if (logReady) return;
    if (esp_reset_reason() == ESP_RST_POWERON || logStore.magic != LOG_MAGIC || logStore.seq >= 0x80000000u) {
        memset(&logStore, 0, sizeof(logStore));
        logStore.magic = LOG_MAGIC;
    } else {
        logCheckRing(logStore.ev, logStore.seq);
        logCheckRing(logStore.dbg, logStore.seq);
    }
    logStore.boot++;
    logReady = true;
}

template <size_t N>
static void logPut(LogRing<N>& r, const LogLine& l) {
    LogLine& slot = r.line[r.head++ % N];
    if (slot.seq > r.dropped) r.dropped = slot.seq;
    slot = l;
}

static void logWrite(char level, uint8_t src, const char* text, size_t len) {
    if (!logReady) logBegin();
    if (len > LOG_TEXT) len = LOG_TEXT;
    while (len && (text[len - 1] == '\n' || text[len - 1] == '\r' || text[len - 1] == ' ')) len--;
    if (len == 0) return;
    LogLine l;
    l.ms = millis();
    l.level = level;
    l.src = src;
    l.len = (uint8_t)len;
    memcpy(l.text, text, len);
    portENTER_CRITICAL(&logLock);
    l.seq = ++logStore.seq;
    l.boot = logStore.boot;
    if (level == 'D') logPut(logStore.dbg, l);
    else logPut(logStore.ev, l);
    portEXIT_CRITICAL(&logLock);
}

static void logMsg(char level, uint8_t src, const char* fmt, ...) __attribute__((format(printf, 3, 4)));
static void logMsg(char level, uint8_t src, const char* fmt, ...) {
    char buf[LOG_TEXT + 1];
    va_list ap;
    va_start(ap, fmt);
    const int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n > 0) logWrite(level, src, buf, n < (int)sizeof(buf) ? n : LOG_TEXT);
}

#define LOGE(src, ...) logMsg('E', (src), __VA_ARGS__)
#define LOGW(src, ...) logMsg('W', (src), __VA_ARGS__)
#define LOGI(src, ...) logMsg('I', (src), __VA_ARGS__)
#define LOGD(src, ...) logMsg('D', (src), __VA_ARGS__)

// The upstream debug prints: one Debug line per text line, source System.
// Only the bridge loop task prints through it.
class LogPrint : public Print {
   public:
    size_t write(uint8_t c) override {
        if (c == '\r') return 1;
        if (c == '\n' || n == LOG_TEXT) {
            logWrite('D', LOG_SYSTEM, line, n);
            n = 0;
            if (c == '\n') return 1;
        }
        line[n++] = (char)c;
        return 1;
    }

   private:
    char line[LOG_TEXT];
    size_t n = 0;
};
static LogPrint debug;

// ESP-IDF log lines ("E (1234) tag: text"), redirected so none goes out of UART0.
static int logVprintf(const char* fmt, va_list ap) {
    char buf[160];
    const int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    if (n <= 0) return n;
    const size_t len = n < (int)sizeof(buf) ? n : sizeof(buf) - 1;
    char text[LOG_TEXT];
    size_t m = 0;
    for (size_t i = 0; i < len && m < LOG_TEXT; i++) {
        if (buf[i] == '\033') {  // colour code: ESC [ ... m
            while (i < len && buf[i] != 'm') i++;
            continue;
        }
        if (buf[i] != '\r' && buf[i] != '\n') text[m++] = buf[i];
    }
    char level = 'E';
    size_t start = 0;
    if (m > 2 && text[1] == ' ' && strchr("EWIDV", text[0])) {
        level = text[0] == 'E' ? 'E' : text[0] == 'W' ? 'W' : 'D';
        start = 2;
        if (start < m && text[start] == '(') {  // skip the time stamp "(1234) "
            while (start < m && text[start] != ')') start++;
            start += 2;
        }
        if (start > m) start = m;
    }
    logWrite(level, LOG_SYSTEM, text + start, m - start);
    return n;
}

static const char* resetText(esp_reset_reason_t r) {
    switch (r) {
        case ESP_RST_POWERON: return "power on";
        case ESP_RST_EXT: return "reset pin";
        case ESP_RST_SW: return "restart by the firmware";
        case ESP_RST_PANIC: return "crash";
        case ESP_RST_INT_WDT: return "interrupt watchdog";
        case ESP_RST_TASK_WDT: return "task watchdog";
        case ESP_RST_WDT: return "watchdog";
        case ESP_RST_DEEPSLEEP: return "wake from deep sleep";
        case ESP_RST_BROWNOUT: return "brownout, the supply voltage dropped";
        default: return "unknown";
    }
}

// Wi-Fi events from the core's event task.
static void logWifiEvent(arduino_event_id_t event, arduino_event_info_t info) {
    static bool joined = false;
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            joined = true;
            LOGI(LOG_WIFI, "Connected to the network, address %s", IPAddress(info.got_ip.ip_info.ip.addr).toString().c_str());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
            const uint8_t reason = info.wifi_sta_disconnected.reason;
            const char* why = WiFi.disconnectReasonName((wifi_err_reason_t)reason);
            if (joined && reason == WIFI_REASON_ASSOC_LEAVE) LOGI(LOG_WIFI, "Left the network");  // the bridge itself, e.g. before a restart
            else if (joined) LOGW(LOG_WIFI, "Network connection lost: %s (%u)", why, reason);
            else LOGD(LOG_WIFI, "Network not joined: %s (%u)", why, reason);
            joined = false;
            break;
        }
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            LOGI(LOG_WIFI, "A phone joined the hotspot, %u connected", WiFi.softAPgetStationNum());
            break;
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            LOGI(LOG_WIFI, "A phone left the hotspot, %u connected", WiFi.softAPgetStationNum());
            break;
        default:
            break;
    }
}

// First thing in setup().
static void logStart() {
    logBegin();
    esp_log_set_vprintf(logVprintf);
    WiFi.onEvent(logWifiEvent);
    const esp_reset_reason_t r = esp_reset_reason();
    const bool bad = r == ESP_RST_PANIC || r == ESP_RST_INT_WDT || r == ESP_RST_TASK_WDT || r == ESP_RST_WDT ||
                     r == ESP_RST_BROWNOUT;
    logMsg(bad ? 'W' : 'I', LOG_SYSTEM, "Start, firmware %s, reset: %s", VERSION, resetText(r));
}

#else  // without the web log: the lines go to the debug output, as the upstream prints

#define LOG_DEBUG_PRINT(src, fmt, ...) (debug.printf(fmt "\n", ##__VA_ARGS__))
#define LOGE(src, fmt, ...) LOG_DEBUG_PRINT(src, fmt, ##__VA_ARGS__)
#define LOGW(src, fmt, ...) LOG_DEBUG_PRINT(src, fmt, ##__VA_ARGS__)
#define LOGI(src, fmt, ...) LOG_DEBUG_PRINT(src, fmt, ##__VA_ARGS__)
#define LOGD(src, fmt, ...) LOG_DEBUG_PRINT(src, fmt, ##__VA_ARGS__)
static inline void logStart() {}

#endif
