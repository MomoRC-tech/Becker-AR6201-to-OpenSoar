# Bridge firmware

Wi-Fi bridge firmware for Zander, FLARM and Becker, built from Yuri's ESP-Serial-Bridge in the steps of the [firmware plan](../../docs/project/roadmap.md#firmware-plan) ([ADR-02](../../docs/design/architecture.md#adr-02)). Current state: step 1, build only.

> [!WARNING]
> Do not upload this image yet. The step 1 start-up has no hotspot fallback and no OTA password: without a network named `ssid` it waits forever, and the board then needs USB again. The first upload comes with step 2.

## Origin and license

| Item | Value |
| --- | --- |
| Upstream | [yuri-rage/ESP-Serial-Bridge](https://github.com/yuri-rage/ESP-Serial-Bridge) |
| Pinned commit | [`130f731416d43c89bce5626ef989b3e65918c9ce`](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) |
| Imported files | `ESP-Serial-Bridge.ino` as [`bridge.ino`](bridge.ino) (arduino-cli needs the sketch named after its folder), [`config.h`](config.h), [`LICENSE`](LICENSE) |
| License | MIT, see [`LICENSE`](LICENSE); keep the notice with every copy |
| Line endings | CRLF as upstream; [`.gitattributes`](.gitattributes) keeps the files byte-identical, so `git diff 56d1b23 -- firmware/bridge` shows every project change |

<a id="project-settings"></a>

## Project settings

Set in `config.h`; everything else is upstream.

| Device | UART | RX | TX | TCP port | UDP port | Baud |
| --- | --- | --- | --- | --- | --- | --- |
| 📈 Zander | UART0 | GPIO32 | GPIO33 | 10110 | 14550 | 19200 |
| 📡 FLARM | UART1 | GPIO26 | GPIO27 | 2000 | 14551 | 19200 |
| 📻 Becker | UART2 | GPIO16 | GPIO17 | 4352 | 14552 | 115200 |

- Pins: [UART plan](../../docs/hardware/wiring.md#overview). TCP ports and the hotspot address 10.10.10.10: [firmware plan](../../docs/project/roadmap.md#firmware-plan).
- Baud rates: Zander and FLARM are candidates from the device documents (S22, S23); the Becker value is the upstream placeholder. The installed settings are [open items](../../docs/project/open-items.md#installed-equipment).
- UDP ports: upstream values; [open item](../../docs/project/open-items.md#software-and-acceptance).

## Build

Same toolchain and partition scheme as the [OTA loader](../ota-loader/README.md#build): Arduino-ESP32 3.3.x, board **ESP32 Dev Module**, Partition Scheme **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**.

```bash
python tools/build_bridge.py
```

The script builds in `build/bridge/` at the repository root, outside the sketch, and prints toolchain, duration, image size and any sketch warnings. A build takes about 4–5 minutes, also when only the sketch changed (S104). Run it in the background.
