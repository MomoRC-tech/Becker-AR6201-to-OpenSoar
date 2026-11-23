// Project: supply voltage (firmware plan step 3d, R18, ADR-07). ESP32 only.
//
// Measures the 12 V input through the divider on SUPPLY_PIN (docs/hardware/power.md),
// sends it to OpenSoar as an OpenVario sentence "$POV,V,12.62*hh" once per second on
// its own TCP port, keeps one point per minute (lowest, mean, highest) for the chart
// on the status page, and writes low-voltage lines to the web log.
//
// Everything here runs in the status task (core 0, low priority): reading the ADC,
// the TCP port and the web routes. The forwarding loop is not touched, and the
// forwarded device ports never carry these sentences.
//
// The minute points sit in .noinit RAM like the web log, so the chart keeps the
// whole time the bridge is on through a restart (not a power cut).
//
// Included by status_page.h after statusServer, before the page and statusJson().

#pragma once

#ifdef SUPPLY_SENSE

#include <esp_attr.h>
#include <esp_system.h>
#include <lwip/sockets.h>
#include <cerrno>
#include <cmath>

#define SUPPLY_BLOCK_MS 100   // one reading: the mean of SUPPLY_SAMPLES ADC samples
#define SUPPLY_SAMPLES 16
#define SUPPLY_MAGIC 0x53555031u  // "SUP1"; change it whenever SupplyHistory changes
#define SUPPLY_NO_READING_MS 10000UL  // sensor on but no plausible reading this long: one warning

struct SupplyPoint {
    uint16_t lo, mean, hi;  // centivolts; 0 = no reading in that minute
};

struct SupplyHistory {
    uint32_t magic;
    uint16_t head;   // next slot
    uint16_t count;  // points kept, up to SUPPLY_HISTORY_MIN
    SupplyPoint point[SUPPLY_HISTORY_MIN];
};

static __NOINIT_ATTR SupplyHistory supplyHistory;  // kept across a software restart

struct SupplyNow {
    float volts = NAN;     // last second, calibrated; NAN = no reading
    float rawVolts = NAN;  // the same without the calibration factor
    uint32_t sentences = 0;
    uint32_t skipped = 0;  // sentences an app could not take
};
static SupplyNow supplyNow;

// Current minute
static float supplyMinSum = 0;
static uint16_t supplyMinCount = 0;
static float supplyMinLo = 0, supplyMinHi = 0;
static uint32_t supplyMinuteAt = 0;  // millis() when the current minute began
static uint32_t supplyMinuteEndAt = 0;  // millis() when the newest point was closed, 0 = none this start

// OpenVario port
static WiFiServer supplyServer(SUPPLY_TCP_PORT);
struct SupplyApp {
    WiFiClient client;
    uint32_t progressAt;  // last time it took a sentence
    uint8_t pendLen;      // unsent tail of the last sentence
    char pend[24];
};
static SupplyApp supplyApps[MAX_NMEA_CLIENTS];

static uint16_t supplyCentivolts(float v) {
    const long c = lroundf(v * 100.0f);
    return c < 1 ? 1 : c > 65535 ? 65535 : (uint16_t)c;
}

static void supplyHistoryBegin() {
    if (esp_reset_reason() == ESP_RST_POWERON || supplyHistory.magic != SUPPLY_MAGIC ||
        supplyHistory.head >= SUPPLY_HISTORY_MIN || supplyHistory.count > SUPPLY_HISTORY_MIN) {
        memset(&supplyHistory, 0, sizeof(supplyHistory));
        supplyHistory.magic = SUPPLY_MAGIC;
    }
}

static void supplyCloseMinute() {
    SupplyPoint p = {0, 0, 0};
    if (supplyMinCount) {
        p.lo = supplyCentivolts(supplyMinLo);
        p.mean = supplyCentivolts(supplyMinSum / supplyMinCount);
        p.hi = supplyCentivolts(supplyMinHi);
    }
    supplyHistory.point[supplyHistory.head] = p;
    supplyHistory.head = (supplyHistory.head + 1) % SUPPLY_HISTORY_MIN;
    if (supplyHistory.count < SUPPLY_HISTORY_MIN) supplyHistory.count++;
    supplyMinSum = 0;
    supplyMinCount = 0;
    supplyMinuteEndAt = millis() | 1u;
}

// ---- OpenVario port -------------------------------------------------------------

static void supplyAppClose(SupplyApp& a, const char* why) {
    LOGI(LOG_PORTS, "Supply voltage: app %s %s", a.client.remoteIP().toString().c_str(), why);
    a.client.stop();
    a.pendLen = 0;
}

static uint32_t supplyAppCount() {
    uint32_t n = 0;
    for (SupplyApp& a : supplyApps)
        if (a.client.fd() >= 0) n++;
    return n;
}

static void supplyServeApps() {
    if (supplyServer.hasClient()) {
        WiFiClient c = supplyServer.accept();
        SupplyApp* slot = nullptr;
        for (SupplyApp& a : supplyApps)
            if (a.client.fd() < 0) { slot = &a; break; }
        if (slot && c.fd() >= 0) {
            appKeepalive(c);  // 0.5.17: a vanished app is closed after about 14 s
            slot->client = c;
            slot->client.setNoDelay(true);
            slot->progressAt = millis();
            slot->pendLen = 0;
            LOGI(LOG_PORTS, "Supply voltage: app %s connected, %lu connected", c.remoteIP().toString().c_str(),
                 (unsigned long)supplyAppCount());
        } else {
            c.stop();
        }
    }
    for (SupplyApp& a : supplyApps) {
        const int fd = a.client.fd();
        if (fd < 0) continue;
        uint8_t in[64];  // OpenSoar sends MacCready, ballast, bugs and polar: read and dropped
        int r;
        while ((r = recv(fd, in, sizeof(in), MSG_DONTWAIT)) > 0) {}
        if (r == 0) supplyAppClose(a, "closed the connection");
        else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) supplyAppClose(a, "connection lost");
    }
}

// Sends what fits now; false if the connection is lost.
static bool supplySend(SupplyApp& a, const char* s, size_t len) {
    const int sent = send(a.client.fd(), s, len, MSG_DONTWAIT);
    if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR && errno != ENOMEM) return false;
    const size_t n = sent > 0 ? (size_t)sent : 0;
    if (n > 0) a.progressAt = millis();
    a.pendLen = (uint8_t)(len - n);
    memmove(a.pend, s + n, a.pendLen);
    return true;
}

static void supplySendSentence(float volts) {
    char body[24], line[32];
    snprintf(body, sizeof(body), "POV,V,%.2f", volts);
    uint8_t sum = 0;
    for (const char* p = body; *p; p++) sum ^= (uint8_t)*p;
    const int len = snprintf(line, sizeof(line), "$%s*%02X\r\n", body, sum);
    for (SupplyApp& a : supplyApps) {
        if (a.client.fd() < 0) continue;
        bool ok = true;
        if (a.pendLen) ok = supplySend(a, a.pend, a.pendLen);  // finish the last sentence first
        if (ok && a.pendLen == 0) {
            ok = supplySend(a, line, len);
            supplyNow.sentences++;
        } else if (ok) {
            supplyNow.skipped++;  // the app is behind: skip this one, as on the device ports
        }
        if (!ok) supplyAppClose(a, "connection lost");
        else if (millis() - a.progressAt >= APP_STALL_CLOSE_MS) supplyAppClose(a, "took no data for a minute, closed");
    }
}

// ---- measurement ----------------------------------------------------------------

static void supplySecond(float volts, float raw, bool ok) {
    static bool first = true, lowLogged = false;
    static uint32_t lowSince = 0, badSince = 0;
    supplyNow.volts = ok ? volts : NAN;
    supplyNow.rawVolts = raw;
    const uint32_t now = millis();
    if (!ok) {
        if (badSince == 0) badSince = now | 1u;
        else if (badSince != 1 && now - badSince >= SUPPLY_NO_READING_MS) {
            LOGW(LOG_SYSTEM, "Supply voltage: no plausible reading (%.1f V); check the divider or switch the sensor off", raw);
            badSince = 1;  // once until a good reading
        }
        lifetimeSupply(NAN);  // Project: lifetime record (step 3f)
        return;
    }
    badSince = 0;
    lifetimeSupply(volts);
    if (first) {
        LOGI(LOG_SYSTEM, "Supply voltage %.2f V", volts);
        first = false;
    }
    if (volts < SUPPLY_LOW_V) {
        if (lowSince == 0) lowSince = now | 1u;  // can be now + 1: compare from the next reading on
        else if (!lowLogged && now - lowSince >= SUPPLY_LOW_MS) {
            LOGW(LOG_SYSTEM, "Supply voltage low: %.2f V for %lu s", volts, SUPPLY_LOW_MS / 1000);
            lowLogged = true;
        }
    } else {
        lowSince = 0;
        if (lowLogged && volts > SUPPLY_OK_V) {
            LOGI(LOG_SYSTEM, "Supply voltage back to %.2f V", volts);
            lowLogged = false;
        }
    }
    supplySendSentence(volts);
}

// Called from the status task loop every few milliseconds.
static void supplyTick() {
    if (!settings.supplySensor) return;
    supplyServeApps();
    static uint32_t blockAt = 0, secondAt = 0;
    static uint8_t blocks = 0, good = 0;
    static float secSum = 0, secRaw = 0;
    const uint32_t now = millis();
    if (now - blockAt < SUPPLY_BLOCK_MS) return;
    // 0.5.15: blocks on a fixed grid, so a late pass does not stretch the second. With
    // blockAt = now the lateness added up and only about 82 % of the seconds produced a
    // sentence once the status task ran at idle priority (S186).
    blockAt += SUPPLY_BLOCK_MS;
    if (now - blockAt >= SUPPLY_BLOCK_MS) blockAt = now;  // held up for longer: start afresh

    uint32_t mv = 0;
    for (int i = 0; i < SUPPLY_SAMPLES; i++) mv += analogReadMilliVolts(SUPPLY_PIN);
    const float raw = mv / (float)SUPPLY_SAMPLES * SUPPLY_DIVIDER / 1000.0f;
    const float v = raw * settings.supplyCal / 1000.0f;
    const bool ok = v >= SUPPLY_MIN_V && v <= SUPPLY_MAX_V;
    if (ok) {
        secSum += v;
        good++;
        if (supplyMinCount == 0 || v < supplyMinLo) supplyMinLo = v;
        if (supplyMinCount == 0 || v > supplyMinHi) supplyMinHi = v;
        supplyMinSum += v;
        supplyMinCount++;
    }
    secRaw += raw;
    // The second follows the clock, not the number of blocks: under heavy network load the
    // status task (idle priority) can miss blocks, and the sentence must still come each second.
    ++blocks;
    if (secondAt == 0) secondAt = now;
    if (now - secondAt >= 1000) {
        secondAt += 1000;
        if (now - secondAt >= 1000) secondAt = now;  // held up for more than a second: start afresh
        supplySecond(good ? secSum / good : NAN, secRaw / blocks, good == blocks);
        blocks = good = 0;
        secSum = secRaw = 0;
    }
    if (now - supplyMinuteAt >= 60000UL) {
        supplyMinuteAt += 60000UL;
        if (now - supplyMinuteAt >= 60000UL) supplyMinuteAt = now;  // the task was held up: start afresh
        supplyCloseMinute();
    }
}

// ---- web --------------------------------------------------------------------------

static void supplyNumber(String& out, float v) {  // JSON number with two decimals, or null
    if (std::isnan(v)) { out += "null"; return; }
    char b[16];
    snprintf(b, sizeof(b), "%.2f", v);
    out += b;
}

// Part of /api/status.
static void supplyStatusJson(String& out) {
    out += ",\"supply\":{\"sensor\":";
    out += settings.supplySensor ? "true" : "false";
    out += ",\"volts\":";
    supplyNumber(out, settings.supplySensor ? supplyNow.volts : NAN);
    out += ",\"raw_volts\":";
    supplyNumber(out, settings.supplySensor ? supplyNow.rawVolts : NAN);
    char b[160];
    snprintf(b, sizeof(b), ",\"calibration\":%u.%03u,\"tcp_port\":%u,\"tcp_clients\":%lu,\"sentences\":%lu,\"skipped\":%lu,\"low_v\":%.1f}",
             settings.supplyCal / 1000, settings.supplyCal % 1000, settings.supplyTcp, (unsigned long)supplyAppCount(),
             (unsigned long)supplyNow.sentences, (unsigned long)supplyNow.skipped, SUPPLY_LOW_V);
    out += b;
}

static void supplyPointJson(String& out, const SupplyPoint& p) {
    if (p.mean == 0) { out += "null"; return; }
    char b[24];
    snprintf(b, sizeof(b), "[%u,%u,%u]", p.lo, p.mean, p.hi);
    out += b;
}

// /api/supply: the minute points, oldest first, in centivolts [lowest, mean, highest];
// null = no reading that minute. "current" is the minute still running.
static void supplyHistoryJson() {
    statusServer.sendHeader("Cache-Control", "no-store");
    statusServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
    statusServer.send(200, "application/json", "");
    const uint32_t now = millis();
    String out;
    out.reserve(1200);
    char b[160];
    const long age = supplyMinuteEndAt ? (long)((now - supplyMinuteEndAt) / 1000) : (long)((now - supplyMinuteAt) / 1000);
    snprintf(b, sizeof(b), "{\"sensor\":%s,\"interval_s\":60,\"low_v\":%.1f,\"age_s\":%ld,\"current\":",
             settings.supplySensor ? "true" : "false", SUPPLY_LOW_V, age);
    out += b;
    SupplyPoint cur = {0, 0, 0};
    if (supplyMinCount) {
        cur.lo = supplyCentivolts(supplyMinLo);
        cur.mean = supplyCentivolts(supplyMinSum / supplyMinCount);
        cur.hi = supplyCentivolts(supplyMinHi);
    }
    supplyPointJson(out, cur);
    out += ",\"points\":[";
    const uint16_t count = supplyHistory.count;
    const uint16_t first = (supplyHistory.head + SUPPLY_HISTORY_MIN - count) % SUPPLY_HISTORY_MIN;
    for (uint16_t i = 0; i < count; i++) {
        if (i) out += ",";
        supplyPointJson(out, supplyHistory.point[(first + i) % SUPPLY_HISTORY_MIN]);
        if (out.length() > 1000) {
            statusServer.sendContent(out);
            out = "";
        }
    }
    out += "]}";
    statusServer.sendContent(out);
    statusServer.sendContent("");  // end of the chunked answer
}

static void supplyRoutes() { statusServer.on("/api/supply", HTTP_GET, supplyHistoryJson); }

// Once, at the start of the status task.
static void supplyBegin() {
    supplyHistoryBegin();
    supplyMinuteAt = millis();
    if (!settings.supplySensor) {
        LOGI(LOG_SYSTEM, "Supply voltage sensor switched off in the settings");
        return;
    }
    analogSetPinAttenuation(SUPPLY_PIN, ADC_11db);
    supplyServer.begin(settings.supplyTcp);
    supplyServer.setNoDelay(true);
}

#else

static inline void supplyStatusJson(String&) {}
static inline void supplyRoutes() {}
static inline void supplyBegin() {}
static inline void supplyTick() {}

#endif
