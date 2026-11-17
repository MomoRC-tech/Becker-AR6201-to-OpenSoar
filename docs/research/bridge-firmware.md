<a id="top"></a>

# 📶 Bridge firmware

🏠 [Overview](../../README.md) › 🔬 Research › **Bridge firmware**

Existing ESP32 serial-to-Wi-Fi firmware that could run on the bridge, and what each one would need.

> [!NOTE]
> **Status:** source reviews from 2026-11-08 and 2026-11-16. Candidate 1 is the selected baseline ([ADR-02](../design/architecture.md#adr-02)); the project's [bridge firmware](../../firmware/bridge/README.md) built from it runs on the bench ESP32 (S104–S107). Progress: [firmware plan](../project/roadmap.md#firmware-plan).

📑 **On this page:** <kbd>[1 · 📐 Requirements for any firmware](#firmware-requirements)</kbd> <kbd>[2 · 🧪 Candidates](#candidates)</kbd> <kbd>[3 · 🔀 Alternatives](#alternatives)</kbd>

Related: 📻 [Becker driver](becker-driver.md) · 📚 [Sources](sources.md)

---

<a id="firmware-requirements"></a>

## 1. 📐 Requirements for any firmware

- Forwards bytes unchanged, with bounded buffers and checked writes.
- Keeps debug output away from the device ports.
- Exactly one client may send to the radio at a time.
- Navigation data keeps flowing while the radio path is busy.

---

<a id="candidates"></a>

## 2. 🧪 Candidates

In order of preference.

| # | Candidate | Good | Needs work |
| :-: | --- | --- | --- |
| 1 | [Yuri ESP-Serial-Bridge 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) — MIT | Bounded reads, routing per port, three UARTs with one TCP port each | Done in the project firmware: hotspot fallback, OTA password, no log output on the device UARTs, larger receive buffers, status page. Still to do: single radio owner and counted writes (plan step 4). Reviews: [first](reviews/2026-11-08-yuri-bridge.md), [adoption](reviews/2026-11-16-yuri-adoption.md) |
| 2 | [AlphaLima ESP32-Serial-Bridge](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) — [MIT](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/LICENSE) | Separate TCP/UART paths, counted writes, no NMEA parsing; [config](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/config.h) has 3 UARTs on TCP 8880–8882 | Prints banners on the UARTs, silently drops bytes beyond 1,023 per pass, unchecked writes, several writers, Bluetooth broadcast |
| — | [AlphaLima Pro bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/blob/bf263c1880d7630cadef5264dac9aa9654e4096e/ESP32-Serial-Bridge-pro/ESP32_Serial_Bridge_pro.ino) | Web configuration, routes, filters | No license file; 127-byte buffers that overwrite the last byte on longer TCP bursts; NMEA filters restart on `$`, so binary data breaks; free heap printed and all TCP → device bytes echoed on UART0, a device port; settings page without login by default — not usable as is ([check](reviews/2026-11-17-bridge-projects-check.md#pro)) |
| — | Firmware in today's bridge | Navigation works (reported) | Unknown source and settings; stays on the backup unit |

---

<a id="alternatives"></a>

## 3. 🔀 Alternatives

| Project | What it offers | To check |
| --- | --- | --- |
| [DroneBridge ESP32](https://github.com/DroneBridge/ESP32/tree/25b3d9c60ca4cb958ad7156a742436c6a4361475) — Apache-2.0 | Transparent bidirectional mode, TCP port 5760, flush timeout, host tests | Whether it handles several independent UARTs; forwarding code not audited |
| [XCVario](https://github.com/iltis42/XCVario/tree/1c5ad83a9e0911b81bb0bd5a47243bea5746f6c2) — GPL-2.0 | ESP32 gliding instrument with serial/Wi-Fi routing (`Router.cpp`) | Good reference for coexistence; adopting the whole firmware is out of scope |
| [GS-Traffic2WIFI 7b1d41d](https://github.com/gs-avionik-gunnar/GS-Traffic2WIFI/tree/7b1d41d2a97f66d70715768199aa15746753cadf) — MIT | One FLARM/NMEA port to TCP, network with hotspot fallback, web page in its own task; drops a client after three short writes | Reference for client handling in step 5 ([check](reviews/2026-11-17-bridge-projects-check.md#other-projects)) |
| [BtBridge 62ebaf6](https://github.com/iltis42/BtBridge/tree/62ebaf63b6522b3dbd273aa22181e6ce32764842) — GPL-2.0 | Two serial ports (FLARM, radios) to Wi-Fi TCP or Bluetooth; only the first client writes to the device | Reference only (GPL) |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
