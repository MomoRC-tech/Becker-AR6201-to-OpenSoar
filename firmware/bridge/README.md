# Bridge firmware

Wi-Fi bridge firmware for Zander, FLARM and Becker, built from Yuri's ESP-Serial-Bridge in the steps of the [firmware plan](../../docs/project/roadmap.md#firmware-plan) ([ADR-02](../../docs/design/architecture.md#adr-02)). Current state: step 3 done, running on the bench ESP32 (S105–S107).

## Origin and license

| Item | Value |
| --- | --- |
| Upstream | [yuri-rage/ESP-Serial-Bridge](https://github.com/yuri-rage/ESP-Serial-Bridge) |
| Pinned commit | [`130f731416d43c89bce5626ef989b3e65918c9ce`](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) |
| Imported files | `ESP-Serial-Bridge.ino` as [`bridge.ino`](bridge.ino) (arduino-cli needs the sketch named after its folder), [`config.h`](config.h), [`LICENSE`](LICENSE) |
| License | MIT, see [`LICENSE`](LICENSE); keep the notice with every copy |
| Line endings | CRLF as upstream; [`.gitattributes`](.gitattributes) keeps the files byte-identical, so `git diff 56d1b23 -- firmware/bridge` shows every project change. Each change is marked `// Project:` |

<a id="project-settings"></a>

## Project settings

Set in `config.h`; everything else is upstream.

| Device | UART | RX | TX | TCP port | UDP port | Baud |
| --- | --- | --- | --- | --- | --- | --- |
| 📈 Zander | UART0 | GPIO32 | GPIO33 | 10110 | 14550 | 19200 |
| 📡 FLARM | UART1 | GPIO26 | GPIO27 | 2000 | 14551 | 19200 |
| 📻 Becker | UART2 | GPIO16 | GPIO17 | 4352 | — | 115200 |

- Pins: [UART plan](../../docs/hardware/wiring.md#overview). TCP ports and the hotspot address: [firmware plan](../../docs/project/roadmap.md#firmware-plan).
- Baud rates: Zander and FLARM are candidates from the device documents (S22, S23); the Becker value is the upstream placeholder. The installed settings are [open items](../../docs/project/open-items.md#installed-equipment).
- UDP ports: upstream values; [open item](../../docs/project/open-items.md#software-and-acceptance). A UDP port of 0 switches UDP off for that UART; the radio port uses it.
- UDP output is a Wi-Fi broadcast without acknowledgement: single datagrams can be lost (S105). Use TCP where every byte counts.

| Setting | Value |
| --- | --- |
| Wi-Fi start-up | `MODE_STA_AP`: join the network in `credentials.h` for 15 s, otherwise open hotspot `WiFi-BridgeTom` at 10.10.10.10. The upstream modes `MODE_STA` and `MODE_AP` stay selectable |
| Hostname | `wifi-bridgetom`, for mDNS and OTA, as the OTA loader |
| OTA | ArduinoOTA with the password from `credentials.h`, in both Wi-Fi modes |
| Log output | `DEBUG` off: UART0 serves the Zander, so nothing is printed |
| UART receive buffer | 4096 bytes per UART, so bytes are kept while the loop waits on a UART write |
| Build-time checks | The build stops if UDP or Bluetooth is pointed at the radio port, or if not exactly one Wi-Fi mode is selected |
| `BATTERY_SAVER` | Fixed so it lowers the transmit power; off |
| Status page | `STATUS_PAGE`: read-only page on port 80 and `/api/status` as JSON, open to everyone on the bridge's Wi-Fi; see below |

<a id="status-page"></a>

## Status page

Open `http://<bridge address>/` (10.10.10.10 in hotspot mode) in a browser; it updates every 2 s. [`status_page.h`](status_page.h) holds the page; the forwarding loop only adds to counters, and a low-priority task on the other CPU core serves the page.

| Field | Meaning |
| --- | --- |
| Firmware, uptime | Version string and seconds since start |
| Wi-Fi | `network` with the bridge's signal to the router, or `hotspot` with the signal of each connected phone (no device addresses) |
| From device / to device | Bytes UART → network and network → UART (TCP and UDP) since start |
| Last data | Time since the last byte from the device |
| Clients | Connected TCP clients |
| UART errors | Receive buffer full, FIFO overflow, framing, parity or break |
| Short writes | TCP writes that took fewer bytes than offered |

## Build

Same toolchain and partition scheme as the [OTA loader](../ota-loader/README.md#build): Arduino-ESP32 3.3.x, board **ESP32 Dev Module**, Partition Scheme **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**.

Copy [`credentials.example.h`](credentials.example.h) to `credentials.h` and fill in the network and two passwords of at least 8 characters each; the OTA loader's `credentials.h` has the same format. `credentials.h` is ignored by Git; never commit it. The built image contains the passwords and stays in `build/bridge/`.

```bash
python tools/build_bridge.py
```

The script builds in `build/bridge/` at the repository root, outside the sketch, and prints toolchain, duration, image size and any sketch warnings. A build takes between 1 and 5 minutes (S104, S105).

To test the hotspot fallback without switching off the network, build `python tools/build_bridge.py fallback-test` (in `build/bridge-fallback-test/`) and upload it. It joins a network that does not exist, so the hotspot opens after 15 s. Without an OTA upload it boots the previous image after 10 minutes, so it never stays installed by accident. The variant is selected by a generated `bridge_variant.h`, ignored by Git and removed after the build.

## Upload over Wi-Fi

The board must run an image with an OTA receiver (OTA loader or this firmware). Arduino IDE: network port `wifi-bridgetom`. Command line, with `espota.py` from the esp32 board package:

```bash
python espota.py -i <bridge address> -a "<OTA password>" -f build/bridge/bridge.ino.bin
```

There is no serial log. Find the address through mDNS (`wifi-bridgetom.local`) or the router; in hotspot mode it is 10.10.10.10.

## Loop check (step 2)

Fit jumper wires on the bare board (positions in [modules](../../docs/hardware/modules.md#esp32-pinout)): J2-8 IO33 → J2-10 IO26, J2-11 IO27 → J2-7 IO32, J3-11 IO17 → J3-12 IO16. Then, from a PC on the same network:

```bash
python tools/bridge_loop_test.py <bridge address> --radio-loop
```

It sends every byte value and a pseudo-random block over TCP and UDP and checks that the looped port returns exactly the same bytes, and that UDP to the radio port reaches nothing. Use the jumpers only while these pins are not soldered to the modules ([warning](../../docs/project/roadmap.md#firmware-plan)).
