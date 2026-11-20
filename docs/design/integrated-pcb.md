<a id="top"></a>

# 🧩 Integrated bridge PCB

🏠 [Overview](../../README.md) › 🧩 Design › **Integrated bridge PCB**

Use this plan to take the module-based bench bridge through a smaller four-layer design, first articles and a repeatable JLCPCB order.

> [!NOTE]
> **Status:** Branch design proposal. The component shortlist, dimensions and cost examples are planning inputs. Schematic capture, layout, production files and the supplier quote follow owner selection of the hardware.

📑 **On this page:** <kbd>[1 · 🔌 Circuits](#circuits)</kbd> <kbd>[2 · 📐 Housing and layout](#layout)</kbd> <kbd>[3 · 🌡️ Heat and verification](#heat)</kbd> <kbd>[4 · 💶 Production cost](#cost)</kbd> <kbd>[5 · 📦 Design to order](#order)</kbd>

---

<a id="circuits"></a>

## 1. 🔌 Circuits and component shortlist

Start with one rigid PCB, four copper layers, top-side placement. Buy the shielded ESP32 radio module as one SMT component. Integrate the regulator, serial interfaces, protection and support parts on the board. Preserve the [instrument connector contract](../hardware/wiring.md#de9-pinout), [supply measurement](../hardware/power.md#voltage-sense) and [firmware pin assignments](../../firmware/bridge/config.h).

| Function | Proposed parts and connections | Check before freezing |
| --- | --- | --- |
| ESP32 and Wi-Fi | Espressif **ESP32-WROOM-32E-N4**, JLC **C701341**. Shield can, integrated PCB antenna, 4 MB flash, no PSRAM. Local bulk and ceramic bypass, EN pull-up and timing capacitor, defined boot straps. S165 | Retain classic ESP32 target, three hardware UARTs, GPIO16/17 and both OTA slots. Build the existing loader and application for the selected flash layout. |
| Programming and recovery | Accessible top-side 3.3 V-logic UART fixture pads for GPIO1/3, EN, GPIO0, GND and a voltage-reference sense pad. External USB–UART adapter for first flash and recovery. Bridge supplies its own power. OTA for routine updates. | No adapter supply feed. Check for injection through TX while unpowered. Verify manual boot/reset sequence, recovery after a blank or corrupt image, and zero boot bytes on instrument lines. Quote onboard USB–UART and protected USB power separately if cable-only service is required. |
| Zander and FLARM | One **TI TRS3232EIPWR**, two independent RS-232 receivers to GPIO32 and GPIO26. Four charge-pump capacitors sized to the datasheet and local bypass. Define unused logic inputs per datasheet and leave driver outputs unrouted. LCSC **C2652868**. S166 | Two independently timed, simultaneous streams. Check polarity, input loading, powered-off behavior and connector ESD. Protect bipolar RS-232 signals with a suitable working voltage. |
| Becker radio | **TI SN65HVD35DR**, JLC **C411877**, full duplex at 3.3 V. Separate receiver and driver pairs, RO to GPIO16, DI from GPIO17. Local bypass, DI idle bias, explicit DE and /RE states. Native logic levels remove the MAX490 receiver divider. S166 | Compare differential swing under the installed load, common-mode limits, receiver thresholds and polarity both ways. Start with receive termination unpopulated as on the bench. Evaluate connector-side differential protection and optional termination. Preserve shield handling. |
| Radio transmit behavior | Keep the receiver enabled. Evaluate a hardware TX-enable link with default disabled for first-article readback, pulled-up DI and a separately tested, enabled bench-loop configuration. | Confirm the installed interface tolerates a released pair. Capture enable, reset, brownout and power-off on the scope. Confirm idle and byte-exact full-duplex operation in the enabled build. Record link population per build. |
| Input and conversion | Harness-source F1, mating power connector, reverse-polarity MOSFET network, coordinated TVS, bulk and ceramic input capacitors. Candidate **TI LMR33610ADDAR**, JLC **C1852147**, 400 kHz synchronous buck directly to 3.3 V. Include shielded inductor, bootstrap and VCC capacitors, feedback divider, EN network and output capacitors per TI. S167 | Treat 7–20 V on the existing housing as a proposed continuous bench range. Define surge amplitude, duration and source impedance separately. Check reverse voltage, TVS standoff and worst clamp at current against every downstream limit, fuse coordination and inrush. If margins fail, reselect the front end. |
| Input voltage reading | Retain 100 kΩ / 10 kΩ scaling, 100 nF at GPIO34 (ADC1) and a quiet return. Sense the protected connector input ahead of the buck. | Bound protection voltage drop against XT30 readings. Check ADC injection with the 3.3 V rail down, input surges, resistor tolerances and one-point calibration against a meter over the range. |
| Connector and fault returns | Male DE-9 and male XT30 mating interfaces as planning baseline. Retain separate instrument returns to the connector, continuous ground on the board and the radio-end-only shield connection. Mechanical anchors for both plugs. | Audit the entire return from DE-9 to supply minus, including pads, necks, vias and any jumper. The existing radio-ground-break estimate is up to 3 A from the radio and about 3.2 A through the shared return. F1 in bridge positive cannot interrupt that external return. [Fault paths](../hardware/power.md#wire-breaks) |
| Factory test | Probe points for input, protected input, 3.3 V, ground, EN, GPIO34 and accessible radio logic. Exercise all three serial channels through the mating DE-9. | Key the fixture, mark polarity and pin 1. Provide probe access with the board mounted and clearance below through-hole tails. |

For the power design, budget the ESP32 supply capability at **at least 500 mA**, add loaded transceivers and margin, and check the selected 1 A converter at hot ambient. Calculate inductance, ripple, hot saturation and RMS current, input capacitor ripple, effective MLCC capacitance under DC bias, loop stability and startup at both input limits. Capture peak rail droop at the module pins during Wi-Fi bursts. Treat the old UBEC ferrite’s noise suppression as a function to verify. Reserve space for a damped input filter if conducted-noise tests require one. S165, S167.

---

<a id="layout"></a>

## 2. 📐 Smaller housing and layout

| Envelope | Length × width × height | How to use it |
| --- | --- | --- |
| Accepted v16 body | **92 × 70 × 25 mm**, 78 mm wide over tie loops | Same measurement basis for both designs. Floor underside to hood top is −2.5 to +22.5 mm. [Dimensioned baseline](../../cad/housing/housing-body-v1/README.md) |
| First compact housing study | **72 × 60 × 25 mm** body, about **33% less envelope volume** | Proposed fit target. Report body, maximum over mounts, and installed envelope with mated plugs and bend radii separately. |
| First placement trial | **About 64 × 50 mm** host PCB | Working outline only. Reserve antenna overhang, posts, plug depth and rework access before routing. Adjust the body and board together. |

1. Place the mated DE-9, jackscrews, XT30, mounting loops and PCB mounting holes first. Trial the manufacturer STEP models in a **separate compact enclosure design**. Transfer mating and extraction loads to the connector wall. Check board insertion, solder tails, driver access and strain relief. Compare a board-mounted connector with the current panel part and short restrained leads if their mating planes cannot align.
2. Put the antenna at the opposite plastic end, preferably projecting beyond the host board edge. Apply Espressif's keepout across **all layers** and check its recommended 15 mm antenna clearance in the assembled product. Include screws, harness, mounting structure and panel metal in the RF fit check. S165.
3. Put input protection at the power entry and line protection at the DE-9. Keep the buck input switching loop small, the switch-node copper compact and the inductor away from the antenna, ADC and radio pairs. Use L1 for components and short signals, L2 for uninterrupted ground under routed signals, L3 for power and ground, L4 for low-speed signals and ground copper. Tie ground pours with vias outside the antenna keepout.
4. Print a connector-wall coupon and a base with a dummy board before freezing mounting holes. Start from the [PETG fit allowances](../hardware/housing.md#fit-test). Install heat-set inserts before the electronics. Check screw clearance, anti-rotation, mated pull load and movement at the PCB joints with the housing hot.
5. If top-side placement fails, report the collision and compare a slightly larger outline, bottom-side placement and a daughterboard using **installed dimensions, price and thermal results**. A stack adds a second fabrication, connectors, retention and another heat pocket. Authorize either alternative only for a measured fit or service advantage.

---

<a id="heat"></a>

## 3. 🌡️ Heat, radio noise and acceptance

| Stage | Method and release evidence |
| --- | --- |
| Estimate heat | Illustrative load at 3.3 V and 0.30 A gives 0.99 W delivered. At an assumed 85% buck efficiency, converter loss is 0.17 W and input power is 1.16 W. Most of that input power can heat the enclosure. Size copper from regulator-package loss and the TI layout, then measure complete-board input power. The module bridge measured 0.72–1.08 W at 12 V, instruments absent (S147). |
| Spread heat | Solder the regulator exposed pad to ground copper with the TI land pattern and thermal vias. Agree paste apertures and via treatment with assembly to avoid solder wicking. Check inductor temperature, capacitor ripple ratings, ESP32 shield temperature and the closest loaded PETG boss. Leave access for fine-wire thermocouples. |
| Assemble | Verify each part's moisture and solder profile, especially connectors. The ESP32 is MSL 3 and Espressif specifies **one reflow** (S165). If two-sided placement becomes necessary, agree the sequence so the module sees one pass. Specify selective or hand solder for connectors whose plastic cannot take the reflow profile. |
| Enclosure soak | Record shaded mounting-site hot ambient and cold storage/start conditions. Run min/nominal/max input, Wi-Fi transfer and all ports active in the closed box, in mounting orientation. Log ambient, internal air, module, regulator, inductor and loaded plastic to thermal equilibrium. Estimate junction temperature using the vendor's board-appropriate method. Require measured margin to each rated limit and retention without creep after the hot soak. Use the measured installation envelope to set numeric pass limits before testing. |
| Temperature cycling | Cold start after dry equilibration, then check condensation after a cold-to-warm transition with power removed. Inspect connector contacts, drains, PCB and antenna. Compare side vents with reduced openings only if water exposure or heat warrants it. PETG specimen heat-deflection values do not establish a loaded boss's service limit. [Environment and materials](../hardware/housing.md#material) |
| First article, bench fixtures | Before power, inspect polarity, pin orientation and continuity. Use a current-limited supply. Scope startup, load steps, ripple and brownout. Verify recovery/OTA, ADC accuracy, two concurrent RS-232 inputs, separate radio RX/TX loopback, readback build TX state and the three-port endurance test. Record resets, lost bytes and rail minima against target limits. Use a current-limited dummy return path to verify ground heating. |
| RF and interference | Compare Wi-Fi throughput and range against the working bridge in the same position with the lid closed. Compare bridge off/on at identical receiver settings for VHF noise or squelch changes under Wi-Fi and converter load. Capture conducted noise at the power leads. Use the bench result to select filtering and shielding. Installed equipment compatibility and aircraft release follow [WP8](../project/roadmap.md#work-packages). |

---

<a id="cost"></a>

## 4. 💶 Production cost

JLC lists the shortlisted ESP32 (C701341) as **Standard PCBA only**. Start with Standard single-sided assembly. The published setup and stencil fees give the following fixed-cost comparison in USD (S168). Quantities are examples for comparing quotes.

| Standard assembly | Setup + stencil per order | Per board at 5 assembled | Per board at 20 assembled |
| --- | ---: | ---: | ---: |
| Top-side SMT | $33.77 | $6.75 | $1.69 |
| Double-sided SMT | $67.54 | $13.51 | $3.38 |

Request complete prices at **5 and 20 assembled** using the same revision, connectors and finish. Include bare boards, parts and attrition, feeder loading, placement, inspection, fixtures, connector soldering, shipping and applicable tax/import charges. Divide by usable assembled units. Quote local connector fitting as a separate option with parts and labour. Add housing, hardware, harness fuse and test time for comparison with the [€43.60 prototype hardware baseline](../project/cost.md#scope), whose scope excludes shipping and labour.

For a budget sensitivity check, assume **$12–20 per populated board** for bare PCB, all parts and placement, plus **$40–80 per lot** for loading, inspection and other processing. These are engineering allowances pending a costed BOM. Adding the published single-side setup and stencil gives **$134–214 for five** (about **$27–43 each**) or **$314–514 for twenty** (about **$16–26 each**), before shipping, tax, housing and functional test. Replace each allowance with its quoted line before deciding whether the compact design saves money.

Use common passive values and packages where ratings permit, standard through vias, and the supplier's stock four-layer build. Check exact manufacturer, suffix, package, process, quantity and lead time in the assembly library before layout freeze and again at quote. For unavailable parts, review an alternate against pinout, electrical limits, temperature and land pattern before approval.

---

<a id="order"></a>

## 5. 📦 From design to JLCPCB order

| Gate | Work product | Ready to proceed when |
| --- | --- | --- |
| 1 · Freeze interfaces | One-page interface and environmental limits sheet, mated connector drawings and compact fit study | Owner selects hardware choices individually. Record instrument variants, electrical limits, allowed TX states, ground-fault case and thermal test limits. Track missing inputs in [open items](../project/open-items.md#pcb-study). |
| 2 · Capture the circuit | Separate KiCad PCB project. Sheets for power/protection, ESP32/service, navigation and radio. Exact MPN, supplier code, rating, land pattern and fitted/DNP state on each line | Rev A schematic owns its board nets. Compare the netlist against the existing harness contract and firmware pin map. Review worst-case power, fault and logic calculations, ERC and every remaining warning. Check footprints against manufacturer drawings. |
| 3 · Place and route | Four-layer board and STEP export, mated compact housing study, placement and thermal review | Fit with connectors and service fixture, continuous return paths, fault-current capacity, antenna clearance, thermal pad and test access. Check JLC stackup, copper, annular rings, drills, mask/paste, routed slots, board-edge distances and panel rails/fiducials. DRC and schematic/PCB parity pass. |
| 4 · Obtain prototype quote | One revision-stamped ZIP with Gerbers for every copper, mask, silk, paste and outline layer, plus Excellon plated/non-plated drills. Separate JLC BOM and CPL, dimensioned fabrication and assembly drawings, fitted/DNP and hand-fit list, editable KiCad source and STEP | Reopen plots/drills in an independent viewer. Match fitted designators exactly between BOM and CPL. BOM carries value, footprint, designator and JLC/LCSC code. CPL carries designator, X/Y in mm, rotation and side using one origin. Inspect polarity, pin 1, module antenna direction, connector face and rotations in JLC's placement preview. S169. |
| 5 · Owner order | Dated quote or reproducible ordering pack with stackup, board thickness/copper/finish, mask, assembly side, service tier, quantities, accepted parts, stock snapshot, lead time, shipping and price breakdown | Every fitted part is matched or explicitly listed for local fitting. Record any substitutions, quote validity shown by vendor and assembly-review corrections. Owner reviews final amount and places the prototype order. |
| 6 · Accept and repeat | Serialized first-article results against section 3, defects and corrected revision where needed. Bundle flash/recovery instructions and fixture pinout with the release | Run the closed-box thermal, electrical, mechanical and radio tests on actual hardware. Use the accepted revision and a fresh stock/price check for the production order. |

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
