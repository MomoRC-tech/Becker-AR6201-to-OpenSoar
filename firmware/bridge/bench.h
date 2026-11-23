// Project: bench build without settings login (BENCH_OPEN), for unattended bench tests.
// Built by `python tools/build_bridge.py bench-open`; never for the aircraft.
//
// The settings page and its API accept every request without a password, and the
// firmware version ends in "-BENCH". The build counts its own operating minutes in
// NVS, across restarts and power cuts. After BENCH_REVERT_MIN it boots the other app
// slot (the image that ran before the upload) and restarts, so a forgotten bench
// build does not stay installed. An OTA upload in the meantime stops the countdown.
#pragma once

#ifdef BENCH_OPEN

#include <Preferences.h>
#include <esp_ota_ops.h>

#define BENCH_NS "bench"
#define BENCH_SAVE_MIN 15  // NVS write interval; a restart loses at most this much

static uint32_t benchMinutes = 0;  // operating minutes of this image so far

// Identifies this image, so a new bench upload starts its own countdown.
static const char* benchBuildId() { return __DATE__ " " __TIME__; }

static void benchSave() {
    Preferences p;
    if (!p.begin(BENCH_NS, false)) return;
    p.putString("id", benchBuildId());
    p.putUInt("min", benchMinutes);
    p.end();
}

// Once at start-up, after the web log is ready.
static void benchBegin() {
    Preferences p;
    if (p.begin(BENCH_NS, true)) {
        if (p.getString("id", "") == benchBuildId()) benchMinutes = p.getUInt("min", 0);
        p.end();
    }
    if (benchMinutes == 0) benchSave();
    LOGW(LOG_SYSTEM, "Bench build: settings without password; reverts to the previous image in %lu min",
         (unsigned long)(BENCH_REVERT_MIN > benchMinutes ? BENCH_REVERT_MIN - benchMinutes : 0));
}

// From loop(). otaStarted: an OTA upload is running, which replaces this image anyway.
static void benchLoop(bool otaStarted) {
    static uint32_t lastMs = 0;
    if (otaStarted || millis() - lastMs < 60000UL) return;
    lastMs = millis();
    if (millis() < 60000UL) return;  // first minute after start: not yet counted
    benchMinutes++;
    if (benchMinutes % BENCH_SAVE_MIN == 0) benchSave();
    if (benchMinutes < BENCH_REVERT_MIN) return;
    benchSave();
    LOGW(LOG_SYSTEM, "Bench build: time is up, starting the previous image");
    const esp_partition_t* previous = esp_ota_get_next_update_partition(nullptr);
    if (previous != nullptr) esp_ota_set_boot_partition(previous);
    delay(500);
    ESP.restart();
}

#endif
