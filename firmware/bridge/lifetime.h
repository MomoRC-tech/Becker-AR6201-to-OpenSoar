// Project: lifetime record (firmware plan step 3f). ESP32 only.
//
// Keeps a few values through power cuts, in NVS (namespace "lifetime", one blob):
// starts, operating time, the highest chip temperature and the lowest supply
// voltage, each with the operating time and, if the clock is known, the date it
// happened. Settings reset does not touch it; it has its own reset.
//
// Flash writes are rare and run in the status task: one at every start, then every
// 10 min, and at most once a minute while a new extreme is being set. While the
// flash is written, both cores pause code from flash and the UART interrupt waits
// (S133), so the forwarding loop must never write here.
//
// Included by status_page.h before supply.h and chiptemp.h, which report to it.

#pragma once

#ifdef LIFETIME_RECORD

#include <Preferences.h>
#include <cmath>
#include <time.h>

#define LIFE_NS "lifetime"
#define LIFE_KEY "rec"
#define LIFE_VERSION 1
#define LIFE_SAVE_MS 600000UL      // operating time saved every 10 min
#define LIFE_EXTREME_MS 60000UL    // a new extreme saved at most once a minute
#define LIFE_SUPPLY_HOLD 10        // the lowest supply voltage counts when held this many seconds
#define LIFE_CLOCK_VALID 1700000000L
#define LIFE_NONE_T INT16_MIN

struct LifeRecord {
    uint32_t version;
    uint32_t starts;
    uint32_t upS;          // operating seconds
    uint32_t sinceUnix;    // when the record was reset; 0 = not known
    int16_t maxChipDeci;   // tenths of a degree C; LIFE_NONE_T = none
    uint16_t minSupplyCv;  // centivolts; 0 = none
    uint32_t maxChipUpS, maxChipUnix;
    uint32_t minSupplyUpS, minSupplyUnix;
};

static LifeRecord life;              // the values now; operating time counted up to lifeUpAt
static uint32_t lifeUpAt = 0;        // millis() up to which life.upS is counted
static bool lifeStorage = false;     // NVS could be opened at start
static bool lifeSaveFailed = false;  // logged once
static uint32_t lifeSavedAt = 0, lifeExtremeSavedAt = 0;
static bool lifeExtremePending = false;

static uint32_t lifeNowUnix() {
    const time_t t = time(nullptr);
    return t >= LIFE_CLOCK_VALID ? (uint32_t)t : 0;
}

static void lifeClear(LifeRecord& r) {
    memset(&r, 0, sizeof(r));
    r.version = LIFE_VERSION;
    r.maxChipDeci = LIFE_NONE_T;
}

static void lifeCount() {  // add the whole seconds since lifeUpAt
    const uint32_t s = (millis() - lifeUpAt) / 1000;
    life.upS += s;
    lifeUpAt += s * 1000;
}

static void lifeSave() {
    lifeCount();
    lifeSavedAt = millis();
    bool ok = false;
    if (lifeStorage) {
        Preferences p;
        if (p.begin(LIFE_NS, false)) {
            ok = p.putBytes(LIFE_KEY, &life, sizeof(life)) == sizeof(life);
            p.end();
        }
    }
    if (!ok && lifeStorage && !lifeSaveFailed) {
        LOGW(LOG_SYSTEM, "Lifetime record: saving failed; values since then are lost at power off");
        lifeSaveFailed = true;
    } else if (ok) {
        lifeSaveFailed = false;
    }
}

// Once, at the start of the status task. Operating time counts from the ESP32 start.
static void lifetimeBegin() {
    lifeClear(life);
    Preferences p;
    if (p.begin(LIFE_NS, false)) {
        lifeStorage = true;
        LifeRecord r;
        if (p.getBytesLength(LIFE_KEY) == sizeof(r) && p.getBytes(LIFE_KEY, &r, sizeof(r)) == sizeof(r) &&
            r.version == LIFE_VERSION)
            life = r;
        p.end();
    } else {
        LOGW(LOG_SYSTEM, "Lifetime record: storage not available, values kept until power off only");
    }
    life.starts++;
    lifeSave();
    LOGI(LOG_SYSTEM, "Lifetime record: start %lu, operating time %lu h %02lu min", (unsigned long)life.starts,
         (unsigned long)(life.upS / 3600), (unsigned long)(life.upS / 60 % 60));
}

// From chiptemp.h, with each 1 min mean from 2 min after the start.
static void lifetimeChip(float c) {
    const int16_t d = (int16_t)lroundf(c * 10);
    if (life.maxChipDeci != LIFE_NONE_T && d <= life.maxChipDeci) return;
    lifeCount();
    life.maxChipDeci = d;
    life.maxChipUpS = life.upS;
    life.maxChipUnix = lifeNowUnix();
    lifeExtremePending = true;
}

// From supply.h, once per second; NAN = no plausible reading. Counts the highest
// value of the last LIFE_SUPPLY_HOLD seconds, so a short dip or the decay at
// switch-off does not count.
static void lifetimeSupply(float v) {
    static float ring[LIFE_SUPPLY_HOLD];
    static uint8_t count = 0, head = 0;
    if (std::isnan(v)) {
        count = 0;
        return;
    }
    ring[head] = v;
    head = (head + 1) % LIFE_SUPPLY_HOLD;
    if (count < LIFE_SUPPLY_HOLD) count++;
    if (count < LIFE_SUPPLY_HOLD) return;
    float held = ring[0];
    for (float x : ring) held = x > held ? x : held;
    const long c = lroundf(held * 100);
    const uint16_t cv = c < 1 ? 1 : c > 65535 ? 65535 : (uint16_t)c;
    if (life.minSupplyCv && cv >= life.minSupplyCv) return;
    lifeCount();
    life.minSupplyCv = cv;
    life.minSupplyUpS = life.upS;
    life.minSupplyUnix = lifeNowUnix();
    lifeExtremePending = true;
}

// Called from the status task loop every few milliseconds.
static void lifetimeTick() {
    const uint32_t now = millis();
    if (now - lifeSavedAt >= LIFE_SAVE_MS) {
        lifeExtremePending = false;
        lifeSave();
    } else if (lifeExtremePending && now - lifeExtremeSavedAt >= LIFE_EXTREME_MS) {
        lifeExtremePending = false;
        lifeExtremeSavedAt = now;
        lifeSave();
    }
}

// Before a restart by the firmware, so no operating time is lost.
static void lifetimeSaveNow() { lifeSave(); }

// Settings page: start a new record. This start counts as the first.
static void lifetimeReset() {
    lifeClear(life);
    life.starts = 1;
    life.sinceUnix = lifeNowUnix();
    lifeUpAt = millis();
    lifeExtremePending = false;
    lifeSave();
    LOGI(LOG_SETTINGS, "Lifetime record reset");
}

// Part of /api/status.
static void lifetimeJson(String& out) {
    lifeCount();
    char b[200];
    snprintf(b, sizeof(b), ",\"lifetime\":{\"storage\":%s,\"starts\":%lu,\"up_s\":%lu,\"since_unix\":%lu,\"max_chip\":",
             lifeStorage ? "true" : "false", (unsigned long)life.starts, (unsigned long)life.upS,
             (unsigned long)life.sinceUnix);
    out += b;
    if (life.maxChipDeci == LIFE_NONE_T) out += "null";
    else {
        snprintf(b, sizeof(b), "{\"celsius\":%.1f,\"up_s\":%lu,\"unix\":%lu}", life.maxChipDeci / 10.0f,
                 (unsigned long)life.maxChipUpS, (unsigned long)life.maxChipUnix);
        out += b;
    }
    out += ",\"min_supply\":";
    if (life.minSupplyCv == 0) out += "null";
    else {
        snprintf(b, sizeof(b), "{\"volts\":%.2f,\"up_s\":%lu,\"unix\":%lu}", life.minSupplyCv / 100.0f,
                 (unsigned long)life.minSupplyUpS, (unsigned long)life.minSupplyUnix);
        out += b;
    }
    out += "}";
}

#else

static inline void lifetimeBegin() {}
static inline void lifetimeChip(float) {}
static inline void lifetimeSupply(float) {}
static inline void lifetimeTick() {}
static inline void lifetimeSaveNow() {}
static inline void lifetimeJson(String&) {}

#endif
