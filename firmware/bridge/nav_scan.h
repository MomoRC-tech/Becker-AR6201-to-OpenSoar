// The bridge loop owns the UART. The settings task submits commands and reads snapshots.
#pragma once

#include <atomic>
#include "nav_sentences.h"

enum class NavState { Idle, Listening, Measuring, Found, Quiet, Undecoded, Cancelled };
struct NavResult {
    unsigned id = 0;
    int port = -1;
    NavState state = NavState::Idle;
    uint32_t baud = 0;
    char frame[4] = "";
    NavEvidence counts;
};

static portMUX_TYPE navLock = portMUX_INITIALIZER_UNLOCKED;
static NavResult navResult;
static std::atomic<int> navPort{-1};
static std::atomic<bool> navCancel{false}, navRxError{false};

static NavResult navSnapshot() {
    portENTER_CRITICAL(&navLock);
    NavResult copy = navResult;
    portEXIT_CRITICAL(&navLock);
    return copy;
}

// Called only from the single HTTP task. Publish the port last, when the request is ready.
static unsigned navStart(int port) {
    if (navPort.load() >= 0) return 0;
    portENTER_CRITICAL(&navLock);
    unsigned id = navResult.id + 1;
    if (!id) id = 1;
    navResult = {};
    navResult.id = id;
    navResult.port = port;
    navResult.state = NavState::Listening;
    navCancel.store(false);
    navRxError.store(false);
    navPort.store(port);
    portEXIT_CRITICAL(&navLock);
    return id;
}

class NavigationScan {
    static constexpr uint32_t probeMs = 2200, evidenceMs = 5500, measureMs = 1200, limitMs = 55000;
    NavSentences parser;
    NavResult result;
    uint32_t started = 0, attempt = 0;
    unsigned rateIndex = 0;
    uint32_t estimate = 0;
    bool active = false, measuring = false, measured = false, sawBytes = false;

    void publish() {
        portENTER_CRITICAL(&navLock);
        navResult = result;
        portEXIT_CRITICAL(&navLock);
    }

    void configure(uint32_t baud, const char* frame) {
        const int p = result.port;
        COM[p]->begin(baud, frameCode(frame[0] - '0', frame[1], frame[2] - '0'),
                      p == 0 ? SERIAL0_RXPIN : SERIAL1_RXPIN, p == 0 ? SERIAL0_TXPIN : SERIAL1_TXPIN);
        uart_flush_input(uart_port_t(p));
    }

    void listen(uint32_t baud, const char* frame, uint32_t now) {
        configure(baud, frame);
        parser = {};
        result.baud = baud;
        strlcpy(result.frame, frame, sizeof(result.frame));
        result.counts = {};
        result.state = NavState::Listening;
        attempt = now;
        publish();
    }

    void stopMeasurement() {
        uart_bitrate_res_t sample{};
        measuring = false;
        if (uart_detect_bitrate_stop(uart_port_t(result.port), false, &sample) != ESP_OK) return;
        sawBytes |= sample.edge_cnt != 0;
        if (sample.edge_cnt < 8 || !sample.low_period || !sample.high_period) return;
        const uint32_t rate = 2ULL * sample.clk_freq_hz / (sample.low_period + sample.high_period);
        for (uint32_t candidate : kSerialRates)
            if (rate >= candidate * 9 / 10 && rate <= candidate * 11 / 10) estimate = candidate;
    }

    void finish(NavState state) {
        if (measuring) stopMeasurement();
        const auto& original = settings.port[result.port];
        configure(original.baud, original.frame);
        result.state = state;
        active = false;
        publish();
        navPort.store(-1);  // UART is restored before a second scan can start.
    }

    void nextRate(uint32_t now) {
        const auto& original = settings.port[result.port];
        // Index zero is the pulse estimate. Skip duplicate baud/frame pairs.
        while (rateIndex <= sizeof(kSerialRates) / sizeof(kSerialRates[0])) {
            unsigned i = rateIndex++;
            uint32_t rate = i ? kSerialRates[i - 1] : estimate;
            if (!rate || (i && rate == estimate) || (rate == original.baud && !strcmp(original.frame, "8N1"))) continue;
            listen(rate, "8N1", now);
            return;
        }
        finish(sawBytes || navRxError.load() ? NavState::Undecoded : NavState::Quiet);
    }

public:
    void tick(int port) {
        const uint32_t now = millis();
        if (!active) {
            active = true;
            result = navSnapshot();
            started = now;
            rateIndex = estimate = 0;
            measured = measuring = sawBytes = false;
            const auto& original = settings.port[port];
            listen(original.baud, original.frame, now);
        }
        if (navCancel.load()) { finish(NavState::Cancelled); return; }
        if (measuring) {
            if (now - attempt < measureMs) return;
            stopMeasurement();
            nextRate(now);
            return;
        }
        // One bounded read per loop pass leaves the radio and the other input serviceable.
        size_t n = COM[port]->read(buf2[port], BUFFERSIZE);
        sawBytes |= n != 0;
        for (size_t i = 0; i < n; ++i) parser.feed(buf2[port][i]);
        result.counts = parser.counts;
        const bool evidence = result.counts.flarm || result.counts.zander || result.counts.generic;
        if (now - attempt < (evidence ? evidenceMs : probeMs) && now - started < limitMs) return;
        if (result.counts.sufficient()) { finish(NavState::Found); return; }
        if (now - started >= limitMs) {
            finish(sawBytes || navRxError.load() ? NavState::Undecoded : NavState::Quiet);
            return;
        }
        if (!measured) {
            measured = true;
            measuring = uart_detect_bitrate_start(uart_port_t(port), nullptr) == ESP_OK;
            if (measuring) {
                result.state = NavState::Measuring;
                attempt = now;
                publish();
                return;
            }
        }
        nextRate(now);
    }
};

static NavigationScan navScan;
