<a id="top"></a>

# 📦 Housing

🏠 [Overview](../../README.md) › 🔧 Hardware › **Housing**

Mechanical design of the bridge: what goes inside, part sizes, connectors on the housing, internal layout.

> [!NOTE]
> **Status:** not designed yet. Selected so far: XT30 and DE-9 on the housing, F1 inside the housing. Everything else: [open items](../project/open-items.md#housing).

📑 **On this page:** <kbd>[1 · 🎯 Requirements](#requirements)</kbd> <kbd>[2 · 📏 Contents and sizes](#contents)</kbd> <kbd>[3 · 🔌 Connectors on the housing](#connectors)</kbd> <kbd>[4 · 🧩 Internal layout](#layout)</kbd>

Related: 🧱 [Modules and parts](modules.md) · 🔌 [Signal wiring](wiring.md) · ⚡ [Power and grounding](power.md)

---

<a id="requirements"></a>

## 1. 🎯 Requirements

- **Holds:** ESP32 board, 2 × RS-232 modules, MAX490 board, UBEC (with jumper, leads, ferrite ring), F1 fuse holder, RX divider and pull-up resistors.
- **On the housing:** XT30 for power, DE-9 for the three instruments ([section 3](#connectors)).
- **F1:** inside the housing, replaceable ([power](power.md#protection)).
- **Wiring:** connector mating forces must not load solder joints.
- **UBEC jumper:** must stay at 5 V — a 6 V setting is a hazard ([modules](modules.md#ubec)).
- **USB:** never together with the UBEC supply; radio unplugged when on USB ([power](power.md#supply)).
- **Wi-Fi:** the ESP32 antenna end needs a non-conductive path to the phone. Metal or carbon around it would block the signal (inference; Becker warns the same for its VHF antenna, S10).

---

<a id="contents"></a>

## 2. 📏 Contents and sizes

Sizes from owner measurements, seller listings or datasheets; see the source column. Remaining measurements: [open items](../project/open-items.md#housing).

| Part | Qty | Size (mm) | Source | Notes |
| --- | :-: | --- | --- | --- |
| ESP32 board (AZ-Delivery, DevKitC V4 layout) | 1 | 48.5 × 28.5 base PCB, excluding antenna | S40 | Antenna end free; Micro-USB at the other end |
| RS-232 module (Hailege H-1-0290) | 2 | 9.2 × 15.6 | S38 | Edge pads, soldered wires |
| RS-422 board (MAX490) | 1 | 50 × 27 | S34 | Screw terminals A/B/Y/Z on one edge, header on the other |
| UBEC (Hobbywing 3A) | 1 | 43 × 17 × 7 (catalog); body about 35 × 12 × 5 | S14 | Plus jumper, leads, ferrite ring |
| F1 fuse holder | 1 | Open | — | Format not selected yet |
| XT30 male, panel | 1 | To measure | — | Printed pocket ([section 3](#connectors)) |
| DE-9 female socket | 1 | Standard D-sub shell | — | Exact part open |
| Resistors (divider, pull-ups) | 5 | Small | — | R-B1, R-B2, R-Z, R-F, R-D |

### 2.1 ESP32 CAD reference

Use the [STEP assembly](../../cad/reference/esp32-devkitc-v4/ESP-WROOM-32D-Export/ESP32-WROOM-32D-Assembly.STEP) for preliminary layout. The [original download files and attribution](../../cad/reference/esp32-devkitc-v4/README.md) include SolidWorks files, 3MF and a preview (S42).

| Dimension | CAD model (mm) | Delivered board (mm, S40) | Model minus measured (mm) |
| --- | ---: | ---: | ---: |
| Base PCB length | 48.26 | 48.5 | −0.24 |
| Base PCB width | 28.10 | 28.5 | −0.40 |
| PCB thickness | 1.60 | — | — |
| Overall length, antenna and USB included | 54.86 | — | — |
| Overall height, headers included | 14.23 | — | — |

- Keep the delivered-board measurements for mounting design; this simplified DevKitC V4/WROOM-32D model is not an exact AZ-Delivery mechanical drawing.
- Model axes: X = width, Y = height, Z = length. The PCB underside is Y = 0; pins extend to Y = −8.89 mm and the top reaches Y = 5.34 mm.
- Do not uniformly scale the assembly to match the PCB: that changes the header pitch and component sizes too. Allow separately for the USB plug and wiring; physical fit checks belong to the [housing measurements](../project/open-items.md#housing).

### 2.2 RS-232 CAD reference

Use the [STEP model](../../cad/reference/rs232-uart-ttl-mini-max3232/RS232_TTL/RS232_TTL.STEP) for preliminary layout. The [original download files and attribution](../../cad/reference/rs232-uart-ttl-mini-max3232/README.md) include a Parasolid alternative (S41).

| Dimension | CAD model (mm) | Delivered board (mm, S38) | Model minus measured (mm) |
| --- | ---: | ---: | ---: |
| PCB width | 9.6 | 9.2 | +0.4 |
| PCB length | 15.8 | 15.6 | +0.2 |
| PCB thickness | 1.2 | — | — |
| Overall component height, both sides | 2.752 | — | — |

Keep the delivered-board measurements for mounting design. The model represents a similar module, not an exact Hailege mechanical drawing; its height excludes soldered wires. Do not uniformly scale it to match the PCB, since that also changes component sizes. Physical fit checks belong to the [housing measurements](../project/open-items.md#housing).

### 2.3 UBEC and resistor CAD references

- [Hobbywing 3A UBEC SolidWorks reference](../../cad/reference/ubec-hobbywing-3a/README.md) (S44). Use the catalog dimensions above for preliminary layout.
- [KiCad DIN0204 axial STEP models](../../cad/reference/kicad-din0204/README.md) (S45) give a 3.6 × Ø1.6 mm body with two options for horizontal leads. Use as placeholders for resistors of that body size.

---

<a id="connectors"></a>

## 3. 🔌 Connectors on the housing

**XT30 (power)** — electrical side: [power](power.md#supply)

- Male on the housing, female on the powered harness.
- Snug printed pocket with retaining shoulder, plus adhesive.
- Support the wires inside so mating forces don't load the solder joints.
- Before the final pocket: measure the real connector, print a fit-test piece.

**DE-9 (instruments)** — pin use: [signal wiring](wiring.md#overview)

- Preliminary layout: [NorComp 171-009-203 STEP](../../cad/reference/norcomp-171-009-203/171-009-203.STEP) (S43). Model envelope: 30.80 × 12.60 × 15.27 mm (width × height × depth). Allow separate space for mounting hardware, mating plug and wiring.

- Female socket on the housing, male cable plug with solder cups and screw locking.
- Backshell and strain relief on the cable side.
- Housing side: screw-lock threads and panel mounting still open.

**F1 (fuse)** — inside the housing; holder, format and replacement access open.

---

<a id="layout"></a>

## 4. 🧩 Internal layout

Not designed yet. Constraints from the wiring:

- **Short wires:** RO → divider → GPIO16 and the pull-ups sit between the MAX490 board and the ESP32.
- **Grounds:** one bridge ground; route the device signal grounds from the DE-9 to the module grounds ([grounding and loops](power.md#ground)).
- **Power path:** XT30 → F1 → UBEC → ESP32 and MAX490 ([supply path](power.md#supply)).
- **Access:** UBEC jumper and Micro-USB reachable or not — see [open items](../project/open-items.md#housing).

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
