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
// Save single readings at setup entry and at 10 s intervals through 120 s.
// The status task separately maintains the live average.
// Included by status_page.h after supply.h.

#pragma once

#ifdef CHIP_TEMP

#include <cmath>
#include <atomic>
#include <mutex>

extern "C" uint8_t temprature_sens_read();  // Arduino core (esp32-hal-misc.c): raw degrees Fahrenheit

#define CHIP_TEMP_EVERY_MS 1000
#define CHIP_TEMP_MEAN 10                 // readings in the shown mean
#define CHIP_TEMP_STUCK 128               // the value of a sensor that does not work
#define CHIP_TEMP_NO_READING_MS 30000UL   // no working reading this long after start: one log line
#define CHIP_TEMP_RECORD_N 60             // lifetime record: 1 min means (single readings scatter by several degrees)
#define CHIP_TEMP_RECORD_AFTER_MS 120000UL  // and only from 2 min after the start, when start-up readings run high

static float chipTempC = NAN;       // mean of the last readings in degrees Celsius; NAN = no reading
struct ChipTempSample {
    uint32_t ms;
    uint8_t raw;  // zero means no usable reading
};

static ChipTempSample chipTempRead() {
    static std::mutex sensor;
    static bool working = false;
    const std::lock_guard<std::mutex> lock(sensor);
    const uint32_t ms = millis();
    const uint8_t raw = temprature_sens_read();
    if (raw == 0 || raw == 255) return {ms, 0};
    if (raw != CHIP_TEMP_STUCK) working = true;
    return {ms, static_cast<uint8_t>(working ? raw : 0)};
}

static constexpr unsigned CHIP_BOOT_COUNT = 13;
static constexpr uint32_t CHIP_BOOT_INTERVAL_MS = 10000;
static ChipTempSample chipBootSamples[CHIP_BOOT_COUNT];
static std::atomic<unsigned> chipBootCount{0};  // slots are immutable once published

static void chipTempBootTask(void*) {
    for (unsigned slot = 1; slot < CHIP_BOOT_COUNT; ++slot) {
        const uint32_t due = slot * CHIP_BOOT_INTERVAL_MS;
        const uint32_t now = millis();
        if (due > now) vTaskDelay(pdMS_TO_TICKS(due - now));
        chipBootSamples[slot] = chipTempRead();
        chipBootCount.store(slot + 1, std::memory_order_release);
    }
    vTaskDelete(nullptr);
}

static void chipTempCaptureBoot() {
    chipBootSamples[0] = chipTempRead();
    chipBootCount.store(1, std::memory_order_release);
}

static void chipTempStartBootSeries() {
    if (xTaskCreate(chipTempBootTask, "boot-temp", 3072, nullptr, 1, nullptr) != pdPASS)
        LOGW(LOG_SYSTEM, "Cannot start boot temperature sampling");
}

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

    const uint8_t raw = chipTempRead().raw;
    if (!raw) {
        if (!count && !noneLogged && now >= CHIP_TEMP_NO_READING_MS) {
            LOGI(LOG_SYSTEM, "Chip temperature: this ESP32's sensor gives no reading");
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
        if (highSince == 0) highSince = now | 1u;  // can be now + 1: compare from the next reading on
        else if (!highLogged && now - highSince >= CHIP_TEMP_WARN_MS) {
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
    out += F(",\"chip_temp\":{\"celsius\":");
    out += std::isnan(chipTempC) ? String("null") : String(chipTempC, 1);
    out += F(",\"warn_c\":");
    out += CHIP_TEMP_WARN_C;
    out += F(",\"boot_samples\":[");
    const unsigned count = chipBootCount.load(std::memory_order_acquire);
    for (unsigned i = 0; i < count; ++i) {
        const ChipTempSample& sample = chipBootSamples[i];
        if (i) out += ',';
        out += F("{\"ms\":");
        out += sample.ms;
        out += F(",\"celsius\":");
        out += sample.raw ? String((sample.raw - 32.0f) / 1.8f, 1) : String("null");
        out += '}';
    }
    out += F("]}");
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
