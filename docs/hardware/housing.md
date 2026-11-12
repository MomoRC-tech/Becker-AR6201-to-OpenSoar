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
- **F1:** 5 × 20 mm panel holder in the connector wall, fuse replaceable from outside ([power](power.md#protection)).
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
| PCB mounting | Snap-fit clips locate and hold every PCB; ribs inside the hood press the PCBs down when it is closed, so vibration loads go into the hood, not the hooks |
| PCB clips | Printed as separate parts, lying flat so the hooks bend along the layer lines; each clip foot slides into a keyed (dovetail) socket in the floor that positions it and takes the load, a drop of glue only stops it sliding out. A misfitting clip is reprinted without reprinting the housing. Each PCB sits with its underside 2 mm above its tallest underside part; the clip type must not set that height, so no long vertical hooks |
| ESP32 | Component side up; its pre-soldered header pins point up, and wires are soldered directly to them from above (not replaceable). Lengthwise, antenna at the short end opposite the connectors. Held at its short ends only (the header solder joints leave no free long edge): rests on a ledge in the middle of each short end. Antenna end: slides under two fixed corner lips 1.7 mm thick on the bare edge between header strip and module (EN side 1.9 mm wide, BOOT side 1.5 mm, 0.25 mm clear of both), and a bridge between the corner posts presses a pad 0.3 mm onto the module shield as a spring; no plastic around the antenna. USB end: one keeper bar presses a pad 0.3 mm onto the USB socket, held by a tongue in a slot on the BOOT side and one M2.5 metal screw cut straight into a PETG boss on the EN side (no insert); the USB socket is not usable with the bar fitted. The board stays captured with the hood off; leave clearance above the header strips and wires |
| Internal wiring | One snap-fit perfboard carries the RX divider, the pull-ups, the bridge ground point and the 3.3 V / 5 V distribution; wires run from it to the modules |
| Access from outside | Connectors and the F1 fuse only. UBEC jumper and the ESP32 EN/BOOT buttons only with the hood off, Micro-USB only with the hood and the ESP32 keeper bar off; no USB opening in the hood |
| Soldering | Every solder joint reachable with all PCBs fitted and the cover off |
| Connector wall | XT30, DE-9 and the F1 fuse holder together on one short side |
| XT30 | Inserted front first from inside; press fit on the flat faces, with clearance at the chamfered corners; the raised markings stop against the inner wall face, so unplugging cannot pull it out; glued in, so plugging-in cannot push it into the housing |
| DE-9 | Male connector (pins) on the housing, mounted from the front: flange set flush into a recess in the outer wall face, rear body through a rectangular cutout; jackposts (UNC 4-40) through flange and wall into nuts in printed pockets inside, wall thinned locally around the nuts if the thread is too short; the female cable connector locks onto the jackposts |
| Walls | 2.5 mm side walls, floor and cover; 3.0 mm connector wall (DE-9 screws, XT30 press fit); at least 2.0 mm of material around each M3 heat-set insert |
| Edges | Rounded outside edges and corners, no sharp corners |
| Location | Shaded spot in the cockpit, out of direct sun through the canopy |
| Attachment | Four slotted ears at the base corners, outside the hood footprint, for cable ties; the hood comes off while the base stays tied in |
| Ventilation | Narrow slots low and high in the hood side walls; no openings on top |
| Status light | Small window in the hood over the ESP32 power LED |
| Markings | Connector names on the connector wall (12 V, instrument DE-9, fuse); owner adds a multicolour logo on the hood later |
| Size | Space the modules for soldering-iron access and tidy wiring first; review the size once modelled |
| CAD | Modelled in Fusion, millimetres; STEP and print-ready 3MF committed to `cad/housing/`, parametric source kept in the owner's Fusion account |

<a id="material"></a>

### 2.1 Material: PETG

PETG is selected because the housing sits in the shade. Heat resistance compared with ASA, the alternative for the enclosed P1S:

| Bambu filament | HDT at 0.45 MPa | HDT at 1.8 MPa | Source |
| --- | ---: | ---: | --- |
| PETG HF | 69 °C | 62 °C | S47 |
| ASA | 100 °C | 92 °C | S48 |

- **Keep it in the shade:** PETG is fine well below about 60 °C. Sun through the canopy can heat cockpit parts far above the outside air; focused sunlight has burnt instrument panels (S49).
- **Loaded features creep first:** snap-fits and the XT30 press fit. If the housing has to move into the sun, switch to ASA (about 30 °C more margin).

<a id="print-values"></a>

### 2.2 Print starting values

Starting values for PETG on the P1S with a 0.4 mm nozzle and 0.2 mm layers. They are estimates from common FDM practice; the [fit test print](#fit-test) replaces each one with a measured value.

| Feature | Starting value |
| --- | --- |
| Walls | Printed solid: enough perimeters to fill the 2.5 mm wall, no infill inside walls |
| Free fit (part drops in) | 0.2 mm gap per side |
| Snug fit (PCB in its pocket) | 0.15 mm gap per side on the measured PCB outline |
| Slot for a PCB edge | PCB thickness + 0.15 mm (fit test: slides in by hand with minor play; the hood ribs take out the rest) |
| Press fit (XT30) | Pocket = measured body (10.55 × 5.25 mm), no allowance; the print undersize gives the press fit (fit test 2, printed flat: holds ten unplugging pulls, pushes out firmly, no damage) |
| M2.5 screw directly into PETG | Pilot hole Ø2.2 mm (fit test 2), at least 5 mm thread engagement, at least 2 mm of material around the hole |
| DE-9 front mount (printed flat) | Flange recess 31.6 × 13.2 × 1.0 mm (snug, flush); rear-body cutout 19.9 × 11.5 mm; flange holes Ø3.4 mm; nut pocket 5.6 mm across flats for the 5.45 mm nuts |
| M3 screw clearance hole | Ø3.4 mm (fit test: Ø3.3 is the smallest free hole; Ø3.4 leaves alignment margin for the four cover screws) |
| Snap hook | At least 8 mm long, 1.8 mm thick, 1.0 mm lip over the PCB edge, 0.1 mm gap per side to the PCB, 1 mm fillet at the root (fit test: 1.2 and 1.5 mm too weak; a 0.6 mm lip did not hold; recheck on the separate clips) |
| Hood hold-down ribs | 0.2 mm interference on the PCB top, ribs 1.2 mm wide so they flex instead of cracking the PCB |
| Smallest feature | 0.8 mm (two extrusion lines) |
| Overhangs | Up to 60° from vertical without support; bridges up to 10 mm |
| Embossed text | 0.6 mm raised, characters 5 mm tall (confirmed by the fit test) |

- **Snap hooks printed standing up** (as on the fit-test coupon) bend across the layer lines, their weakest direction; the separate clips are printed lying flat instead, so the coupon results are on the safe side.
- **Measured accuracy:** parts print about 0.5 % small in X/Y (0.3 mm over 68 mm), Z true, no elephant foot. No scaling: print the housing with the same filament and settings as the fit test, so the measured fits carry over.
- **Heat-set insert hole** (ruthex RX-M3x5.7, S50): Ø4.0 mm (confirmed by the fit test), at least 6.7 mm deep (insert length + 1 mm), at least 2.0 mm of material around it (ruthex minimum 1.6 mm).

<a id="fit-test"></a>

### 2.3 Fit test print

One small PETG coupon, printed before the housing is modelled: [print files and feature map](../../cad/housing/fit-test-coupon/README.md) (six small pieces, about 14.5 cm³, print without supports). Check each feature and record the best value in [section 2.2](#print-values).

**You need:** calipers; one M3×8 screw and a hex key; three ruthex inserts and a soldering iron with an insert tip; the XT30 male and its female mate; the DE-9 male, two jackposts with nuts and a female cable connector; edges of a 1.6 mm and a 1.2 mm PCB.

**Order:** reversible tests first, the inserts last (they cannot be removed). Let the pieces cool fully and remove any brim first.

- [x] **Print accuracy:** F 19.9 × 19.9 × 1.56 mm, C 67.7 mm (about 0.5 % small in X/Y), no elephant foot.
- [x] **M3 clearance holes (C):** screw Ø2.9 mm; Ø3.2 snug, Ø3.3 and larger free. Selected Ø3.4.
- [x] **PCB edge slots (D):** RS-232 (1.15 mm) in the 1.3 slot and a 1.55 mm test board in the 1.7 slot: both slide in by hand with minor play. The ESP32's header solder joints reach to within about 0.1 mm of its long edges, so it cannot be held in long-edge slots. MAX490 board not delivered yet.
- [x] **Snap hooks (E + F):** 1.2 and 1.5 mm hold too weakly; 1.8 mm has the right force, but the 0.6 mm lip barely holds (the 19.9 mm plate leaves only about 0.4 mm overlap in the 20.3 mm gap). The 8.5 mm support height needed for the hook to flex wastes space under the PCB; next: clip trial print.
- [ ] **XT30 pockets (B):** failed, the connector does not go into any pocket. The pockets were sized on the CAD model (10.25 mm), the delivered connector is 10.55 mm wide with a different end shape and no rib. Redesign on the measured connector and reprint.
- [ ] **DE-9 cutout (A):** failed: the delivered connector is male (shell 17.85 × 9.2 mm), the cutout was sized on the female NorComp shell; it is now mounted from the front. Retest on the reprinted piece: rear body through the cutout, flange flat on the outer face.
- [ ] **Jackposts (A):** on the reprinted piece: which hole the 4-40 thread passes, and whether the nut gets a full grip through flange plus 3.0 mm wall.
- [x] **Embossed text (A):** "12V", 5 mm high, raised 0.6 mm on a vertical face; reads clearly.
- [x] **Insert holes (C):** Ø4.0 mm works best.

**Fit test 2:** [print files and piece map](../../cad/housing/fit-test-2/README.md) (fifteen small pieces, about 13 cm³, print without supports).

- [x] **XT30 (G):** the 0.00 pocket (= measured body) fits; holds ten pulls, pushes out firmly, no damage. Glued in against plugging-in force.
- [x] **DE-9 flange and cutout (H):** recess snug and flush, cutout passes.
- [x] **DE-9 holes (H):** Ø3.4 fits; nut pockets too big for the 5.45 mm nuts, reduce to 5.6 mm.
- [ ] **Clip sockets (J, K):** each clip drops into its socket by hand, sits without wobble and cannot be pulled straight up.
- [ ] **Clips on plate F (J):** try fixed lip C1 + arm C2, C1 + C3, and the pair C3 + C4. Pick the combination that snaps in by thumb, holds without play and comes out again without breaking.
- [ ] **Clips on an RS-232 board (K):** same with C5 + C6 and C5 + C7, across the board's 9.2 mm width.
- [x] **M2.5 pilot holes (M):** Ø2.2 mm fits best.
- [x] **ESP32 holder (L, T1, T2):** board flat, no rattle, buttons and USB free. Changes: lips 0.5 mm wider and thicker, a spring bridge over the module shield, one keeper bar with one screw instead of two tabs.

**Fit test 3:** [ESP32 holder v2](../../cad/housing/fit-test-3/README.md) (two pieces, about 3.3 cm³, print without supports).

- [ ] **ESP32 holder v2 (L2, K):** EN side at the wide lip. Slide the antenna end under the lips and the bridge, lower the USB end, hook the keeper bar into its slot and screw it down. The board sits flat without rattle, the bridge pad presses the shield lightly, the lips do not break, the bridge has not sagged onto the module.

The [mechanical fit coupons](../../cad/housing/mechanical-tests/README.md) give three independent prints: an ESP32 cradle with keeper tabs that seat on the bosses, an RS-232 saddle with a removable keeper, and a combined DE-9/XT30 front plate.

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
- Press-fit pocket; front shoulder and a hood rib behind capture it ([design rules](#rules)); adhesive optional.
- Support the wires inside so mating forces don't load the solder joints.
- Before the final pocket: measure the real connector, print a fit-test piece.

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

Not designed yet. Constraints from the wiring:

- **Short wires:** RO → divider → GPIO16 and the pull-ups sit between the MAX490 board and the ESP32.
- **Grounds:** one bridge ground; route the device signal grounds from the DE-9 to the module grounds ([grounding and loops](power.md#ground)).
- **Power path:** XT30 → F1 → UBEC → ESP32 and MAX490 ([supply path](power.md#supply)).
- **Ferrite ring:** keep its placeholder volume ([section 3](#contents)) at the UBEC's output end (opposite the jumper), on the 5 V lead towards the ESP32; the ring is not modelled in the UBEC CAD reference.
- **ESP32:** lengthwise, antenna at the far short end; Micro-USB faces the connector end.
- **Resistors and ground point:** on one snap-fit perfboard ([design rules](#rules)).

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
