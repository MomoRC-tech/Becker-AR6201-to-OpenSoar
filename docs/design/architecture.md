<a id="top"></a>

# 🏗️ Architecture

🏠 [Overview](../../README.md) › 🧩 Design › **Architecture**

How the bridge is split up, which hardware is selected, and the decisions behind it.

> [!NOTE]
> **Status:** hardware design selected (ADR-04); firmware not selected. Building and testing are tracked in the [roadmap](../project/roadmap.md).

📑 **On this page:** <kbd>[1 · 🧩 Who does what](#responsibilities)</kbd> <kbd>[2 · 🔧 Hardware](#hardware)</kbd> <kbd>[3 · 📝 Decisions](#decisions)</kbd>

Related: 🔌 [Wiring](../hardware/wiring.md) · 🧱 [Modules and parts](../hardware/modules.md) · 🎯 [Requirements](requirements.md)

---

<a id="responsibilities"></a>

## 1. 🧩 Who does what

| Component | Responsibility |
| --- | --- |
| 📱 OpenSoar | Understands the radio protocol: builds frames, converts channels, tracks radio state and replies |
| 🧠 ESP32 bridge | Forwards bytes unchanged; keeps radio, navigation and diagnostic traffic on separate paths |
| 📡📈 Navigation | FLARM and Zander data keep flowing as today, including any return traffic |
| 📻 Radio interface | ESP32 UART ↔ 5 V MAX490 RS-422 board ([details](../hardware/modules.md#rs422-board)) |

---

<a id="hardware"></a>

## 2. 🔧 Hardware — one ESP32

| Item | Selection |
| --- | --- |
| Board | AZ-Delivery ESP32 WROOM dev module |
| Zander, FLARM | 2 × Hailege H-1-0290 RS-232 modules at 3.3 V |
| Becker | 5 V MAX490 RS-422 board; receiver output to the ESP32 through 10 kΩ |
| UARTs | UART0 (remapped) Zander, UART1 FLARM, UART2 Becker |
| Supply | 12 V battery → 2 A fuse → Hobbywing UBEC at 5 V → ESP32 and MAX490; ESP32 3.3 V → RS-232 modules |
| Firmware | Not selected — software waits for the hardware. Candidates: [bridge firmware](../research/bridge-firmware.md) |
| OpenSoar review baseline | 7.46.26, `b2e745cfddfe0150f6ed7db467ac18c321b38d01` (S09); the installed build is not known yet |

Pin-by-pin details: [wiring](../hardware/wiring.md). Today's installation: [existing installation](../project/existing-installation.md).

---

<a id="decisions"></a>

## 3. 📝 Decisions

| ADR | Decision | State |
| --- | --- | --- |
| [ADR-00](#adr-00) | The repository is the project record | ✅ Selected |
| [ADR-01](#adr-01) | Separate, transparent radio path | 🟡 Proposed |
| [ADR-02](#adr-02) | Firmware and reuse baseline | ⏸️ Open |
| [ADR-03](#adr-03) | New hardware; existing setup kept as backup | ✅ Selected |
| [ADR-04](#adr-04) | Bench hardware and power path | ✅ Selected |

<a id="adr-00"></a>

### 3.1 ADR-00 — repository as project record

One work queue: the [roadmap](../project/roadmap.md). Requirements live in [requirements](requirements.md), missing information in [open items](../project/open-items.md), evidence in [sources](../research/sources.md), design in this page and the hardware pages.

<a id="adr-01"></a>

### 3.2 ADR-01 — separate transparent radio path

**Proposed.** The radio gets its own bidirectional TCP endpoint; all protocol and state handling stays in OpenSoar, and the bridge only forwards bytes. Final selection needs the installed equipment details and the firmware review.

<a id="adr-02"></a>

### 3.3 ADR-02 — firmware and reuse baseline

**Open, on hold until the hardware is finished.** Yuri's ESP serial bridge is the first candidate; it needs exclusive radio ownership, separate diagnostics and checked writes ([review](../research/reviews/2026-11-08-yuri-bridge.md)). DroneBridge is an alternative. Check licenses and build inputs before reusing code. OpenSoar driver fixes are planned separately (WP4).

<a id="adr-03"></a>

### 3.4 ADR-03 — new hardware, existing setup as backup

**Selected.** One new ESP32 with its own interfaces, power and wiring. The existing hardware, firmware and configuration stay untouched as a backup.

<a id="adr-04"></a>

### 3.5 ADR-04 — bench hardware and power path

> [!TIP]
> **Selected:**
> - ESP32, two Hailege RS-232 modules and the Hobbywing UBEC at 5 V.
> - Becker: 5 V MAX490 board; receiver output to GPIO16 through a 10 kΩ series resistor; J1-21 bonded to the board ground; 10 kΩ pull-up on the driver input so the radio line stays quiet during boot.
> - UART plan: UART0 (remapped) Zander, UART1 FLARM, UART2 Becker.
> - Power protection: 2 A fuse plus the UBEC's reverse-input protection.

Parts: [parts list](../hardware/modules.md#parts-list).

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
