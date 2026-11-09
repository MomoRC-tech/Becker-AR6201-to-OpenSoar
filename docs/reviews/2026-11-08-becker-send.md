# Becker send regression

S12 · 2026-11-08 · R01/R03/R04. Host reproduction. App patch, deployment and physical acceptance open.

## Sources and historical baseline

| Evidence | Reference / finding |
| --- | --- |
| Reported | [PR 189](https://github.com/XCSoar/XCSoar/pull/189): AR6201 tests on Linux/Windows; tested binary/frequencies unspecified |
| Rechecked source | [cd818c90](https://github.com/eku60/XCSoar/blob/cd818c90ebb6e803a4b7a804935d87a7575eaa6f/src/Device/Driver/AR62xx.cpp), 2019-05-03; PR head `6d615c04fedb6d243cb664f40daa0b945f4cfdb2`: counted writes |
| Rechecked source | OpenSoar 7.46.26, `b2e745cfddfe0150f6ed7db467ac18c321b38d01`: [driver](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp), [Port.hpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Port/Port.hpp), [Port.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Port/Port.cpp) |
| Rechecked source | [7a575346](https://github.com/eku60/XCSoar/commit/7a575346b71c2384420d275b335f36d3078d327d), 2024-03-05: char-pointer call replaces broken span adaptation; introduces current string-length behavior |

---

## Comparison

| Area | Historical PR source | OpenSoar 7.46.26 | Consequence |
| --- | --- | --- | --- |
| Send length | Send calls FullWrite(msg, msg_size, env, CMD_TIMEOUT). Historical Port accepts pointer plus length. | Send casts msg to const char*, omits msg_size. Port overload resolution selects std::string_view, whose pointer constructor measures to the first zero. | Demonstrated binary truncation regression. |
| Frame construction | SetAR620xStation builds ten bytes containing both channel indices and CRC. | Same frame structure and CRC polynomial. Freq2Idx now explicitly maps displayed 8.33 channel suffixes, whereas the older code uses Frq2Idx directly. | Keep current channel conversion pending independent vectors. Restoring the whole historical driver would discard changes. |
| Success | ACK is assigned locally after successful FullWrite. | ACK is assigned locally after FullWrite returns. | Historical weakness persists. Neither implementation establishes radio acceptance. |
| Active state | Both stored frequencies are read to construct the command. Constructor does not establish a valid initial frequency state. | Same dependency, stored in radio_para. | Known active state is required before using the combined command. Hardware startup behavior and a reliable refresh mechanism remain unresolved. |
| Reception | Parser resets on every A5, uses static state, accepts zero CRC and skips conversion while sending. | These behaviors remain. | Historical success reports do not cover all frame contents, reconnects or local tuning. |

---

## Measured host reproduction

Harness: [check_becker_send.py](../../tools/check_becker_send.py).

| Method | Scope |
| --- | --- |
| Extracted source | Actual Send, SetAR620xStation, Freq2Idx, Frq2Idx, CRCBitwise; Port string_view body/signatures |
| Sink | In-memory byte span; exact full-buffer equality |
| Stubs | Host mutex; no-op condition wait; known synthetic frequencies; no radio response |
| Correction | Only FullWrite call replaced with counted byte span |

| Synthetic active / requested standby (MHz) | Original 7.46.26 bytes reaching sink | Corrected Send | Original return |
| --- | --- | --- | --- |
| 118.000 / 123.500 | 4 / 10 | 10 / 10, exact byte match | true |
| 123.000 / 118.000 | 6 / 10 | 10 / 10, exact byte match | true |
| 123.000 / 124.000 | 10 / 10 | 10 / 10, exact byte match | true |
| 121.500 / 123.450 | 10 / 10 | 10 / 10, exact byte match | true |

Frame: `A5 14 05 16 00 00 03 70 53 17`; original sink: `A5 14 05 16`. Truncation precedes TCP/serial. Inference: a transparent bridge cannot restore omitted bytes.

---

## Minimum changes and open decisions

| Change | Status / required check |
| --- | --- |
| `port.FullWrite(std::as_bytes(std::span{msg, msg_size}), env, CMD_TIMEOUT);` | Host-tested correction; retain zero/zero-free regressions; app integration pending |
| Unknown/fresh active state | Startup/reconnect/local-tuning behavior; no arbitrary default frequencies |
| Response/readback | Actual protocol rules; separate transport success from confirmed state |
| Parser | Length bounds, per-instance state, payload A5, CRC exceptions, receive during send |

---

## Verification summary

Measured: WSL Ubuntu, g++ 13.3.0 (`13.3.0-6ubuntu2~24.04.1`), C++20, ASan/UBSan. Four original-outcome and four corrected-byte assertions passed; no sanitizer findings in exercised paths.

Reproduce: `python3 tools/check_becker_send.py --compiler g++` (Linux/WSL, network). GPL headers reviewed; extracted upstream bodies temporary. No full app build, live TCP, ESP32, radio or aircraft test. Results retained; not rerun for prose edits.
