<a id="top"></a>

# 📶 Bridge firmware

🏠 [Overview](../../README.md) › 🔬 Research › **Bridge firmware**

References for the selected ESP32 serial bridge and possible Bluetooth operation.

> [!NOTE]
> **Status:** Yuri's ESP-Serial-Bridge is the selected firmware baseline ([ADR-02](../design/architecture.md#adr-02)). The Bluetooth references support the [Wi-Fi-off decision](../project/roadmap.md#bluetooth-decision).

<a href="#firmware-requirements"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-bridge-firmware-1-dark.svg"><img src="../images/nav/menu-research-bridge-firmware-1.svg" alt="1 · Requirements for any firmware" height="30"></picture></a> <a href="#candidates"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-bridge-firmware-2-dark.svg"><img src="../images/nav/menu-research-bridge-firmware-2.svg" alt="2 · Candidates" height="30"></picture></a> <a href="#alternatives"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-bridge-firmware-3-dark.svg"><img src="../images/nav/menu-research-bridge-firmware-3.svg" alt="3 · Alternatives" height="30"></picture></a> <a href="#softrf"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-bridge-firmware-4-dark.svg"><img src="../images/nav/menu-research-bridge-firmware-4.svg" alt="4 · SoftRF Bluetooth" height="30"></picture></a> <!-- page-menu -->

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

The selected baseline is Yuri's ESP-Serial-Bridge at 130f731. The [adoption review](reviews/2026-11-16-yuri-adoption.md) records the build and retained features. The [comparison with AlphaLima](reviews/2026-11-17-bridge-projects-check.md) records the alternatives considered.

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

<a id="softrf"></a>

## 4. 📡 SoftRF Bluetooth

SoftRF provides a reference for sending FLARM sentences to stock OpenSoar over Bluetooth Low Energy (S165). See the [implementation decision gate](../project/roadmap.md#bluetooth-decision).

| SoftRF practice | Application to the bridge |
| --- | --- |
| HM-10 BLE serial service with notifications | Try NimBLE with service `FFE0` and characteristic `FFE1`. Forward the original FLARM sentences to the phone. |
| Device-specific name and advertising after disconnect | Give the bridge a recognizable Bluetooth name independent of the Wi-Fi SSID. Clear queued traffic on disconnect so the next session starts with current data. |
| Bounded transmit queue and paced notifications | Isolate the BLE reader from the Wi-Fi forwarding loop. Count overflow and size the send rate against sustained FLARM input. SoftRF’s pacing falls short of a continuous 19,200-baud 8N1 feed (S165). |
| Phone setup illustrated in the navigation app | Capture the settings and incoming data during the existing [stock OpenSoar phone checks](../../firmware/bridge/README.md#slow-app-check). |

### 4.1 XCVario and shared live data

XCVario combines its own sensor sentences and FLARM GPS and traffic on one Bluetooth stream. XCSoar uses the XCVario driver for the sensor sentences and its common parser for FLARM. The manual uses a dedicated connection for radio control (S216).

For the bridge, one **Zander / SDI** entry is a candidate for Zander and FLARM live data. The bridge could also send its voltage as an LK8EX1 sentence in that stream. Becker tuning needs a separate bidirectional connection to the stock AR62xx driver (S217).

### 4.2 Wi-Fi or BLE at boot

Starting one stack at a time avoids simultaneous runtime heap use. A single image with both modes still has to fit static RAM and the OTA slot. Check with NimBLE and the current bridge baseline (S166, S218).

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
