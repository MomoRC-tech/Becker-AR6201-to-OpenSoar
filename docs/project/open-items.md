<a id="top"></a>

# ❓ Open items

🏠 [Overview](../../README.md) › 📋 Project › **Open items**

The one list of everything still missing: what is needed, who provides it, and what it unblocks. Other pages link here instead of repeating open questions.

> [!NOTE]
> **Status:** all items open. Current focus: the bench hardware checks and the housing inputs.

📑 **On this page:** <kbd>[1 · 🔧 Bench hardware — current focus](#bench-hardware)</kbd> <kbd>[2 · 📦 Housing — current focus](#housing)</kbd> <kbd>[3 · 🛩️ Installed equipment](#installed-equipment)</kbd> <kbd>[4 · 💻 Software and acceptance — later](#software-and-acceptance)</kbd>

Related: 🗺️ [Roadmap](roadmap.md) · 🤔 [Assumptions](../design/requirements.md#assumptions) · 🛩️ [Existing installation](existing-installation.md)

---

<a id="bench-hardware"></a>

## 1. 🔧 Bench hardware — current focus

Checks on the delivered parts before the bridge is connected to any device. Step-by-step order: [bench bring-up](../hardware/wiring.md#bring-up).

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | MAX490 board | Which header pin is RO and which is DI; termination; current draw | Bench check on the delivered board |
| 2 | GPIO16 input | High and low level at GPIO16 behind the divider, also at power-up and power-down | Bench measurement |
| 3 | RS-232 modules | Continuity of both modules against the inferred pad map, including R1/R2 signal functions (the CAD signal arrows differ from the supplied photo); IC marking | Bench check |
| 4 | ESP32 board | Module marking and header positions match the DevKitC V4 reference | Visual check |
| 5 | UBEC | 5 V output with and without load; match the physical input/output polarity and XT30 cups to the labelled routing endpoints; fit in the enclosure | Bench measurement |
| 6 | Current and fuse | Running current and switch-on inrush at 12 V; select fuse rating and speed using these measurements and the wire protection requirements | Bench measurement and owner selection |
| 8 | Device connectors | Photos of the installed connectors and exact device models (from Tom) | Confirms the pin views in the wiring tables |
| 9 | Becker J1 | Installed J1 revision, moulded pin numbers and which pins are already used | Confirms the J1 pins and the J1-21 ground |
| 10 | Radio noise | No audible noise or squelch opening caused by the bridge | Check in the aircraft |
| 11 | Power connector fit and retention | Adhesive compatibility with the XT30 body and PETG, wire support and insulated terminations ([selected arrangement](../hardware/housing.md#connectors)) | Connector-panel print and assembly check |
| 12 | Supply-cable protection | Owner confirmation of cable length, routing and wire specification; where F1 sits at the supply end ([protection](../hardware/power.md#protection)) | Owner supplies harness details; agree cable protection before aircraft connection |
| 13 | F1 holder | Inline holder in the 12 V harness at its supply end is selected; holder part and fuse format ([protection](../hardware/power.md#protection)) | Owner selection with item 6 |
| 14 | Combined DE-9 instrument connection | Exact mating parts, wire-gauge compatibility, screw threads, backshell, strain relief and housing mounting. Identify the loose conductors and verify shield termination against the [selected pinout](../hardware/wiring.md#de9-pinout) and [Becker wiring](../hardware/wiring.md#becker). | Owner selects parts and mounting details. Check fit and continuity before connection. |

---

<a id="housing"></a>

## 2. 📦 Housing — current focus

Inputs still needed before the housing can be modelled. Selected rules: [housing design rules](../hardware/housing.md#rules). Connector details are items 11 and 14 above. Work top-down: each group sets limits for the next.

### 2.1 Installation and environment

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 3 | Location in the aircraft | Which shaded spot the bridge sits in, maximum envelope (L × W × H) at that spot, orientation, and which structure the cable ties through the four base tie loops go around | Owner / Tom; photo and measurement of the spot |
| 7 | Cable exits | Direction from which the 12 V harness and the instrument cables arrive at the connector side; free space in front of it for the mating plugs, DE-9 backshell and cable bend radius | Owner, from item 3 |
| 8 | Moisture | Moisture or condensation at the location (shaded spot selected) | Owner estimate |
| 9 | Distance to compass and radio antenna | Minimum distance from the magnetic compass (UBEC coil, ferrite ring, steel screws and inserts) and from the VHF antenna cable | Owner / Tom; compass check in the aircraft |
| 6 | Wi-Fi path | Antenna position relative to the phone and surrounding structure | Owner selection; check in the aircraft |

### 2.2 Manufacturing and print rules

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 10 | Print dimensions | Free-fit and snug-fit gaps, smallest feature and bridge length are still [starting values](../hardware/housing.md#print-values) | Check on the first housing print |

### 2.3 Construction

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 4 | Holders | Slice, print and fit the [UBEC carrier](../hardware/housing.md#ubec-carrier): check support-free legs, end solder clearance, 19 mm support, tie threading and actual tie-head size, hood clearance, M2.5 × 12 head/driver access, the offset shafts through the PCB holes and thread holding; check the base's R1 post roots and 0.4 mm pilot-hole floors. Choose a glue that holds on PETG (ESP32 holder, XT30) | Owner / fit test; remaining selections separately |
| 15 | Ferrite cradle | Slice and fit-print the [cradle](../hardware/housing.md#ferrite-cradle). Check the exposed core sector seats without trapping wire turns; confirm lead exits, hood and rear-strap clearance, actual tie thickness/head size, threading through the printed tunnels and tool access. Check retention with the hood removed, under vibration and at the expected loaded temperature | Owner / physical fit test |
| 18 | Centre-floor wire guide | After the overall routing in item 14, compare an integral channel with discrete open guides. The channel study uses X 30–68 mm, centred at Z 5.5 mm in the [layout frame](../hardware/housing.md#internal-routing). Starting envelope: 6.4 mm outside width, 4.6 mm clear width and 3.0 mm height above the floor (S67). Allow top loading, side exits for board branches and a rise before the rear floor obstruction. Fit-check the actual bundle, exit bends and access to the separate junctions | Owner chooses guide style and location |
| 19 | Raised ferrite-end guide | Compare the low channel alone with a short, rounded extension of its MAX490-side wall. Trial position X 65–68 mm and Z 7.8–8.7 mm, up to 9 mm above the floor (S68). Use the raised wall to guide the pair through the rise while keeping top access. Check PCB clearance, wire bend radius, clearance to the 5 V lead and resistance to chafing with the actual bundle | Owner chooses whether to add the raised guide. Fit-check with item 18 |
| 14 | Internal wiring details | Select wire type/gauge and allowable bends. Fit-check the inboard XT30 pair and carrier tool access using the [routing study](../hardware/housing.md#internal-routing). Check hood gaps, bends, supports, tie tunnels and seam. Set termination allowance and service slack. Complete restraint beside R-B1/R-D and the remaining harness routes | Owner selection and physical routing/maintenance check |
| 21 | Ferrite supply and return fit | Fit-check the modelled base-plate runs on both sides of the ferrite with cradle ties and actual windings in place. Check bends, clearance and service slack | Physical fit check |
| 22 | RS-232 harness routing | Lay out the two DE-9-to-module input leads and the two module-to-ESP32 receive leads from the [signal wiring](../hardware/wiring.md#zander). Keep each short inter-board supply and return link together and keep the pull-ups at the pads. Compare a removable keeper on the existing RS-232 clamp arm with a guide on the base. Check that pre-soldered modules can slide into the clamp and that the clamp can lift clear with both modules, with slack at the pads and access to its screw | Owner chooses the keeper location after a Fusion routing and removal check. Verify pad functions on the delivered boards before soldering |
| 23 | MAX490 logic routing | Lay out short blue RO and DI runs in the gap to ESP32 J3 using the [logic-side wiring](../hardware/wiring.md#becker). Keep R-B1/R-B2 at GPIO16 and R-D beside GPIO17. Reserve room for the heat-shrink and support the leads by their insulation next to the ESP32. Check clearance to the revised power routes, hood, header joints and carrier screwdriver paths. Leave the MAX490 ends provisional until RO and DI are identified | Owner reviews the route and retention. Verify RO/DI on the delivered board and measure GPIO16 through the divider on the bench |
| 17 | Central distribution | Select insulated 5V-J and GND-J connection hardware and mounting; fit the reserved space, support the leads, preserve removal access, and rate the full ground path for the documented fault currents. Verify the ESP32 → RS-232-mod2 → mod1 supply/return chain has no redundant ground link | Owner selects hardware; bench continuity and fit check |
| 20 | Centre-channel wire retention | After routing and guide selection, evaluate staggered fingers extending inward from alternating sides. Feed the wires from above around the rounded tips, then straighten them beneath the fingers with room to move. Place fingers between branch exits. Fit-print to check insertion, removal, retention, insulation clearance and printability | Owner chooses retention after the route layout. Fit-check with the actual wires |
| 16 | Internal cable saddles | Slice and fit-print the two open 12 V saddles; thread the actual ties through the 3.2 × 1.6 mm passages, check tie-head placement, insulation clearance and access with the hood off | Owner / fit test |

### 2.4 Measurements

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | Part sizes | Compare the ESP32 with its [CAD reference](../hardware/housing.md#contents): solder-joint height underneath, header spacing, overall length including antenna and USB protrusion, overall height with headers, USB-plug and wiring clearance; measure RS-232 assembled height on both sides and soldered-wire clearance against its CAD reference on the same page; measure the RS-422 board's hole positions, PCB thickness and screw-terminal height against its CAD reference; measure the DE-9 jackpost stud length once delivered; check the UBEC and ferrite lead-exit envelopes beyond the recorded body and winding dimensions | Bench measurement |

---

<a id="installed-equipment"></a>

## 3. 🛩️ Installed equipment

Facts about the equipment already in the aircraft, mostly from Tom. What is known so far: [existing installation](existing-installation.md). Closing the interface items is milestone M1.

| # | Item | What is needed | From | Unblocks |
| :-: | --- | --- | --- | --- |
| 1 | Radio | Type plate, article number, hardware/software versions, J1 occupancy of the reported AR6201-(022) | Tom | Matching manual and interface |
| 2 | Zander ZS1 | Program version, baud and output settings, cable | Tom | Serial settings |
| 3 | FLARM | Model and firmware (reported Classic, IGC series 6), socket used, baud and output settings | Tom | Serial settings |
| 4 | Existing wiring | How each device signal reaches the existing ESP32, including any converters | Tom | Backup record |
| 5 | Existing converter | Chip, supply and logic level of the RS422 TO TTL board | Tom | Backup record |
| 6 | Existing ESP32 | Board, firmware, configuration | Tom | Backup record |
| 7 | Existing power | UBEC jumper/output, board input, grounding | Tom / owner | Backup record |
| 8 | Navigation baseline | Data types, update gaps, reconnect behaviour of today's setup | Owner / Tom | Acceptance limits for the new bridge |
| 9 | OpenSoar | Installed build, driver and endpoint settings, role of the OpenVario | Owner / Tom | App configuration |
| 10 | Radio ground | Whether both P1 ground pins (13, 25) run to the battery minus, and whether the radio's grounding bolt is connected | Tom / owner | Confirms the radio cannot return its current through the bridge ([ground](../hardware/power.md#ground)) |

---

<a id="software-and-acceptance"></a>

## 4. 💻 Software and acceptance — later

On hold until the hardware is finished.

| # | Item | What is needed | Unblocks |
| :-: | --- | --- | --- |
| 1 | Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, channel vectors ([details](../research/becker-driver.md#protocol-details)) | Driver plan; independent test vectors |
| 2 | Radio configuration | Matching manual, serial settings, remote-control configuration | Port configuration |
| 3 | Application integration | AR62xx driver over TCP alongside navigation; status display | Driver plan |
| 4 | Firmware selection | Candidate audit, licenses, reproducible build ([ADR-02](../design/architecture.md#adr-02)) | Bridge firmware |
| 5 | Network settings | New-bridge network configuration and device endpoints | App and bridge configuration |
| 6 | Driver regression | Zero/A5 bytes, short frames, bad CRC, unknown start state, two instances, missing or late replies | Software verification (M3) |
| 7 | Bridge reliability | Byte-exact tests both ways, bursts, slow clients, partial writes, reconnects, navigation in parallel | Software verification (M3) |
| 8 | Physical acceptance | Frequency change and readback on the real radio, local controls and navigation unaffected | Aircraft integration |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
