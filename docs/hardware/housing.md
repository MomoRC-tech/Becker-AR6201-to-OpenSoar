<a id="top"></a>

# 📦 Housing

🏠 [Overview](../../README.md) › 🔧 Hardware › **Housing**

Mechanical design of the bridge: what goes inside, part sizes, connectors on the housing, internal layout.

> [!NOTE]
> **Status:** not modelled yet. Design rules, material and wall thickness selected ([section 2](#rules)); remaining inputs: [open items](../project/open-items.md#housing).

📑 **On this page:** <kbd>[1 · 🎯 Requirements](#requirements)</kbd> <kbd>[2 · 📐 Design rules](#rules)</kbd> <kbd>[3 · 📏 Contents and sizes](#contents)</kbd> <kbd>[4 · 🔌 Connectors on the housing](#connectors)</kbd> <kbd>[5 · 🧩 Internal layout](#layout)</kbd>

Related: 🧱 [Modules and parts](modules.md) · 🔌 [Signal wiring](wiring.md) · ⚡ [Power and grounding](power.md)

---

<a id="requirements"></a>

## 1. 🎯 Requirements

- **Holds:** ESP32 board, 2 × RS-232 modules, MAX490 board, UBEC (with jumper, leads, ferrite ring), F1 fuse holder, RX divider and pull-up resistors.
- **On the housing:** XT30 for power, DE-9 for the three instruments ([section 4](#connectors)).
- **F1:** holder built into the housing wall, fuse replaceable from outside ([power](power.md#protection)).
- **Wiring:** connector mating forces must not load solder joints.
- **UBEC jumper:** must stay at 5 V — a 6 V setting is a hazard ([modules](modules.md#ubec)).
- **USB:** never together with the UBEC supply; radio unplugged when on USB ([power](power.md#supply)).
- **Wi-Fi:** the ESP32 antenna end needs a non-conductive path to the phone. Metal or carbon around it would block the signal (inference; Becker warns the same for its VHF antenna, S10).

---

<a id="rules"></a>

## 2. 📐 Design rules

> [!TIP]
> Selected rules for the first housing version.

| Topic | Rule |
| --- | --- |
| Manufacturing | 3D printed on a Bambu Lab P1S in PETG ([section 2.1](#material)) |
| Split | Shallow base tray (floor, low side walls, full-height connector wall) and a deep hood as the cover; connectors stay on the base when the hood is off |
| Cover | Hood held by M3 screws in heat-set inserts in the base |
| PCB mounting | Snap-fits locate and hold every PCB; ribs inside the hood press the PCBs down when it is closed, so vibration loads go into the hood, not the hooks |
| ESP32 | Component side up; its pre-soldered header pins point up, and wires are soldered directly to them from above (not replaceable). Lengthwise, antenna at the short end opposite the connectors |
| Internal wiring | One snap-fit perfboard carries the RX divider, the pull-ups, the bridge ground point and the 3.3 V / 5 V distribution; wires run from it to the modules |
| Access from outside | Connectors and the F1 fuse only. Micro-USB and the UBEC jumper are reachable only with the hood off; no USB opening in the hood |
| Soldering | Every solder joint reachable with all PCBs fitted and the cover off |
| Connectors | XT30 and DE-9 together on one short side |
| XT30 | Press fit positions it; a shoulder in front and a hood rib behind capture it, so unplugging cannot pull it out; glue optional |
| DE-9 | Rigid mount, screwed to the housing |
| Walls | 2.5 mm side walls, floor and cover; 3.5 mm connector wall (DE-9 screws, XT30 press fit); at least 2.0 mm of material around each M3 heat-set insert |
| Edges | Rounded outside edges and corners, no sharp corners |
| Location | Shaded spot in the cockpit, out of direct sun through the canopy |
| Attachment | Cable-tie lugs on the housing to tie the closed housing to the aircraft |
| Heat | Ventilation openings |

<a id="material"></a>

### 2.1 Material: PETG

PETG is selected because the housing sits in the shade. Heat resistance compared with ASA, the alternative for the enclosed P1S:

| Bambu filament | HDT at 0.45 MPa | HDT at 1.8 MPa | Source |
| --- | ---: | ---: | --- |
| PETG HF | 69 °C | 62 °C | S47 |
| ASA | 100 °C | 92 °C | S48 |

- **Keep it in the shade:** PETG is fine well below about 60 °C. Sun through the canopy can heat cockpit parts far above the outside air; focused sunlight has burnt instrument panels (S49).
- **Loaded features creep first:** snap-fits and the XT30 press fit. If the housing has to move into the sun, switch to ASA (about 30 °C more margin).

### 2.2 Later optimizations

- **DE-9 cable tie-down:** a lug below the DE-9 that sticks out past the mated plug, so the cable can be cable-tied to it. The cable then cannot lever the connector out of the housing.

---

<a id="contents"></a>

## 3. 📏 Contents and sizes

Sizes from owner measurements, seller listings or datasheets; see the source column. Remaining measurements: [open items](../project/open-items.md#housing).

| Part | Qty | Size (mm) | Source | Notes |
| --- | :-: | --- | --- | --- |
| ESP32 board (AZ-Delivery, DevKitC V4 layout) | 1 | 48.5 × 28.5 base PCB, excluding antenna | S40 | Antenna end free; Micro-USB at the other end |
| RS-232 module (Hailege H-1-0290) | 2 | 9.2 × 15.6 | S38 | Edge pads, soldered wires |
| RS-422 board (MAX490) | 1 | 50 × 27 | S34 | Screw terminals A/B/Y/Z on one edge, header on the other |
| UBEC (Hobbywing 3A) | 1 | 43 × 17 × 7 (catalog); body about 35 × 12 × 5 | S14 | Plus jumper, leads, ferrite ring |
| F1 fuse holder | 1 | Open | — | Format not selected yet |
| XT30 male, panel | 1 | To measure | — | Press-fit pocket ([section 4](#connectors)) |
| DE-9 female socket | 1 | Standard D-sub shell | — | Exact part open |
| Resistors (divider, pull-ups) | 5 | Small | — | R-B1, R-B2, R-Z, R-F, R-D |

### 3.1 ESP32 CAD reference

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

### 3.2 RS-232 CAD reference

Use the [STEP model](../../cad/reference/rs232-uart-ttl-mini-max3232/RS232_TTL/RS232_TTL.STEP) for preliminary layout. The [original download files and attribution](../../cad/reference/rs232-uart-ttl-mini-max3232/README.md) include a Parasolid alternative (S41).

| Dimension | CAD model (mm) | Delivered board (mm, S38) | Model minus measured (mm) |
| --- | ---: | ---: | ---: |
| PCB width | 9.6 | 9.2 | +0.4 |
| PCB length | 15.8 | 15.6 | +0.2 |
| PCB thickness | 1.2 | — | — |
| Overall component height, both sides | 2.752 | — | — |

Keep the delivered-board measurements for mounting design. The model represents a similar module, not an exact Hailege mechanical drawing; its height excludes soldered wires. Do not uniformly scale it to match the PCB, since that also changes component sizes. Physical fit checks belong to the [housing measurements](../project/open-items.md#housing).

### 3.3 UBEC and resistor CAD references

- [Hobbywing UBEC 3A MAX 5A CAD reference](../../cad/reference/ubec-hobbywing-3a/README.md) (S44). The full model envelope is 54.40 × 24.00 × 9.50 mm, including the modeled jumper and wire ends. Use it for preliminary layout and measure the actual unit before sizing its mount.
- [KiCad DIN0204 axial STEP models](../../cad/reference/kicad-din0204/README.md) (S45) give a 3.6 × Ø1.6 mm body with two options for horizontal leads. Use as placeholders for resistors of that body size.

### 3.4 RS-422 board CAD reference

Use the [STEP model](../../cad/reference/rs422-to-ttl-max490/rs422-to-ttl-max490.step) for preliminary layout. It is a simplified project model based on reference photos ([details](../../cad/reference/rs422-to-ttl-max490/README.md), S46).

| Dimension | CAD model (mm) | Basis |
| --- | ---: | --- |
| PCB | 50.0 × 27.0 | Seller size (S34) |
| PCB thickness | 1.6 | Assumed standard thickness |
| Mounting holes | 4 × Ø3.2, spacing 21.6 × 34.8 | Photo scale |
| Hole centres from edges | 2.8 and 2.6 from the long sides, 2.5 from the header edge, 12.7 from the terminal edge | Photo scale |
| Screw terminal | 20.8 × 8.3, housing top 10.0 above the PCB | Photo scale, 5.08 mm pitch |
| Header pins past the PCB edge | 6.9 | Photo scale, right-angle header |
| Overall envelope, pins included | 27.0 × 56.9 × 14.1 | — |

- Model axes: X = width, Y = length (terminal edge Y = 0, header edge Y = 50), Z = up; PCB underside Z = 0, pins below reach Z = −2.5.
- Measure the mounting holes, PCB thickness and terminal height before sizing the mount ([open items](../project/open-items.md#housing)).

---

<a id="connectors"></a>

## 4. 🔌 Connectors on the housing

**XT30 (power)** — electrical side: [power](power.md#supply)

- Male on the housing, female on the powered harness.
- Press-fit pocket; front shoulder and a hood rib behind capture it ([design rules](#rules)); adhesive optional.
- Support the wires inside so mating forces don't load the solder joints.
- Before the final pocket: measure the real connector, print a fit-test piece.

**DE-9 (instruments)** — pin use: [signal wiring](wiring.md#overview)

- Preliminary layout: [NorComp 171-009-203 STEP](../../cad/reference/norcomp-171-009-203/171-009-203.STEP) (S43). Model envelope: 30.80 × 12.60 × 15.27 mm (width × height × depth). Allow separate space for mounting hardware, mating plug and wiring.

- Female socket on the housing, male cable plug with solder cups and screw locking.
- Backshell and strain relief on the cable side.
- Housing side: rigid, screwed panel mount; screw-lock threads: [open items](../project/open-items.md#bench-hardware).

**F1 (fuse)** — panel-mount holder in the housing wall, fuse replaceable from outside; holder, format and position: [open items](../project/open-items.md#bench-hardware).

---

<a id="layout"></a>

## 5. 🧩 Internal layout

Not designed yet. Constraints from the wiring:

- **Short wires:** RO → divider → GPIO16 and the pull-ups sit between the MAX490 board and the ESP32.
- **Grounds:** one bridge ground; route the device signal grounds from the DE-9 to the module grounds ([grounding and loops](power.md#ground)).
- **Power path:** XT30 → F1 → UBEC → ESP32 and MAX490 ([supply path](power.md#supply)).
- **ESP32:** lengthwise, antenna at the far short end; Micro-USB faces the connector end.
- **Resistors and ground point:** on one snap-fit perfboard ([design rules](#rules)).

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
