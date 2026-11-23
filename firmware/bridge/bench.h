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
static bool benchNoRevert = false; // the other slot holds an earlier bench build, not a normal one

// Identifies this image, so a new bench upload starts its own countdown.
static const char* benchBuildId() { return __DATE__ " " __TIME__; }

static void benchSave() {
    Preferences p;
    if (!p.begin(BENCH_NS, false)) return;
    p.putString("id", benchBuildId());
    p.putUInt("min", benchMinutes);
    p.putBool("norev", benchNoRevert);
    p.end();
}

// Once at start-up, after the web log is ready. Normal builds erase the "bench"
// namespace (benchForget), so a stored id from another build means the image in
// the other slot is a bench build too: reverting to it would never end.
static void benchBegin() {
    Preferences p;
    if (p.begin(BENCH_NS, true)) {
        const String id = p.getString("id", "");
        if (id == benchBuildId()) {
            benchMinutes = p.getUInt("min", 0);
            benchNoRevert = p.getBool("norev", false);
        } else if (id.length()) {
            benchNoRevert = true;
        }
        p.end();
    }
    if (benchMinutes == 0) benchSave();
    if (benchNoRevert)
        LOGW(LOG_SYSTEM, "Bench build after another bench build: no automatic revert; upload a normal build");
    else
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
    if (benchMinutes < BENCH_REVERT_MIN || benchNoRevert) return;
    benchSave();
    LOGW(LOG_SYSTEM, "Bench build: time is up, starting the previous image");
    const esp_partition_t* previous = esp_ota_get_next_update_partition(nullptr);
    if (previous != nullptr) esp_ota_set_boot_partition(previous);
    delay(500);
    ESP.restart();
}

// GET /api/coredump: the core dump the last panic or watchdog reset left in the
// coredump partition (ELF format, CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH), for
// `esp-coredump info_corefile -c dump.elf bridge.ino.elf`. 404 when there is none.
#include <WebServer.h>
#include <esp_core_dump.h>
#include <esp_partition.h>

template <typename Server>
static void benchRoutes(Server& srv) {
    srv.on("/api/coredump", HTTP_GET, [&srv]() {
        size_t addr = 0, size = 0;
        const esp_partition_t* part =
            esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_COREDUMP, nullptr);
        if (!part || esp_core_dump_image_get(&addr, &size) != ESP_OK || size == 0 || size > part->size) {
            srv.send(404, "text/plain", "No core dump stored");
            return;
        }
        srv.setContentLength(size);
        srv.send(200, "application/octet-stream", "");
        uint8_t buf[1024];
        for (size_t off = 0; off < size; off += sizeof(buf)) {
            const size_t n = size - off < sizeof(buf) ? size - off : sizeof(buf);
            if (esp_partition_read(part, addr - part->address + off, buf, n) != ESP_OK) break;
            srv.client().write(buf, n);
            vTaskDelay(1);
        }
    });
}

#else

#include <Preferences.h>

// Normal builds: forget a bench build's countdown, so a later bench build knows
// that the image in the other slot is a normal one (bench.h, benchBegin).
static void benchForget() {
    Preferences p;
    if (!p.begin("bench", true)) return;  // no bench build ever ran
    const bool had = p.isKey("id");
    p.end();
    if (had && p.begin("bench", false)) {
        p.clear();
        p.end();
    }
}

#endif
