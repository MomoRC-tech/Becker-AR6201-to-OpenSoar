<a id="top"></a>

# 📶 Bridge firmware

🏠 [Overview](../../README.md) › 🔬 Research › **Bridge firmware**

Existing ESP32 serial-to-Wi-Fi firmware that could run on the bridge, and what each one would need.

> [!NOTE]
> **Status:** source review from 2026-11-08; nothing built or run. No firmware is selected ([ADR-02](../design/architecture.md#adr-02)), and the choice waits until the hardware is finished.

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
| 1 | [Yuri ESP-Serial-Bridge 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) — MIT | Bounded reads, routing per port | Checked writes, single radio owner, separate diagnostics, pinned build ([review](reviews/2026-11-08-yuri-bridge.md)) |
| 2 | [AlphaLima ESP32-Serial-Bridge](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) — [MIT](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/LICENSE) | Separate TCP/UART paths, counted writes, no NMEA parsing; [config](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/config.h) has 3 UARTs on TCP 8880–8882 | Prints banners on the UARTs, overruns its 1,023-byte buffer, unchecked writes, several writers, Bluetooth broadcast |
| — | [AlphaLima Pro bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/blob/bf263c1880d7630cadef5264dac9aa9654e4096e/ESP32-Serial-Bridge-pro/ESP32_Serial_Bridge_pro.ino) | Web configuration, routes, filters | Small TCP buffer, filters change binary data, no license file — not usable as is |
| — | Firmware in today's bridge | Navigation works (reported) | Unknown source and settings; stays on the backup unit |

---

<a id="alternatives"></a>

## 3. 🔀 Alternatives

| Project | What it offers | To check |
| --- | --- | --- |
| [DroneBridge ESP32](https://github.com/DroneBridge/ESP32/tree/25b3d9c60ca4cb958ad7156a742436c6a4361475) — Apache-2.0 | Transparent bidirectional mode, TCP port 5760, flush timeout, host tests | Whether it handles several independent UARTs; forwarding code not audited |
| [XCVario](https://github.com/iltis42/XCVario/tree/1c5ad83a9e0911b81bb0bd5a47243bea5746f6c2) — GPL-2.0 | ESP32 gliding instrument with serial/Wi-Fi routing (`Router.cpp`) | Good reference for coexistence; adopting the whole firmware is out of scope |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
