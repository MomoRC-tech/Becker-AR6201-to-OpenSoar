<a id="top"></a>

# 🏗️ Architecture

🏠 [Overview](../../README.md) › 🧩 Design › **Architecture**

How the bridge is split up, which hardware is selected, and the decisions behind it.

> [!NOTE]
> **Status:** hardware design (ADR-04), update path (ADR-05) and schematic role (ADR-06) selected; bridge firmware baseline selected (ADR-02). Building and testing are tracked in the [roadmap](../project/roadmap.md).

📑 **On this page:** <kbd>[1 · 🧩 Who does what](#responsibilities)</kbd> <kbd>[2 · 🔧 Hardware](#hardware)</kbd> <kbd>[3 · 📝 Decisions](#decisions)</kbd>

Related: 🔌 [Signal wiring](../hardware/wiring.md) · ⚡ [Power and grounding](../hardware/power.md) · 🧱 [Modules and parts](../hardware/modules.md) · 🎯 [Requirements](requirements.md)

---

<a id="responsibilities"></a>

## 1. 🧩 Who does what

| Component | Responsibility |
| --- | --- |
| 📱 OpenSoar | Understands the radio protocol: builds frames, converts channels, tracks radio state and replies |
| 🧠 ESP32 bridge | Forwards bytes unchanged; keeps radio, navigation and diagnostic traffic on separate paths |
| 📡📈 Navigation | FLARM and Zander send data to the bridge; no return lines to either instrument |
| 📻 Radio interface | ESP32 UART ↔ 5 V MAX490 RS-422 board ([details](../hardware/modules.md#rs422-board)) |

---

<a id="hardware"></a>

## 2. 🔧 Hardware — one ESP32

| Item | Selection |
| --- | --- |
| Board | AZ-Delivery ESP32 WROOM dev module |
| Zander, FLARM | 2 × Hailege H-1-0290 RS-232 modules at 3.3 V |
| Becker | 5 V MAX490 RS-422 board; receiver output to the ESP32 through a 2.2 kΩ / 3.3 kΩ divider |
| UARTs | UART0 (remapped) Zander, UART1 FLARM, UART2 Becker |
| Supply | 12 V battery → supply fuse → Hobbywing UBEC at 5 V → ESP32 and MAX490; ESP32 3.3 V → RS-232 modules |
| Firmware | Yuri ESP-Serial-Bridge 130f731 as baseline ([ADR-02](#adr-02)), built in the steps of the [firmware plan](../project/roadmap.md#firmware-plan) |
| OpenSoar review baseline | 7.46.26, `b2e745cfddfe0150f6ed7db467ac18c321b38d01` (S09); the installed build is not known yet |

Pin-by-pin details: [signal wiring](../hardware/wiring.md) and [power and grounding](../hardware/power.md). Today's installation: [existing installation](../project/existing-installation.md).

---

<a id="decisions"></a>

## 3. 📝 Decisions

| ADR | Decision | State |
| --- | --- | --- |
| [ADR-00](#adr-00) | The repository is the project record | ✅ Selected |
| [ADR-01](#adr-01) | Separate, transparent radio path | 🟡 Proposed |
| [ADR-02](#adr-02) | Firmware and reuse baseline | ✅ Selected |
| [ADR-03](#adr-03) | New hardware; existing setup kept as backup | ✅ Selected |
| [ADR-04](#adr-04) | Bench hardware and power path | ✅ Selected |
| [ADR-05](#adr-05) | Update path and Wi-Fi start-up | ✅ Selected |
| [ADR-06](#adr-06) | KiCad schematic as a derived view of the pin tables | ✅ Selected |

<a id="adr-00"></a>

### 3.1 ADR-00 — repository as project record

One work queue: the [roadmap](../project/roadmap.md). Requirements live in [requirements](requirements.md), missing information in [open items](../project/open-items.md), evidence in [sources](../research/sources.md), design in this page and the hardware pages.

<a id="adr-01"></a>

### 3.2 ADR-01 — separate transparent radio path

**Proposed.** The radio gets its own bidirectional TCP endpoint; all protocol and state handling stays in OpenSoar, and the bridge only forwards bytes. Final selection needs the installed equipment details and the firmware review.

<a id="adr-02"></a>

### 3.3 ADR-02 — firmware and reuse baseline

> [!TIP]
> **Selected:**
> - Base the bridge firmware on Yuri's ESP serial bridge 130f731 (MIT) and keep its license notice.
> - Keep all upstream features; removing one needs the owner's answer first.
> - Add the OTA loader's start-up, exclusive radio ownership, counted writes and a read-only status page.

Reviews: [adoption review](../research/reviews/2026-11-16-yuri-adoption.md), [first review](../research/reviews/2026-11-08-yuri-bridge.md). Steps and feature selections: [firmware plan](../project/roadmap.md#firmware-plan). OpenSoar driver fixes are planned separately (WP4).

<a id="adr-03"></a>

### 3.4 ADR-03 — new hardware, existing setup as backup

**Selected.** One new ESP32 with its own interfaces, power and internal wiring. Reuse the existing Becker and Zander harnesses from their [loose bridge-side ends](../project/existing-installation.md#equipment). Preserve the existing bridge electronics, firmware and configuration as a backup.

<a id="adr-04"></a>

### 3.5 ADR-04 — bench hardware and power path

> [!TIP]
> **Selected:**
> - ESP32, two Hailege RS-232 modules and the Hobbywing UBEC at 5 V.
> - Becker: 5 V MAX490 board; receiver output to GPIO16 through a 2.2 kΩ / 3.3 kΩ divider; J1-21 bonded directly (no resistor) to the board ground by a separate wire; bridge supply ground to the radio's supply ground point; cable shield to J1-11 at the radio end only; 10 kΩ pull-up on the driver input so the radio line stays quiet during boot.
> - UART plan: UART0 (remapped) Zander, UART1 FLARM, UART2 Becker.
> - Power protection: supply fuse plus the UBEC's reverse-input protection; [fuse selection](../project/open-items.md#bench-hardware).

Parts: [parts list](../hardware/modules.md#parts-list).

<a id="adr-05"></a>

### 3.6 ADR-05 — update path and Wi-Fi start-up

> [!TIP]
> **Selected:**
> - Flash an OTA loader once over USB on the bench; all later firmware goes over Wi-Fi ([R13](requirements.md#requirements)). Built with Arduino-ESP32, like both firmware candidates, which include ArduinoOTA (S95).
> - Partition scheme **Minimal SPIFFS (1.9MB APP with OTA)**, `min_spiffs.csv`: two app slots of about 1.9 MB each (S94). OTA cannot change the partition table, so every later build uses the same scheme.
> - Wi-Fi start-up ([R12](requirements.md#requirements)): join the configured network; if that fails, open the bridge's own password-protected hotspot. OTA works in both modes.
> - Every firmware image keeps an OTA receiver. The update method of the project firmware is chosen with [ADR-02](#adr-02).

| Item | Rule |
| --- | --- |
| Loader | [`firmware/ota-loader`](../../firmware/ota-loader/README.md), ArduinoOTA (S94). Tries the configured network for 15 s, then opens hotspot `WiFi-BridgeTom` at 192.168.4.1 (S96) |
| Bridge firmware | Same start-up and hotspot name; hotspot address 10.10.10.10, as on today's bridge ([configuration](../project/existing-installation.md#configuration)) |
| Availability | Hotspot and OTA stay available as long as the bridge is powered |
| Board setting | ESP32 Dev Module, partition scheme as above; PlatformIO: `board_build.partitions = min_spiffs.csv` |
| Credentials | Private builds: Wi-Fi and OTA passwords stay in an untracked local file, never in the repository. The committed [hotspot-only build](../../firmware/ota-loader/README.md#variants) uses public default passwords and is for the bench only |
| Recovery | An image without OTA, or one that fails at boot, needs USB: hood and USB cap off ([power rules](../hardware/power.md#supply)) |

Bench step: [bring-up](../hardware/wiring.md#bring-up) step 3.

<a id="adr-06"></a>

### 3.7 ADR-06 — KiCad schematic as a derived view

> [!TIP]
> **Selected:**
> - The pin tables in [signal wiring](../hardware/wiring.md) and [power and grounding](../hardware/power.md) own every connection. The KiCad schematic is a derived view; on a mismatch the tables win and the schematic is fixed.
> - KiCad project in `electronics/schematic/`, next to `cad/`. Wiring only: no footprints, no PCB.
> - The [schematic page](../hardware/schematic.md) shows the exported sheet and points to the two pin-table pages; it holds no pin tables of its own.
> - The images generated by `tools/diagrams.py` stay, including supply and ground.

| Item | Rule |
| --- | --- |
| Layout | One sheet with all boards; every connection drawn as a visible wire |
| Net names | As written in the pin tables (for example `5V-J`, `GND-J`) |
| Module symbols | One project library; pin names as on the delivered boards' silkscreen ([modules](../hardware/modules.md)) |
| Check after each change | ERC with zero errors; export the netlist and compare it with the tables, net by net, using a checklist per sheet. A comparison script only if manual checks become a burden |
| Not committed | KiCad local files: `.kicad_prl`, `*-backups/`, `fp-info-cache`, `*.lck` |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
