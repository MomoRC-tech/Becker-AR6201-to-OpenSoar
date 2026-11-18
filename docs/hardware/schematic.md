<a id="top"></a>

# 🧾 Schematic

🏠 [Overview](../../README.md) › 🔧 Hardware › **Schematic**

All boards of the bridge and their connections on one KiCad sheet, drawn from the pin tables in [signal wiring](wiring.md) and [power and grounding](power.md), which own every connection ([ADR-06](../design/architecture.md#adr-06)).

> [!NOTE]
> **Status:** complete sheet including the [supply voltage sense](power.md#voltage-sense); ERC passes and the netlist matches the pin tables. Not a bench test.

Related: 🔌 [Signal wiring](wiring.md) · ⚡ [Power and grounding](power.md) · 🧱 [Modules and parts](modules.md)

---

<a id="sheet"></a>

## 1. 🗺️ Bridge sheet

[![KiCad schematic: XT30 and UBEC feed 5V-J and GND-J; ESP32 in the middle; two RS-232 modules for Zander and FLARM on the left; the MAX490 RS-422 board with the RX divider and DI pull-up at the bottom; supply voltage divider R-V1, R-V2 and C-V above the ESP32; DE-9 instrument connector on the left](../images/schematic-wifi-bridge.svg)](../images/schematic-wifi-bridge.svg)

Click the image to open it full size. KiCad project: [`electronics/schematic`](../../electronics/schematic).

- Net names follow the pin tables: `12V-IN`, `5V-J`, `GND-J`, `3V3`, `V_SENSE`, `TX2±`, `RX2±`, `ZS1_TX`, `FLARM_TX`.
- Two connections exist only on the boards and are shown with a repeated net label: ESP32 GND pins, and RS-232 module L2 ↔ R3.
- The schematic shows nets, not wire routing. The branch order (5V-J, GND-J, module chain) is in [power and grounding](power.md#supply).

---

<a id="update"></a>

## 2. 🔁 After each change

Edit the pin table first, then the schematic.

- [ ] ERC: zero errors and zero warnings.
- [ ] Netlist compared with [wiring](wiring.md) and [power](power.md), net by net: every pin, nothing extra.
- [ ] SVG re-exported to `docs/images/schematic-wifi-bridge.svg`, trailing spaces stripped (KiCad adds them), and checked for overlapping text.
- [ ] Schematic and SVG committed together.

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
