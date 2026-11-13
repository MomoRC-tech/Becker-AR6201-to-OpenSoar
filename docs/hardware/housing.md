<a id="top"></a>

# 📦 Housing

🏠 [Overview](../../README.md) › 🔧 Hardware › **Housing**

Mechanical design of the bridge: what goes inside, part sizes, connectors on the housing, internal layout.

> [!NOTE]
> **Status:** internal layout selected in Fusion ([section 5](#layout)); first housing-body prototype modelled, not printed ([section 2.3](#fit-test)). Carrier and base exports include flush seats, offset axes for the 12 mm screws and a 0.4 mm closed floor beneath the pilots ([section 2.4](#ubec-carrier)). Remaining fit checks: [open items](../project/open-items.md#housing).

📑 **On this page:** <kbd>[1 · 🎯 Requirements](#requirements)</kbd> <kbd>[2 · 📐 Design rules](#rules)</kbd> <kbd>[3 · 📏 Contents and sizes](#contents)</kbd> <kbd>[4 · 🔌 Connectors on the housing](#connectors)</kbd> <kbd>[5 · 🧩 Internal layout](#layout)</kbd>

Related: 🧱 [Modules and parts](modules.md) · 🔌 [Signal wiring](wiring.md) · ⚡ [Power and grounding](power.md)

---

<a id="requirements"></a>

## 1. 🎯 Requirements

- **Holds:** ESP32 board, 2 × RS-232 modules, MAX490 board, UBEC (with jumper, leads, ferrite ring). The five resistors sit on the module pads and ESP32 pins, so there is no extra board.
- **On the housing:** XT30 for power, DE-9 for the three instruments ([section 4](#connectors)).
- **F1:** in the 12 V harness at its supply end, not in the housing ([power](power.md#protection)).
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
| Split | Base tray (floor, side walls 10 mm above the floor top, full-height connector wall) and a hood as the cover; connectors stay on the base when the hood is off. Seam: step lip, the inner half of the tray wall rises 1.75 mm and the hood's outer half drops over it (about 0.9 + 0.2 gap + 0.9 mm), so the hood is located before the screws go in. The hood seats on the post tops, on the wall shoulder under its skirt and on the flat top of the connector wall and its two full-height corners; every other base face that would touch it is set 0.25 mm back |
| Cover | Hood held by four M3 × 6 socket head cap screws (DIN 912 / ISO 4762, head Ø5.4 × 2.9), one per corner, heads flush in Ø6.0 × 3.1 mm counterbores, into ruthex RX-M3x5.7 brass heat-set inserts (S50); each screw reaches 4.5 mm into its insert. The hood top stays 2.5 mm thick except for four 4.6 mm thick spots that hang 2.1 mm down into the housing over the posts, leaving 1.5 mm under each head. Inserts at the top of four corner posts on the base (about 8 × 8 mm inside the wall corners, tops 17.9 mm above the floor), so each screw only passes through the hood. The posts hang from the walls: their free faces end 8 mm above the floor and the underside rises 45° from the walls, so they print without support; free vertical edges R2. At the connector-end corners the side and connector walls stay full height around the post top and the thick spot sits between them with 0.2 mm gap. At the XT30 corner the post sits on the XT30 collar, screw 5 mm from the side face; there the side wall top has a 1.0 mm wide inner rebate along the thick spot (0.9 mm below the seat) and the thick spot reaches down into it (0.2 mm side, 0.25 mm bottom gap), so the counterbore wall stays closed |
| PCB mounting | Separate printed holders carry the ESP32 and RS-232 boards; the MAX490 sits on posts built into the base floor. Each PCB sits with its underside at least 2 mm above its tallest underside part. The UBEC sits on a removable carrier sharing the MAX490's four mounting screws ([section 2.4](#ubec-carrier)); the carrier can be reprinted separately |
| ESP32 | Component side up; its pre-soldered header pins point up, and wires are soldered directly to them from above (not replaceable). Lengthwise, antenna at the short end opposite the connectors. Held at its short ends by two blocks glued onto the housing floor on broad feet, with the board underside 4.5 mm above the floor to clear the header solder tails, positioned by a printed gluing jig (a dummy board) while the glue cures. Antenna cap: one piece, printed standing on its antenna-facing side; side walls along both long edges, corner end walls, lips 1.7 mm thick over the bare edge between header strip and module (EN side 1.9 mm, BOOT side 1.5 mm wide), a chamfered ledge under the board, and a spring bridge whose pad reaches 2 mm onto the module shield with 0.3 mm pre-load; the antenna overhang passes through an open window, with the bridge about 3 mm above it. Above the header strips the bridge stays about 1.9 mm clear of the first header pins on each side (estimated from the CAD model), room for the soldered wires. USB end: a screw block with a ledge, two end stops and one boss directly in front of the USB socket; a USB cap covers the top and sides of the socket, presses 0.2 mm onto it and is held by one M2.5 metal screw cut straight into the boss (no insert); the USB socket is not usable with the cap fitted. Rounded outer edges. The board stays captured with the hood off; leave clearance above the header strips and wires |
| RS-232 modules | Each slides flat into a slot of two grooved rails, held by friction on its long edges beside the chip feet; slot 1.25 mm for the 1.15 mm PCB, both pad rows free for soldering. Both slots are part of the ESP32 USB block ([layout](#layout)) |
| Internal wiring | Wires are soldered directly to the module pads and ESP32 header pins (no plugs, nothing to shake loose); the resistors sit on those pads and pins ([signal wiring](wiring.md)), the ESP32 ground copper is the bridge ground ([supply path](power.md#supply)) |
| Access from outside | Connectors only. UBEC jumper and the ESP32 EN/BOOT buttons only with the hood off, Micro-USB only with the hood and the ESP32 USB cap off; no USB opening in the hood |
| Soldering | Every solder joint reachable with all PCBs fitted and the cover off, except the MAX490 header joints under the UBEC bridge: reachable with the bridge lifted |
| Connector wall | XT30 and DE-9 together on one short side. The wall ends at the inside top like the corner posts; the hood top runs over it to the outer face, and the step lip continues up the wall's two vertical end edges where the hood side walls meet it |
| XT30 | Inserted front first from inside; press fit on the flat faces; the pocket follows the two chamfered corners with 0.2 mm clearance; the raised markings stop against the inner wall face, so unplugging cannot pull it out; glued in, so plugging-in cannot push it into the housing |
| DE-9 | Male connector (pins) on the housing, mounted from the front: flange set flush into a recess in the outer wall face, rear body through a D-shaped cutout; jackposts (UNC 4-40) through flange and wall into nuts in printed pockets inside, wall thinned locally around the nuts if the thread is too short; the female cable connector locks onto the jackposts |
| Walls | 2.0 mm side walls; 2.5 mm floor and cover; 3.0 mm connector wall (DE-9 screws, XT30 press fit); at least 2.0 mm of material around each M3 heat-set insert |
| Edges | Rounded outside edges and corners, no sharp corners: vertical corners R6 (inner R4, keeping the 2 mm walls), hood top edges R3 |
| Location | Shaded spot in the cockpit, out of direct sun through the canopy |
| Attachment | Four tie loops blended into the base's long side walls, below the hood seam, for 4.8 mm cable ties; the hood comes off while the base stays tied in. Each loop is a rounded pad 12 mm long and 4 mm out, from the floor underside to 8.5 mm above the floor top, all edges R1.5 (convex outside, concave into the wall). A vertical tunnel 5.4 × 1.9 mm (tie width and thickness + 0.6 mm) runs through it flat against the wall face, open at the top and at the floor underside, corners R0.5 and rounded entries; 2.1 mm of material outside the tie. The tie goes down through the loop and around the structure below. Centres 14 mm from each short end's outer face, clear of the connectors |
| Ventilation | Two rows of horizontal slots in both hood side walls, none on top: low just above the seam lip, high just under the top-edge rounding. Slots 1.5 mm high, about 10 mm long (bridge limit), corners R0.5, 2 mm ribs, clear of the corner posts; five slots per row, about 300 mm² open area in total |
| Status light | No window in the hood; the ESP32 power LED shows through the vent slots |
| Markings | Text size: 60 % of the first sizes; use the same scale for new text. Engraved 0.6 mm into the connector wall's outer face in a heavy font (Arial Black): "7-20V" in 2.16 mm characters above the XT30, "INSTR" in 2.4 mm characters in the strip below the DE-9 flange (not fit-tested yet). Hood top, as part of the dark grey two-colour insert: "7-20V" (Arial Black, 3 mm) above the XT30 and "WIFI Bridge" (Arial bold, 3 mm) along the far short edge; MomoRC logo (Ø56 mm) printed in dark grey, flush with the hood top as a 0.6 mm two-colour insert, features at least 0.8 mm |
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
| DE-9 front mount (printed flat) | Flange recess 31.6 × 13.2 × 1.0 mm (snug, flush); rear-body cutout 19.9 × 11.5 mm (rectangle on the fit-test panel; D-shaped in the housing body); flange holes Ø3.4 mm; nut pocket 5.6 mm across flats for the 5.45 mm nuts |
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
| [Connector wall print test](../../cad/housing/connector-print-test/README.md): XT30 and DE-9 slices of the housing body v1 base | Not printed; prints standing like the base |
| [Housing body v1](../../cad/housing/housing-body-v1/README.md): base tray and hood on layout v4 | Prototype, not printed; includes MAX490 floor posts and the measured UBEC and ring |
| [UBEC carrier](../../cad/housing/ubec-carrier/README.md): shared four-screw mounting and two cable ties | Modelled, not printed; [dimensions and assembly](#ubec-carrier) |

- [ ] **Connector panel:** XT30 front first from the back until the markings stop it, DE-9 from the front into the flush recess, nuts in the pockets; mate both plugs and check backshell and tool clearance.
- [ ] **Connector wall print test:** print both slices and work through their [checklist](../../cad/housing/connector-print-test/README.md) before the full base.
- [ ] **Housing body v1:** print base and hood; check the hood over the seam lip (0.2 gap), the four inserts and the flush DIN 912 screws in the hood thick spots, the DE-9 and XT30 openings printed standing (19.9 and 10.55 mm bridges), fitting the DE-9 nuts beside the corner post and the XT30 collar, the engraved "7-20V" and "INSTR" labels, the hood "7-20V" and "WIFI Bridge" inserts, the two-colour logo on the bed face and the fit of the holders and boards.

<a id="ubec-carrier"></a>

### 2.4 UBEC carrier and MAX490 mounting

> [!TIP]
> Four raised-head M2.5 × 12 screws hold both the carrier and MAX490 board; two small cable ties retain the UBEC. [CAD files and verification](../../cad/housing/ubec-carrier/README.md).

Exported geometry, in the installed layout frame; owner measurements and Fusion checks: S53.

| Feature | Dimension / construction |
| --- | --- |
| UBEC height | PCB underside 10.7 mm above the floor; cover top about 18.0, leaving 2.0 to the hood. A 1.0 mm tie over the top leaves about 1.0 to the hood |
| Carrier plate | 1.5 mm thick, underside at 9.2; only a 19 mm length supports the UBEC, X 43.65–62.65 |
| Solder clearance | No carrier under the PCB for 6.25 mm from the jumper end and 8.75 mm from the output end; connecting arms run outside its outline. The attached leads pass through these open ends |
| PCB holes / post centres | X 29.7 / 64.5, Z 10.8 / 32.4; 34.8 × 21.6 mm pattern. PCB holes Ø3.2, confirmed by owner |
| Screw axes | X 29.7 / 64.5, Z 10.6 / 32.6; carrier holes and floor pilots offset 0.2 mm outward from the PCB-hole centres. An M2.5 shaft has 0.15 mm nominal radial clearance at the nearest PCB-hole edge |
| Carrier screw seats | Flush with the carrier top at 10.7 mm above the floor; legs bear on the MAX490 at 6.1, giving 4.6 mm seat thickness. Ø2.8 clearance holes; no raised pocket lips or screw reference solids |
| Shared screws | 4 × M2.5 × 12 raised-head metal screws, directly into PETG; owner-measured head Ø4.4, height allowance 2.5. Nominal rear-head gaps to the UBEC: 0.2 mm on the ESP32 side, 0.4 mm on the wall side. Engagement 5.8 mm below the MAX490; tips stop 0.8 mm above the pilot-hole bottoms |
| Floor posts | Ø6.6 × 4.5 high, Ø2.2 pilot holes 6.6 deep, leaving 0.4 mm of floor beneath. The offset pilots retain at least 2.0 mm of post wall. The two outer posts join the side wall through short solid webs; the two inner posts have R1 fillets at the floor |
| Cable ties | 2 × 2.5 mm-wide ties, about 1.0 mm thick; centres X 47.5 and 58.5. Four 3.2 × 1.6 slots, R0.3 corners and R0.2 top entries; underside channels 0.5 deep leave 1.0 mm of plate beneath the UBEC |
| Tie heads | Place beside the UBEC on the housing-wall side, not on top; cut off the tails. The assembly includes an estimated 4 × 4 × 3 head envelope |
| Printing | Print the carrier with its UBEC contact face down, as in the 3MF, supports off. The full-height legs start on the bed, with vertical screw holes; no projecting screw-seat roofs. Print the base floor down; the 0.4 mm floor beneath each pilot is two nominal 0.2 mm layers |

- Fit the MAX490 on the floor posts, place the carrier over it, and tighten the four shared screws gently.
- Thread the ties through the underside channels before placing the UBEC; check the end solder joints clear the plate, then tighten only enough to hold the module without deforming it.
- Keep the jumper accessible. Leave enough slack in the attached leads to lift the carrier after removing the four screws; the MAX490 is also released when those screws come out.
- Work through the carrier, tie-head and screw checks in [open items](../project/open-items.md#housing).

### 2.5 Later optimizations

- **DE-9 cable tie-down:** a lug below the DE-9 that sticks out past the mated plug, so the cable can be cable-tied to it. The cable then cannot lever the connector out of the housing.
- **Status LED:** a dedicated LED in a hole drilled into the finished hood, if one is ever wanted.

---

<a id="contents"></a>

## 3. 📏 Contents and sizes

Sizes from owner measurements, seller listings or datasheets; see the source column. Remaining measurements: [open items](../project/open-items.md#housing).

| Part | Qty | Size (mm) | Source | Notes |
| --- | :-: | --- | --- | --- |
| ESP32 board (AZ-Delivery, DevKitC V4 layout) | 1 | 48.5 × 28.5 base PCB, excluding antenna | S40 | Antenna end free; Micro-USB at the other end |
| RS-232 module (Hailege H-1-0290) | 2 | 9.2 × 15.6 | S38 | Edge pads, soldered wires |
| RS-422 board (MAX490) | 1 | 50 × 27 | S34 | Screw terminals A/B/Y/Z on one edge, header on the other |
| UBEC (Hobbywing 3A) | 1 | PCB and cover dimensions in the [measured model notes](../../cad/reference/ubec-hobbywing-3a/README.md) | Owner measurements | Allow extra space for the jumper and leads. |
| UBEC ferrite ring | 1 | Ø14.2 outside × 5.4 axial, 2.9 radial wall (Ø8.4 bore calculated) | Owner measurements | On the UBEC 5 V output lead. Reserve 30 × 30 × 15 for the ring and wire turns pending measurement of the wired envelope. |
| XT30 male, panel | 1 | 10.55 × 5.25 body; 10.8 × 5.66 over the raised markings at the solder-cup end (owner measurement) | S51 | Press-fit pocket ([section 4](#connectors)) |
| DE-9 male, panel | 1 | Flange 31 × 12.6 × 1.0, holes Ø3.1 at 25.0 spacing; D shell 17.85 × 9.2 outer; rear body behind the flange 19.3 × 10.9 × 5.0 deep, 8.0 including the solder cups (owner measurement) | S43 | Flange and rear side as the female plug model |
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
| Mounting holes | 4 × Ø3.2, spacing 21.6 × 34.8 | Diameter confirmed by owner (S53); spacing from photo scale |
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

- Reference for the flange and rear side: [female plug STEP](../../cad/reference/dsub-9pin/DSUB_9pin_female_plug.STEP) (S43), the female variant; the delivered male has the same flange (31 × 12.6 mm) and rear side, its D shell measures 17.85 × 9.2 mm.
- Male connector (pins) on the housing, female cable connector with solder cups and screw locking.
- Front mount: flange in a flush recess on the outer face, rear body (19.3 × 10.9 mm) through a D-shaped cutout that follows its tapered sides; the cable connector seats fully because no wall is in front of the flange.
- Backshell and strain relief on the cable side.
- Cable-side clearance references: the same [female plug STEP](../../cad/reference/dsub-9pin/DSUB_9pin_female_plug.STEP) for the female cable connector (S43), and the [backshell STEP](../../cad/reference/dsub-9pin/DSUB_9pin_backshell.STEP), a straight plastic backshell with 4-40 thumbscrews, about 31.5 × 38.1 × 15.2 mm (S52).
- Standard 4-40 jackposts (stud length about 4.8 mm, estimate; not delivered yet): through 1.0 mm flange and 2.0 mm wall, the nut pocket is sunk into the inner face so the nut gets full thread.
- Housing side: jackposts ([design rules](#rules)); exact jackpost and local wall thickness: [open items](../project/open-items.md#bench-hardware).

---

<a id="layout"></a>

## 5. 🧩 Internal layout

> [!TIP]
> **Selected layout:** Fusion document `Housing layout - compact v4` in the Segelflug-WifiBridge folder, with the holder `esp32 holder v3 (RS-232 clamp)`; repository copies in [cad/housing/layout-v4](../../cad/housing/layout-v4/README.md). Inside about 87 × 60 × 20 mm (L × W × H, from the walls' inner faces, with 2.0 mm side walls inside the 64 mm plate width; the 20 mm height is an estimate for the ESP32 header wires). The document `Housing layout` keeps the parts in their first arrangement as the untouched master.

| Column | Parts, from the connector wall |
| --- | --- |
| ESP32 side (−Z) | XT30 low in the wall (body from 3.9 above the floor, collar standing on the floor); in the housing body 4.2 mm further from the side wall than in layout v4 (body from 7.0 inside the inner side-wall face, so the corner post keeps 2.0 mm around its insert with the screw 5 mm from the side face), with a smaller, rounded collar · ESP32 (USB edge X = 27, centre Z = −10.5, antenna end X = 81) with both RS-232 boards on its USB clamp; the clamp starts 3.8 mm behind the XT30 body |
| MAX490 side (+Z) | DE-9, in the housing body 4 mm closer to the XT30 than in layout v4 (centre Z 8.5), so its nut pocket clears the corner post · in the housing body this side is 3 mm wider than in layout v4, and the MAX490 and UBEC sit 3 mm further out (MAX490 board Z 8–35) · MAX490 on the floor (terminal edge X = 17, PCB underside 4.5) with the UBEC on the [four-screw carrier](#ubec-carrier), jumper toward the wall · ferrite ring on edge behind the UBEC output end |

- **ESP32 holder:** tested USB block B (orange in Fusion), antenna cap A without foot ears (31.9 wide, blue), and a USB clamp UR carrying both RS-232 boards (green). The boards sit turned 90°, one behind the other across the ESP32 between its header rows, 13.8 above the floor (rails to 16.1). One M2.5 screw removes the clamp with both boards.
- **Clamp arm:** a 6 × 2 mm arm runs from the clamp's free end over the ESP32 module into a short tunnel on top of the antenna cap's spring bridge. It only locates the free end, without loading the spring bridge (cap top 15.0 above the floor). Gaps, sized for print tolerances and the ~0.4 mm lift of the bridge once the ESP32 is fitted:

  | Gap (mm) | Modelled | ESP32 fitted |
  | --- | --- | --- |
  | Below the arm | 0.7 | about 0.3 |
  | Above the arm | −0.1 (overlap in the model) | about 0.3 |
  | Each side | 0.25 | 0.25 |
  | Arm tip to tunnel end | 0.2 | 0.2 |

- **Fitting the clamp:** push the arm tip into the tunnel with the USB end raised, then lower the clamp onto the USB socket and screw it down.
- **Assembly order:** solder the RS-232 wires on the bench, slide the boards into the clamp, fit the ESP32, then screw the clamp on.
- **UBEC carrier:** lift it off for access to the MAX490 header joints. Mounting and tie routing: [section 2.4](#ubec-carrier).
- **Wiring:** XT30 wires run over the USB clamp to the UBEC input. With the XT30 low, its solder cups end level with the ESP32 USB screw block (top 9.25, 0.3 mm behind the cups), so the wires must bend up right at the cups; not modelled yet.
- **Ferrite ring placement:** in the housing prototype the measured ring stands on the floor at X 83–88.4. The wire turns still need clearance.
- **Behind the connector wall:** 12–16 mm free depth before the ESP32 USB block, the USB clamp and the MAX490 (Fusion check), too short for a panel fuse holder; F1 sits in the harness ([protection](power.md#protection)).
- **Corner posts:** the measured ferrite ring clears the far post on the MAX490 side in Fusion.
- **Width:** the housing body is 67 mm wide, 3 mm more than layout v4 on the MAX490 side. With 2.0 mm side walls the antenna cap sits 1.0 mm and the MAX490 board 0.5 mm from the inner side-wall faces; the gap between the ESP32 and MAX490 columns is 3 mm larger than in layout v4.
- **Hood shape:** rounded top edges, a cut-off rear corner on the MAX490 side and a lower hood over the ring area. A curved hood saves little: the tallest items (ESP32 header wire rows, UBEC) sit close to both side walls.
- **Fit test:** the first print of holder v3 fits (clamp printed top up with support under the arm and slot plate; arm tilts into the tunnel).
- **Not modelled yet:** internal wire routing and ring wire turns.

Components in the Fusion layout v4. The [housing prototype archive](../../cad/housing/housing-body-v1/README.md) uses the measured UBEC and ferrite ring and embeds the MAX490 board.

| Component | Source |
| --- | --- |
| Connector side | Plate (`plate_w` 64, `plate_h` 25, `wall_t` 3 mm) with the tested DE-9 and XT30 features (XT30 raised); XT30 and DE-9 reference models |
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
- **Power path:** F1 (harness) → XT30 → UBEC → ESP32 and MAX490 ([supply path](power.md#supply)).
- **Ferrite ring:** keep its placeholder volume ([section 3](#contents)) at the UBEC's output end (opposite the jumper), on the 5 V lead towards the ESP32; the ring is not modelled in the UBEC CAD reference.
- **ESP32:** lengthwise, antenna at the far short end; Micro-USB faces the connector end.

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
