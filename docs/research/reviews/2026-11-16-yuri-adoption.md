# Yuri bridge: adoption review for the firmware plan

S103, 2026-11-16 · WP6 planning · R02/R03/R07/R10/R12–R14 · follows the [first source review](2026-11-08-yuri-bridge.md) (S08)

Can the bridge firmware start from [Yuri ESP-Serial-Bridge 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce), and what has to change for the RS-422 radio port and a status web page?

> [!NOTE]
> Source read of the pinned revision plus one compile of the unmodified sketch in a temporary folder. Nothing was flashed or run, and no upstream code was added to the repository. The resulting steps are in the [firmware plan](../../project/roadmap.md#firmware-plan); they need the owner's approval.

---

## Result

**Feasible.** Use the upstream sketch as the baseline and keep its forwarding loop. Replace start-up, configuration and diagnostics, which the project already has in tested form in the [OTA loader](../../../firmware/ota-loader/README.md).

- Upstream is small: one sketch of 327 lines and an 83-line `config.h`. 130f731 is still the newest commit on the default branch.
- About 90 lines carry the bridge: the client accept and forwarding loop, lines 242–327. The rest is start-up, Wi-Fi, OTA, UDP, Bluetooth and ESP8266 support.
- Starting from it saves little code. It gives a known MIT-licensed baseline, and every project change stays visible as a diff against it.
- Nothing found speaks for another candidate or a fresh start.

---

## Compile check

| Item | Value |
| --- | --- |
| Source | 130f731, unmodified, upstream `config.h` defaults (TCP, UDP and OTA on, Bluetooth off) |
| Toolchain | Arduino-ESP32 3.3.12, arduino-cli 1.6.0, `esp32:esp32:esp32:PartitionScheme=min_spiffs` — as for the OTA loader |
| Result | Compiles without errors at the default warning level |
| Size | 960,851 bytes, 48 % of the 1,966,080-byte app slot; 58,744 bytes of static RAM (17 %) |
| Duration | 284 s for the first build in a new build folder |

The sketch was written for Arduino-ESP32 2.0; it needs no source change for 3.3.12. Size leaves room for the status page.

---

## What stays, what changes

Line numbers refer to [`ESP-Serial-Bridge.ino`](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/ESP-Serial-Bridge.ino) and [`config.h`](https://github.com/yuri-rage/ESP-Serial-Bridge/blob/130f731416d43c89bce5626ef989b3e65918c9ce/config.h).

| Part | Upstream | For the bridge |
| --- | --- | --- |
| Three UARTs, one TCP server each | Lines 57–59, 66–78; pins, baud rates and ports per UART in `config.h` 50–70 | Keep. Set the [UART plan](../../hardware/wiring.md#overview) pins. The Becker RS-422 path is the third UART: no new driver code, the MAX490 is transparent to the firmware |
| Forwarding loop | Lines 283–326: TCP → UART and UART → every TCP client, 1,024-byte buffers | Keep the shape. Radio port: counted writes and one owner (increments 2 and 3 of the first review) |
| Direction | Every port forwards both ways (lines 286–300) | Zander and FLARM receive only: discard client bytes and attach no TX pin |
| Start-up | Station mode waits without limit for the network (lines 152–159); the disconnect handler waits again (lines 106–110) | Replace with the loader's start-up: network for 15 s, then hotspot (R12) |
| OTA | ArduinoOTA without password; callbacks print on `Serial` (lines 174–205) | Replace with the loader's password-protected OTA (R13) |
| Diagnostics | `debug` is `Serial`, the same UART0 object as port 0 (`config.h` line 75, sketch line 59) | Remove. UART0 receives the Zander; the console is switched off and diagnostics go to the status page |
| UDP, Bluetooth | UDP packets are written straight to the UART (lines 219–225); Bluetooth adds another writer (lines 249–257) | Remove both |
| ESP8266 | `#ifdef` branches throughout | Remove |
| UART receive buffer | Arduino default, not enlarged | Enlarge before `begin()`, so a short stall in the loop does not lose navigation or radio bytes |
| Transmit power | Line 235 declares a function instead of calling it; `BATTERY_SAVER` has no effect on the ESP32 | Remove; off by default |
| Status, web page | None; settings are fixed at build time | Add a read-only page and JSON with the core `WebServer` library (R14), in its own task. Phone signal strength from the access-point station list (S55, R10) |

Inference, not measured: a client that stops reading can hold up the single loop while a TCP write waits, which delays all three ports. Step 5 of the plan tests this with a slow client.

---

## Limits

- No runtime evidence: forwarding, Wi-Fi behaviour and timing are untested.
- Blocking times of the Arduino-ESP32 3.3.12 network and UART calls were not traced in the framework source.
- The OTA code path of ArduinoOTA itself was not audited; it is the one already running in the OTA loader (S97–S99).
