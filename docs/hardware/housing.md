<a id="top"></a>

# 📦 Housing

🏠 [Overview](../../README.md) › 🔧 Hardware › **Housing**

Mechanical design of the bridge: what goes inside, part sizes, connectors on the housing, internal layout.

> [!NOTE]
> **Status:** internal layout selected in Fusion ([section 5](#layout)); housing body not modelled yet. Design rules, material and wall thickness selected ([section 2](#rules)); remaining inputs: [open items](../project/open-items.md#housing).

📑 **On this page:** <kbd>[1 · 🎯 Requirements](#requirements)</kbd> <kbd>[2 · 📐 Design rules](#rules)</kbd> <kbd>[3 · 📏 Contents and sizes](#contents)</kbd> <kbd>[4 · 🔌 Connectors on the housing](#connectors)</kbd> <kbd>[5 · 🧩 Internal layout](#layout)</kbd>

Related: 🧱 [Modules and parts](modules.md) · 🔌 [Signal wiring](wiring.md) · ⚡ [Power and grounding](power.md)

---

<a id="requirements"></a>

## 1. 🎯 Requirements

- **Holds:** ESP32 board, 2 × RS-232 modules, MAX490 board, UBEC (with jumper, leads, ferrite ring), F1 fuse holder. The five resistors sit on the module pads and ESP32 pins, so there is no extra board.
- **On the housing:** XT30 for power, DE-9 for the three instruments ([section 4](#connectors)).
- **F1:** 5 × 20 mm panel holder in the connector wall, fuse replaceable from outside ([power](power.md#protection)). Added last, after the rest of the housing is modelled.
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
| Cover | Hood held by four M3 × 8 stainless A2 cylinder-head screws (ISO 4762), one per corner, heads in counterbores, into ruthex RX-M3x5.7 brass heat-set inserts in corner bosses of the base (S50) |
| PCB mounting | Each PCB type gets its own printed holder, shaped to the board and fixed to the housing floor, so a misfitting holder is reprinted without reprinting the housing. Each PCB sits with its underside at least 2 mm above its tallest underside part. The UBEC sits on a lift-off bridge over the MAX490 ([layout](#layout)). Holders for the MAX490 board and the UBEC: [open items](../project/open-items.md#housing) |
| ESP32 | Component side up; its pre-soldered header pins point up, and wires are soldered directly to them from above (not replaceable). Lengthwise, antenna at the short end opposite the connectors. Held at its short ends by two blocks glued onto the housing floor on broad feet, with the board underside 4.5 mm above the floor to clear the header solder tails, positioned by a printed gluing jig (a dummy board) while the glue cures. Antenna cap: one piece, printed standing on its antenna-facing side; side walls along both long edges, corner end walls, lips 1.7 mm thick over the bare edge between header strip and module (EN side 1.9 mm, BOOT side 1.5 mm wide), a chamfered ledge under the board, and a spring bridge whose pad reaches 2 mm onto the module shield with 0.3 mm pre-load; the antenna overhang passes through an open window, with the bridge about 3 mm above it. Above the header strips the bridge stays about 1.9 mm clear of the first header pins on each side (estimated from the CAD model), room for the soldered wires. USB end: a screw block with a ledge, two end stops and one boss directly in front of the USB socket; a USB cap covers the top and sides of the socket, presses 0.2 mm onto it and is held by one M2.5 metal screw cut straight into the boss (no insert); the USB socket is not usable with the cap fitted. Rounded outer edges. The board stays captured with the hood off; leave clearance above the header strips and wires |
| RS-232 modules | Each slides flat into a slot of two grooved rails, held by friction on its long edges beside the chip feet; slot 1.25 mm for the 1.15 mm PCB, both pad rows free for soldering. Both slots are part of the ESP32 USB block ([layout](#layout)) |
| Internal wiring | Wires are soldered directly to the module pads and ESP32 header pins (no plugs, nothing to shake loose); the resistors sit on those pads and pins ([signal wiring](wiring.md)), the ESP32 ground copper is the bridge ground ([supply path](power.md#supply)) |
| Access from outside | Connectors and the F1 fuse only. UBEC jumper and the ESP32 EN/BOOT buttons only with the hood off, Micro-USB only with the hood and the ESP32 USB cap off; no USB opening in the hood |
| Soldering | Every solder joint reachable with all PCBs fitted and the cover off, except the MAX490 header joints under the UBEC bridge: reachable with the bridge lifted |
| Connector wall | XT30, DE-9 and the F1 fuse holder together on one short side |
| XT30 | Inserted front first from inside; press fit on the flat faces, with clearance at the chamfered corners; the raised markings stop against the inner wall face, so unplugging cannot pull it out; glued in, so plugging-in cannot push it into the housing |
| DE-9 | Male connector (pins) on the housing, mounted from the front: flange set flush into a recess in the outer wall face, rear body through a rectangular cutout; jackposts (UNC 4-40) through flange and wall into nuts in printed pockets inside, wall thinned locally around the nuts if the thread is too short; the female cable connector locks onto the jackposts |
| Walls | 2.0 mm side walls; 2.5 mm floor and cover; 3.0 mm connector wall (DE-9 screws, XT30 press fit); at least 2.0 mm of material around each M3 heat-set insert |
| Edges | Rounded outside edges and corners, no sharp corners |
| Location | Shaded spot in the cockpit, out of direct sun through the canopy |
| Attachment | Four slotted ears at the base corners, outside the hood footprint, for cable ties; the hood comes off while the base stays tied in |
| Ventilation | Narrow slots low and high in the hood side walls; no openings on top |
| Status light | Small window in the hood over the ESP32 power LED |
| Markings | Connector names on the connector wall (12 V, instrument DE-9, fuse); owner adds a multicolour logo on the hood later |
| Shape | Preferred direction, not a fixed rule: narrow and long rather than square or wide, but not too narrow; a better layout may differ. Height set once the layout is modelled |
| Size | Space the modules for soldering-iron access and tidy wiring first; review the size once modelled |
| CAD | Modelled in Fusion, millimetres. Each model's folder in `cad/housing/` holds the editable Fusion archive (`.f3d`, full timeline), a STEP and a print-ready 3MF; the owner's Fusion project keeps the same documents. After a change, export all three again and commit them together |

<a id="material"></a>

### 2.1 Material: PETG

PETG is selected because the housing sits in the shade. Heat resistance compared with ASA, the alternative for the enclosed P1S:

| Bambu filament | HDT at 0.45 MPa | HDT at 1.8 MPa | Source |
| --- | ---: | ---: | --- |
| PETG HF | 69 °C | 62 °C | S47 |
| ASA | 100 °C | 92 °C | S48 |

- **Keep it in the shade:** PETG is fine well below about 60 °C. Sun through the canopy can heat cockpit parts far above the outside air; focused sunlight has burnt instrument panels (S49).
- **Loaded features creep first:** the RS-232 friction slots, the ESP32 bridge spring and the XT30 press fit. If the housing has to move into the sun, switch to ASA (about 30 °C more margin).

<a id="print-values"></a>

### 2.2 Print starting values

Values for PETG on the P1S with a 0.4 mm nozzle and 0.2 mm layers. Values marked with a fit test are measured ([section 2.3](#fit-test)); the others are estimates from common FDM practice.

| Feature | Starting value |
| --- | --- |
| Walls | Printed solid: enough perimeters to fill each wall, no infill inside walls |
| Free fit (part drops in) | 0.2 mm gap per side |
| Snug fit (PCB in its pocket) | 0.15 mm gap per side on the measured PCB outline |
| Slot for a PCB edge | PCB thickness + 0.10 mm for a friction hold (RS-232 holder: 1.25 mm slot for the 1.15 mm PCB); + 0.15 mm slides in with minor play |
| Press fit (XT30) | Pocket = measured body (10.55 × 5.25 mm), no allowance; the print undersize gives the press fit (fit test 2, printed flat: holds ten unplugging pulls, pushes out firmly, no damage) |
| M2.5 screw directly into PETG | Pilot hole Ø2.2 mm (fit test 2), at least 5 mm thread engagement, at least 2 mm of material around the hole |
| DE-9 front mount (printed flat) | Flange recess 31.6 × 13.2 × 1.0 mm (snug, flush); rear-body cutout 19.9 × 11.5 mm; flange holes Ø3.4 mm; nut pocket 5.6 mm across flats for the 5.45 mm nuts |
| M3 screw clearance hole | Ø3.4 mm (fit test: Ø3.3 is the smallest free hole; Ø3.4 leaves alignment margin for the four cover screws) |
| Smallest feature | 0.8 mm (two extrusion lines) |
| Overhangs | Up to 60° from vertical without support; bridges up to 10 mm |
| Embossed text | 0.6 mm raised, characters 5 mm tall (confirmed by the fit test) |

- **Measured accuracy:** parts print about 0.5 % small in X/Y (0.3 mm over 68 mm), Z true, no elephant foot. No scaling: print the housing with the same filament and settings as the fit test, so the measured fits carry over.
- **Heat-set insert hole** (ruthex RX-M3x5.7, S50): Ø4.0 mm (confirmed by the fit test), at least 6.7 mm deep (insert length + 1 mm), at least 2.0 mm of material around it (ruthex minimum 1.6 mm).

<a id="fit-test"></a>

### 2.3 Fit tests and current models

The fit-test prints are done; their results are the values in [section 2.2](#print-values). Old test models are in Git history.

| Tested | Result |
| --- | --- |
| Print accuracy | About 0.5 % small in X/Y, Z true, no elephant foot |
| M3 clearance hole | Ø3.3 smallest free, Ø3.4 selected |
| M2.5 straight into PETG | Pilot hole Ø2.2 mm |
| Heat-set insert hole | Ø4.0 mm |
| PCB edge slots | PCB thickness + 0.15 mm slides with minor play; not usable on the ESP32 (solder joints at the long edges) |
| Snap hooks (standing) | 1.8 mm thick needed, 0.6 mm lip too small; replaced by dedicated holders |
| Embossed text | 5 mm characters, 0.6 mm raised, readable |
| XT30 (printed flat) | Pocket = measured body 10.55 × 5.25 mm, chamfer clearance; holds unplugging, glued |
| DE-9 front mount (printed flat) | Recess 31.6 × 13.2 × 1.0 mm snug and flush, cutout 19.9 × 11.5 mm, holes Ø3.4 mm, nut pockets 5.6 mm |
| ESP32 holder | Works after two changes: board underside 4.5 mm above the floor (header solder tails), bridge 1 mm narrower on each side at the first header pins |
| RS-232 holder | Works with the 1.25 mm slot (PCB 1.15 mm): friction hold, slides in by hand |

Current models in `cad/housing/` (files: [CAD rule](#rules)):

| Model | Status |
| --- | --- |
| [ESP32 holder](../../cad/housing/esp32-holder/README.md): antenna cap, USB block, USB cap, gluing jig | Tested, selected |
| [RS-232 holder](../../cad/housing/rs232-holder/README.md): grooved two-rail slot holder, 1.25 mm slot | Tested, selected; two needed |
| [Connector panel](../../cad/housing/connector-panel/README.md): DE-9, XT30, space for F1 | Updated to the measured values; prints flat, front face down |

- [ ] **Connector panel:** XT30 front first from the back until the markings stop it, DE-9 from the front into the flush recess, nuts in the pockets; mate both plugs and check backshell and tool clearance.

### 2.4 Later optimizations

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
| UBEC (Hobbywing 3A) | 1 | 43 × 17 × 7 (catalog); body about 35 × 12 × 5 | S14 | Plus jumper and leads |
| UBEC ferrite ring | 1 | Reserve 30 × 30 × 15 keep-out (estimate from the product photo, ring with wire turns) | — | On the UBEC 5 V output lead, as shipped; resize once measured |
| F1 fuse holder | 1 | Open | — | Format not selected yet |
| XT30 male, panel | 1 | 10.55 × 5.25 body; 10.8 × 5.66 over the raised markings at the solder-cup end (owner measurement) | S51 | Press-fit pocket ([section 4](#connectors)) |
| DE-9 male, panel | 1 | Flange 31 × 12.6 × 1.0, holes Ø3.1 at 25.0 spacing; D shell 17.85 × 9.2 outer; rear body behind the flange 19.3 × 10.9 × 5.0 deep, 8.0 including the solder cups (owner measurement) | S43 | Flange and rear side as the female NorComp model |
| Resistors (divider, pull-ups) | 5 | Small | — | R-B1, R-B2, R-Z, R-F, R-D |

### 3.1 ESP32 CAD reference

Use the [STEP assembly](../../cad/reference/esp32-devkitc-v4/ESP-WROOM-32D-Export/ESP32-WROOM-32D-Assembly.STEP) for preliminary layout. The [original download files and attribution](../../cad/reference/esp32-devkitc-v4/README.md) include SolidWorks files, 3MF and a preview (S42).

| Dimension | CAD model (mm) | Delivered board (mm, S40) | Model minus measured (mm) |
| --- | ---: | ---: | ---: |
| Base PCB length | 48.26 | 48.5 | −0.24 |
| Base PCB width | 28.10 | 28.5 | −0.40 |
| PCB thickness | 1.60 | 1.50 (owner measurement) | +0.10 |
| Overall length, antenna and USB included | 54.86 | — | — |
| Overall height, headers included | 14.23 | — | — |
| A, B: bare edge at the antenna end, header strip to module | 2.50 each | A 2.4, B 2.0 (owner measurement) | +0.10, +0.50 |
| C, D: bare edge at the USB end, header strip to USB socket | 7.50 each, with the EN or BOOT button in it | 7.5 each | 0 |
| Bare strip between USB socket and button, each side | 2.95 wide | — | — |
| E: EN and BOOT button, near side from the USB-end edge | 0.77 | 1.5 both | −0.73 |
| F: USB socket beyond the PCB edge | 0.61 | 0.5 | +0.11 |
| Module (antenna) beyond the PCB edge | 6.01 | — | — |

- Keep the delivered-board measurements for mounting design; this simplified DevKitC V4/WROOM-32D model is not an exact AZ-Delivery mechanical drawing.
- Measurement letters A–F: top view, USB end on the left, antenna end on the right; header strips 2.54 mm wide along both long edges, full length.
- Model axes: X = width, Y = height, Z = length. The PCB underside is Y = 0; pins extend to Y = −8.89 mm and the top reaches Y = 5.34 mm.
- Do not uniformly scale the assembly to match the PCB: that changes the header pitch and component sizes too. Allow separately for the USB plug and wiring; physical fit checks belong to the [housing measurements](../project/open-items.md#housing).

### 3.2 RS-232 CAD reference

Use the [STEP model](../../cad/reference/rs232-uart-ttl-mini-max3232/RS232_TTL/RS232_TTL.STEP) for preliminary layout. The [original download files and attribution](../../cad/reference/rs232-uart-ttl-mini-max3232/README.md) include a Parasolid alternative (S41).

| Dimension | CAD model (mm) | Delivered board (mm, S38) | Model minus measured (mm) |
| --- | ---: | ---: | ---: |
| PCB width | 9.6 | 9.2 | +0.4 |
| PCB length | 15.8 | 15.6 | +0.2 |
| PCB thickness | 1.2 | 1.15 (owner measurement) | +0.05 |
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

- Measurement letters A–F: top view, USB end on the left, antenna end on the right; header strips 2.54 mm wide along both long edges, full length.
- Model axes: X = width, Y = length (terminal edge Y = 0, header edge Y = 50), Z = up; PCB underside Z = 0, pins below reach Z = −2.5.
- Measure the mounting holes, PCB thickness and terminal height before sizing the mount ([open items](../project/open-items.md#housing)).

### 3.5 XT30 CAD reference

Use the [STEP model](../../cad/reference/xt30u-m/xt30u-m.step) of the AMASS XT30U-M (male, solder cups) for the pocket design; [attribution and license](../../cad/reference/xt30u-m/README.md) (S51).

| Dimension | CAD model (mm) | Delivered connector (mm, owner measurement) |
| --- | ---: | ---: |
| Body cross-section | 10.25 × 5.25 | 10.55 × 5.25 |
| Rib around the body (9.0–11.0 from the front) | 10.45 wide | No rib; raised "XT30" markings from 8.5 mm behind the front face to the solder-cup end, 10.8 × 5.66 over them |
| Shaped end | 0.5 mm radii at one end, chamfered corners at the other | Two chamfers, about 1.3 × 1.6 (along the short × long side) |
| Body length | 12.5 | — |
| Solder cups beyond the body | 3.0 | — |
| Overall length with cups | 15.5 | — |
| Solder cup diameter / centre spacing | 2.0 / 5.0 | — |

- **The delivered connector differs from the model:** design the pocket on the measured values.
- Model axes: Z = length, solder cups at +Z; the two corners at the Y = 0 end have 0.5 mm radii, the other end is chamfered.
- Measure the real connector against this table before the final pocket; allow separately for wires and heat-shrink on the cups.

---

<a id="connectors"></a>

## 4. 🔌 Connectors on the housing

**XT30 (power)** — electrical side: [power](power.md#supply)

- Male on the housing, female on the powered harness.
- Press-fit pocket, markings stop it against the inner wall face, glued in ([design rules](#rules)); pocket size: [print values](#print-values).
- Support the wires inside so mating forces don't load the solder joints.

**DE-9 (instruments)** — pin use: [signal wiring](wiring.md#overview)

- Reference for the flange and rear side: [NorComp 171-009-203 STEP](../../cad/reference/norcomp-171-009-203/171-009-203.STEP) (S43), the female variant; the delivered male has the same flange (31 × 12.6 mm) and rear side, its D shell measures 17.85 × 9.2 mm.
- Male connector (pins) on the housing, female cable connector with solder cups and screw locking.
- Front mount: flange in a flush recess on the outer face, rear body (19.3 × 10.9 mm) through a rectangular cutout; the cable connector seats fully because no wall is in front of the flange.
- Backshell and strain relief on the cable side.
- Standard 4-40 jackposts (stud length about 4.8 mm, estimate; not delivered yet): through 1.0 mm flange and 2.0 mm wall, the nut pocket is sunk into the inner face so the nut gets full thread.
- Housing side: jackposts ([design rules](#rules)); exact jackpost and local wall thickness: [open items](../project/open-items.md#bench-hardware).

**F1 (fuse)** — 5 × 20 mm fuse in a panel-mount holder on the connector wall, replaceable from outside. Exact holder and rating: [open items](../project/open-items.md#bench-hardware).

---

<a id="layout"></a>

## 5. 🧩 Internal layout

> [!TIP]
> **Selected layout:** Fusion document `Housing layout - compact v4` in the Segelflug-WifiBridge folder, with the holder `esp32 holder v3 (RS-232 clamp)`; repository copies in [cad/housing/layout-v4](../../cad/housing/layout-v4/README.md). Inside about 87 × 60 × 20 mm (L × W × H, from the walls' inner faces, with 2.0 mm side walls inside the 64 mm plate width; the 20 mm height is an estimate for the ESP32 header wires). The document `Housing layout` keeps the parts in their first arrangement as the untouched master.

| Column | Parts, from the connector wall |
| --- | --- |
| ESP32 side (−Z) | XT30, 4.5 mm higher than the tested panel (body from 11.9 above the floor) · ESP32 (USB edge X = 27, centre Z = −10.5, antenna end X = 81) with both RS-232 boards on its USB clamp; the clamp starts 3.8 mm behind the XT30 body |
| MAX490 side (+Z) | DE-9 · MAX490 on the floor (terminal edge X = 17, PCB underside 4.5) with the UBEC on a bridge 10 mm above it from X = 28, jumper toward the wall · ferrite ring on edge behind the UBEC output end (X 83–89) |

- **ESP32 holder:** tested USB block B (orange in Fusion), antenna cap A without foot ears (31.9 wide, blue), and a USB clamp UR carrying both RS-232 boards (green). The boards sit turned 90°, one behind the other across the ESP32 between its header rows, 13.8 above the floor (rails to 16.1). One M2.5 screw removes the clamp with both boards.
- **Clamp arm:** a 6 × 2 mm arm runs from the clamp's free end over the ESP32 module (2.1 mm clear) into a U-channel on top of the antenna cap's spring bridge. It drops in when the clamp is lowered and only locates the free end (0.2 mm clearance, no load on the spring bridge).
- **Assembly order:** solder the RS-232 wires on the bench, slide the boards into the clamp, fit the ESP32, then screw the clamp on.
- **UBEC bridge:** covers the MAX490 header joints and must lift off for soldering there; the UBEC jumper stays reachable from above with the hood off.
- **Wiring:** XT30 wires run over the USB clamp to the UBEC input.
- **Length driver:** the ferrite ring; the ESP32 column ends 8 mm before the ring's far end.
- **Width:** with 2.0 mm side walls the antenna cap sits 1.0 mm and the MAX490 board 0.5 mm from the inner side-wall faces; the ESP32 column cannot move much further away from the MAX490 column without widening the housing.
- **Hood shape:** rounded top edges, a cut-off rear corner on the MAX490 side and a lower hood over the ring area. A curved hood saves little: the tallest items (ESP32 header wire rows, UBEC) sit close to both side walls.
- **Fit tests needed:** the clamp prints top up, so its slot height comes from the 0.2 mm layers, its top needs bridging and the arm needs support; arm fit in the antenna-cap channel.
- **Not modelled yet:** wire routing, MAX490 posts, UBEC bridge, ring wire turns. The UBEC model (24 × 43 × 8) is larger than the catalog size (17 × 43 × 7), so the real unit may free a few millimetres.

Components in the Fusion layout:

| Component | Source |
| --- | --- |
| Connector side | Plate (`plate_w` 64, `plate_h` 25, `wall_t` 3 mm) with the tested DE-9 and XT30 features (XT30 raised), no F1 yet; XT30 and DE-9 reference models |
| ESP32 and RS-232 holder | Antenna cap, USB block and USB clamp with RS-232 slots derived (linked) from the Fusion `esp32 holder v3 (RS-232 clamp)` document, with the ESP32 and RS-232 reference boards. ESP32 header strips turned so the pins point up as on the real board |
| MAX490 board | Linked Fusion document `MAX490 board - CHATGPT` |
| UBEC (reference) | Repository STEP, body raised 1.5 mm for the jumper pins |
| Ferrite ring (estimate) | Ø15 / Ø8 × 6 from the product photo (parameters `ferrite_od`, `ferrite_id`, `ferrite_h`) |

- **Layout frame:** X along the housing length from the outside face of the connector wall; Y up from the floor top (2.5 mm floor below); Z across.
- **Holder pieces:** each derived body is moved from its print position into its installed position by a *Place installed* move; a change in the holder document updates the layout.
- **Interference check:** no collisions between components; only intended holder contacts and the UBEC output lead entering the ring.

Constraints from the wiring:

- **Short wires:** keep the MAX490 board close to the ESP32's J3 header (RO, DI and GND wires); leave a tie-down point next to the ESP32 for the heat-shrunk R-B1 and R-D.
- **Grounds:** one bridge ground; route the device signal grounds from the DE-9 to the module grounds ([grounding and loops](power.md#ground)).
- **Power path:** XT30 → F1 → UBEC → ESP32 and MAX490 ([supply path](power.md#supply)).
- **Ferrite ring:** keep its placeholder volume ([section 3](#contents)) at the UBEC's output end (opposite the jumper), on the 5 V lead towards the ESP32; the ring is not modelled in the UBEC CAD reference.
- **ESP32:** lengthwise, antenna at the far short end; Micro-USB faces the connector end.

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
