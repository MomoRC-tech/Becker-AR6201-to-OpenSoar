// Project: ESP32 chip temperature (firmware plan step 3e). ESP32 only.
//
// Reads the internal temperature sensor once per second and shows the mean of the
// last 10 s on the status page, in /api/status and in the web log. Nothing else
// happens: a hot bridge keeps forwarding, because losing FLARM and radio data in
// flight is worse than running warm.
//
// On the classic ESP32 the sensor is undocumented (S131). It measures the chip,
// not the air in the housing, with an offset that differs from chip to chip, so the
// value is a trend, not an air temperature. A chip whose sensor does not work
// returns 128 (degrees Fahrenheit) at every read; that shows as "no reading".
//
// Runs in the status task (core 0, low priority), like the supply voltage.
// Included by status_page.h after supply.h.

#pragma once

#ifdef CHIP_TEMP

#include <cmath>

extern "C" uint8_t temprature_sens_read();  // Arduino core (esp32-hal-misc.c): raw degrees Fahrenheit

#define CHIP_TEMP_EVERY_MS 1000
#define CHIP_TEMP_MEAN 10                 // readings in the shown mean
#define CHIP_TEMP_STUCK 128               // the value of a sensor that does not work
#define CHIP_TEMP_NO_READING_MS 30000UL   // no working reading this long after start: one log line
#define CHIP_TEMP_RECORD_N 60             // lifetime record: 1 min means (single readings scatter by several degrees)
#define CHIP_TEMP_RECORD_AFTER_MS 120000UL  // and only from 2 min after the start, when start-up readings run high

static float chipTempC = NAN;       // mean of the last readings in degrees Celsius; NAN = no reading
static bool chipTempWorks = false;  // a value other than CHIP_TEMP_STUCK seen since start

// Called from the status task loop every few milliseconds.
static void chipTempTick() {
    static uint32_t at = 0, highSince = 0;
    static uint8_t ring[CHIP_TEMP_MEAN], count = 0, head = 0;
    static uint16_t minuteSum = 0;
    static uint8_t minuteCount = 0;
    static bool first = true, highLogged = false, noneLogged = false;
    const uint32_t now = millis();
    if (now - at < CHIP_TEMP_EVERY_MS) return;
    at = now;

    const uint8_t raw = temprature_sens_read();
    const bool edge = raw == 0 || raw == 255;  // out of the sensor's range
    if (!edge && raw != CHIP_TEMP_STUCK) chipTempWorks = true;
    if (!chipTempWorks || edge) {
        if (!chipTempWorks && !noneLogged && now >= CHIP_TEMP_NO_READING_MS) {
            LOGI(LOG_SYSTEM, "Chip temperature: this ESP32's sensor gives no reading (raw %u)", raw);
            noneLogged = true;
        }
        return;
    }

    ring[head] = raw;
    head = (head + 1) % CHIP_TEMP_MEAN;
    if (count < CHIP_TEMP_MEAN) count++;
    uint16_t sum = 0;
    for (uint8_t i = 0; i < count; i++) sum += ring[i];
    const float c = (sum / (float)count - 32.0f) / 1.8f;
    chipTempC = c;
    minuteSum += raw;  // Project: lifetime record (step 3f)
    if (++minuteCount == CHIP_TEMP_RECORD_N) {
        if (now >= CHIP_TEMP_RECORD_AFTER_MS) lifetimeChip((minuteSum / (float)CHIP_TEMP_RECORD_N - 32.0f) / 1.8f);
        minuteSum = 0;
        minuteCount = 0;
    }

    if (first && count == CHIP_TEMP_MEAN) {
        LOGI(LOG_SYSTEM, "Chip temperature %.0f \xC2\xB0" "C (ESP32 chip, approximate)", c);
        first = false;
    }
    if (c >= CHIP_TEMP_WARN_C) {
        if (highSince == 0) highSince = now | 1u;
        if (!highLogged && now - highSince >= CHIP_TEMP_WARN_MS) {
            LOGW(LOG_SYSTEM, "Chip temperature high: %.0f \xC2\xB0" "C for %lu s; the bridge keeps running", c,
                 CHIP_TEMP_WARN_MS / 1000);
            highLogged = true;
        }
    } else {
        highSince = 0;
        if (highLogged && c < CHIP_TEMP_OK_C) {
            LOGI(LOG_SYSTEM, "Chip temperature back to %.0f \xC2\xB0" "C", c);
            highLogged = false;
        }
    }
}

// Part of /api/status.
static void chipTempJson(String& out) {
    char b[64];
    if (std::isnan(chipTempC)) snprintf(b, sizeof(b), ",\"chip_temp\":{\"celsius\":null,\"warn_c\":%d}", CHIP_TEMP_WARN_C);
    else snprintf(b, sizeof(b), ",\"chip_temp\":{\"celsius\":%.1f,\"warn_c\":%d}", chipTempC, CHIP_TEMP_WARN_C);
    out += b;
}

// For the Debug line every 60 s: "; chip 47 °C", or empty without a reading.
static const char* chipTempText() {
    static char b[24];
    if (std::isnan(chipTempC)) b[0] = 0;
    else snprintf(b, sizeof(b), "; chip %.0f \xC2\xB0" "C", chipTempC);
    return b;
}

#else

static inline void chipTempTick() {}
static inline void chipTempJson(String&) {}
static inline const char* chipTempText() { return ""; }

#endif
