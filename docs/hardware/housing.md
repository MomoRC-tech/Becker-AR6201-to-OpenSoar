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
| ESP32 board (AZ-Delivery, DevKitC V4 layout) | 1 | 48.26 × 27.94 base PCB; 54.30 long including antenna overhang (reference) | S39 | Espressif reference layout; USB protrusion additional; antenna end free |
| RS-232 module (Hailege H-1-0290) | 2 | 9.2 × 15.6 | S38 | Edge pads, soldered wires |
| RS-422 board (MAX490) | 1 | 50 × 27 | S34 | Screw terminals A/B/Y/Z on one edge, header on the other |
| UBEC (Hobbywing 3A) | 1 | 43 × 17 × 7 (catalog); body about 35 × 12 × 5 | S14 | Plus jumper, leads, ferrite ring |
| F1 fuse holder | 1 | Open | — | Format not selected yet |
| XT30 male, panel | 1 | To measure | — | Printed pocket ([section 3](#connectors)) |
| DE-9 female socket | 1 | Standard D-sub shell | — | Exact part open |
| Resistors (divider, pull-ups) | 5 | Small | — | R-B1, R-B2, R-Z, R-F, R-D |

---

<a id="connectors"></a>

## 3. 🔌 Connectors on the housing

**XT30 (power)** — electrical side: [power](power.md#supply)

- Male on the housing, female on the powered harness.
- Snug printed pocket with retaining shoulder, plus adhesive.
- Support the wires inside so mating forces don't load the solder joints.
- Before the final pocket: measure the real connector, print a fit-test piece.

**DE-9 (instruments)** — pin use: [signal wiring](wiring.md#overview)

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
