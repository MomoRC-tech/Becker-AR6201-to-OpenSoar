<a id="top"></a>

# ❓ Open items

🏠 [Overview](../../README.md) › 📋 Project › **Open items**

The one list of everything still missing: what is needed, who provides it, and what it unblocks. Other pages link here instead of repeating open questions.

> [!NOTE]
> **Status:** all listed items are open. Current focus: bench hardware. Housing checks carry forward to prototype assembly and installation.

📑 **On this page:** <kbd>[1 · 🔧 Bench hardware — current focus](#bench-hardware)</kbd> <kbd>[2 · 📦 Housing fit and installation](#housing)</kbd> <kbd>[3 · 🛩️ Installed equipment](#installed-equipment)</kbd> <kbd>[4 · 💻 Software and acceptance — later](#software-and-acceptance)</kbd> <kbd>[5 · 🧩 Integrated PCB study](#pcb-study)</kbd>

Related: 🗺️ [Roadmap](roadmap.md) · 🤔 [Assumptions](../design/requirements.md#assumptions) · 🛩️ [Existing installation](existing-installation.md)

---

<a id="bench-hardware"></a>

## 1. 🔧 Bench hardware — current focus

Start with the delivered boards and power supply, with all instruments disconnected. See [bench bring-up](../hardware/wiring.md#bring-up) for the order; installation checks are identified separately there.

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | MAX490 board | Current draw | Bench check on the delivered board |
| 3 | RS-232 replacement modules | IC marking, continuity against the [pad map](../hardware/modules.md#rs232-module) and R1 as the TTL output | [Loose-board check](../hardware/wiring.md#bring-up) before fitting a replacement |
| 5 | UBEC | 5 V output without load; match the physical input/output polarity and XT30 cups to the labelled routing endpoints; fit in the enclosure | Bench measurement |
| 6 | Fuse | Select fuse rating and speed from the running current (60–90 mA at 12 V, S147) and the wire protection requirements; switch-on inrush is not measured | Owner selection |
| 8 | Device connectors | Photos of the installed connectors (from Tom); device models are [installed-equipment items 1–3](#installed-equipment) | Confirms the pin views in the wiring tables |
| 9 | Becker J1 | Installed J1 revision, moulded pin numbers and which pins are already used | Confirms the J1 pins and the J1-21 ground |
| 10 | Radio noise | No audible noise or squelch opening caused by the bridge | Check in the aircraft |
| 11 | Power connector fit and retention | Adhesive compatibility with the XT30 body and PETG, wire support and insulated terminations ([selected arrangement](../hardware/housing.md#connectors)) | Connector-panel print and assembly check |
| 12 | Supply-cable protection | Owner confirmation of cable length, routing and wire specification; where F1 sits at the supply end ([protection](../hardware/power.md#protection)) | Owner supplies harness details; agree cable protection before aircraft connection |
| 13 | F1 holder | Inline holder in the 12 V harness at its supply end is selected; holder part and fuse format ([protection](../hardware/power.md#protection)) | Owner selection with item 6 |
| 14 | Combined DE-9 instrument connection | Exact mating parts, wire-gauge compatibility, screw threads, backshell, strain relief and housing mounting. Identify the loose conductors and verify shield termination against the [selected pinout](../hardware/wiring.md#de9-pinout) and [Becker wiring](../hardware/wiring.md#becker). | Owner selects parts and mounting details. Check fit and continuity before connection. |
| 15 | Supply voltage accuracy | Compare the calibrated reading with a multimeter through 20 V as specified in [bench step 13](../hardware/wiring.md#bring-up) | Bench builder records the error at each voltage |

---

<a id="housing"></a>

## 2. 📦 Housing fit and installation

Checks on the [housing v16 bench prototype](../hardware/housing.md) as built, and on its eventual placement in the aircraft.

### 2.1 Installation and environment

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 3 | Location in the aircraft | Which shaded spot the bridge sits in, maximum envelope (L × W × H) at that spot, orientation, and which structure the cable ties through the four base tie loops go around | Owner / Tom; photo and measurement of the spot |
| 7 | Cable exits | Direction from which the 12 V harness and the instrument cables arrive at the connector side; free space in front of it for the mating plugs, DE-9 backshell and cable bend radius | Owner, from item 3 |
| 8 | [Winter moisture and summer heat](../hardware/housing.md#climate) | Coldest intended start, wetting after covered storage, hottest parked mounting spot and powered temperature rise | Owner / bench builder. Use the checks below before deciding on a different hood. |
| 9 | Distance to compass and radio antenna | Minimum distance from the magnetic compass (UBEC coil, ferrite ring, steel screws and inserts) and from the VHF antenna cable | Owner / Tom; compass check in the aircraft |
| 6 | Wi-Fi path | Antenna position relative to the phone and surrounding structure | Owner selection; check in the aircraft |

**Item 8 — proposed climate checks**

- [ ] At the proposed spot, log air temperature and relative humidity through damp nights, thaw and a sunny closed-canopy stop. Record cover and canopy position. With the bridge unpowered, compare a board or connector surface temperature with the local dew point. Look for droplets, tide marks and corrosion inside the hood and at both plugs. A dry afternoon inspection can miss overnight dew.
- [ ] Read both RS-232 chip markings and the MAX490 suffix. Check operating and storage limits for the delivered boards, UBEC and actual filament. Record any missing limit and the margin available at the coldest intended start and hottest operating condition.
- [ ] On the bench, reproduce the hot mounting-site air temperature and orientation after an unpowered soak. Run the fully wired bridge on its normal supply with sustained Wi-Fi and representative serial traffic. Log input volts and amps, air beside the box and RS-232 boards, surface temperatures at the ESP32 regulator, UBEC and loaded PETG clamp, and the bridge's [chip temperature](../../firmware/bridge/README.md#chip-temperature), until stable. Check for resets, serial errors and loss of retention after cooling.
- [ ] If considering a solid hood, repeat with the slots temporarily blanked on the dry bench, at the same ambient, load and clearances. Compare each temperature rise above local ambient. Stop before an applicable limit. Advance the preferred configuration only with dry boards and connectors, temperature margin and retained fit. Resolve any water path, unknown critical rating or loss of margin before installation.

### 2.2 Construction

Fit and assembly of the bench build passed (S156).

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 20 | Internal harness retention | Check the [floor retainers](../hardware/housing.md#floor-guide), the [ferrite cradle](../hardware/housing.md#ferrite-cradle) and support at the DE-9 breakout, signal runs and board branches against [R11](../design/requirements.md#requirements): movement, chafing and continuity after representative vibration, and cradle retention at the expected loaded temperature | Physical vibration and heat checks |
| 17 | Ground path rating | Rate the full ground path through the [GND-J splice](../hardware/housing.md#internal-routing) for the documented fault currents | Owner check against the [power page](../hardware/power.md) |

---

<a id="installed-equipment"></a>

## 3. 🛩️ Installed equipment

Facts about the equipment already in the aircraft, mostly from Tom. What is known so far: [existing installation](existing-installation.md). Closing the interface items is milestone M1.

| # | Item | What is needed | From | Unblocks |
| :-: | --- | --- | --- | --- |
| 1 | Radio | Type plate, article number, hardware/software versions of the reported AR6201-(022); J1 occupancy is [bench item 9](#bench-hardware) | Tom | Matching manual and interface |
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

See the [bridge firmware plan](roadmap.md#firmware-plan) and the [OpenSoar readback-only patch](../research/becker-driver.md#consequences).

| # | Item | What is needed | Unblocks |
| :-: | --- | --- | --- |
| 1 | Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, channel vectors ([details](../research/becker-driver.md#protocol-details)) | Standby tuning |
| 2 | Radio configuration | Matching manual; serial settings (the bridge uses 9600 8N1 until confirmed, [check](../research/reviews/2026-11-17-bridge-projects-check.md#other-projects)); remote control and Tandem mode enabled, as LXNAV and SD-Link require; this changes how the radio behaves, so the owner or installer decides before it is switched on | Port configuration |
| 3 | Application integration | Build the patched OpenSoar app for Android and check AR62xx over TCP alongside navigation | Application acceptance |
| 5 | Network settings | Network to join in the aircraft and the passwords of the new bridge; hotspot name ([ADR-05](../design/architecture.md#adr-05)), address and ports ([firmware plan](roadmap.md#firmware-plan)) are selected | App and bridge configuration |
| 7 | Bridge reliability | Byte-exact tests both ways, bursts, slow clients, partial writes, reconnects, navigation in parallel | Software verification (M4) |
| 8 | Physical acceptance | Tune locally on the matching radio, check both frequencies in OpenSoar and verify that local controls and navigation keep working | Radio readback acceptance |
| 9 | Firmware review checks | Cross-port delay during bulk writes at low baud and remaining connection capacity. Repeated clean closes on quiet Zander and the ordinary two-app connection load passed (S160). [Firmware review](../research/reviews/2026-11-19-firmware-0.5.4-review.md#missing-checks) | Severity of findings 1 and 4 |
| 10 | mDNS memory error | One `mdns_networking: Cannot allocate memory` error while the radio port refused repeated clients (S150): cause, and whether it repeats or affects the ports | Bridge reliability |
| 11 | Settings under power loss | Interrupt a save and an automatic rollback on a spare ESP32, then check that the stored configuration is complete (S162) | Physical settings persistence |
---

<a id="pcb-study"></a>

## 5. 🧩 Integrated PCB study

| Input or decision | Owner | What it releases |
| --- | --- | --- |
| Wider front-panel placement | Owner to review the [candidate and fixed-axis comparison](../design/integrated-pcb.md#placement-comparison). Check the proposed right wall and DA-15 opening, real plug and screw access, vent alignment and all probes with the three cables fitted. The layout designer should regroup C16 with the buck output bank before routing; the current L1-to-C16 centre distance is 42.1 mm. | Front-panel and placement approval for routing |
| Installed equipment, mating parts, space for connector removal, tie mounts and final housing supports | Owner and designer to compare installed parts, a print or coupon and manufacturer patterns | Lock external clearance, connector loads and pin 1 orientation |
| DA-15 production harness | Select the female DA-15 and hood, check contact derating and the 4-40 screws, then verify panel capture and plug withdrawal with the XT30 mated. Obtain the through-hole assembly quote for the [selected J1](../design/integrated-pcb.md#smaller-dsub). | Mated fit and ten-board pricing |
| Continuous input, surges and shared fault return | Record voltage, polarity, duration, repetition and source impedance/current at the XT30 for switching and credible faults. Use a representative dummy harness to measure mated contact and wire temperature/drop during ground loss. On a controlled bench reproduce the input envelope and scope the protected input, regulator VIN, gate/source, 3.3 V and GPIO34 including hard collapse and live reversal. | Coordinate F1, MOSFET, TVS, capacitors, ADC limits and 36 V regulator margin |
| Source fuse and internal fault clearing | Identify the actual branch, loads, protection part and rating, wire gauges and lengths. Measure integrated inrush, bound prospective fault currents and compare clearing times with input-path withstand. Use a controlled bench for fault tests. | Decide if the installed protector can serve as F1 and whether a second device improves [board protection](../hardware/power.md#integrated-fuse) |
| Cold outdoor storage, condensation and parked cockpit heat | Log site RH, surface and air temperatures through cold-to-warm changes and hot soak. Check loaded PETG, mated connectors and closed-box component temperatures. | Choose climate margins, coating and a tested operating/storage envelope |
| Connector and enclosure first article | Agree a finished-hole tolerance or lot acceptance by gauge for J1’s 3.10 ±0.10 mm board locks. On a coupon or first article check both connectors, molded XT30 polarity to pads and harness with power off, DA-15 pin 1, plug and jackscrew access and transfer of connector loads into the housing. With the restrained harness, log contact interruptions and rail resets through a representative three-axis vibration exposure. Recheck contact drop and fasteners after hot soak. | Physical fit and continuity before applying power |
| Antenna and compact housing | Compare bare-board and closed-box Wi-Fi range, throughput and reconnects with the intended fasteners, mated harness and phone orientations. Repeat with Bluetooth active and in fallback. Include the expansion envelope. Measure tip-to-wall clearance again after restoring both front connectors. The [side-entry study](../design/integrated-pcb.md#placement-comparison) measured 3.31 mm. | Owner accepts the measured operating range or selects a housing change |
| ESP32 and buck exposed pads | For the ten-board quote send the S179 artwork and identify U4’s mask-defined pad. Ask JLCPCB to specify hole treatment, stencil openings and thickness, finish, price and first-article joint inspection. A tented U1 option requires revised mask artwork and fresh plots. | Accepted production files, solder joints and loaded buck temperature |
| Power-stage acceptance | Capture U4 VIN, VOUT and PG during hard input collapse. Check PG against VIN + 0.3 V and assess TI’s output-to-input protection if VOUT outlasts VIN. With the fixture attached, verify TP2 senses 3.3 V and TP6 stays high-impedance when board power is off. During a surge overlapping startup or shutdown, capture U5 VDD, divider and GPIO34 together against the [supply-dependent limits](../design/requirements.md#pcb-schematic). Check effective buck capacitance, hot inductor current and DA-15-to-XT30 fault drop and temperature. | Buck, switch and GPIO fault margins |
| Controller and interface checks | Measure 3.3 V at U1 during Wi-Fi bursts and verify EN timing on slow rise and rapid restart. Check ADC settling and noise with C18 ahead of U5. Qualify C1–C9, C18 and C20 for voltage, temperature and effective capacitance. Check both Becker pairs on the installed cable for common mode, reflections and idle level. | Local capacitance, ADC filtering and serial-interface acceptance |
| Test points and internal expansion (PCB08–PCB09) | Check the [optional J3 header](../design/integrated-pcb.md#expansion), mated height, wire restraint and lid on a first article. Check the exposed diameter of every pad in the production artwork, including the new TX pads. With DA-15, XT30 and USB plugged in and the lid off, contact each pad with the intended handheld probe while the ground lead is attached. Check tip stability and clearance to adjacent conductors. Hand-solder a representative extension lead, apply strain relief and check continuity, lid clearance and access to adjacent pads. Qualify peripheral load and boot with all links active. | Mated probe access, expansion load and first-article fit |
| Status LED visibility and drive | Verify power indication before firmware starts, heartbeat during operation and activity on valid traffic. Check side-slot sightlines with the lid fitted and the actual mounting orientation. Measure current and visibility of the green and red side-emitting LEDs. | Visible indications on the first article |
| Upright USB-C programming | Check the 90 Ω USB routing target on the quoted stackup, including the ESD fanout and the B-side D− crossover. Check both plug orientations for enumeration, flashing and repeated transfer after hot-plug. Check plug access and insertion support. Demonstrate automatic download and normal boot with the CP2102N. Check for contention or unintended powering at TX/RX, EN and GPIO0 with the pogo adapter connected and with USB absent. [Programming circuit](../design/integrated-pcb.md#usb-programming) | UART0, EN and BOOT layout |
| USB-C and XT30 power path | Characterize the accepted below-5 V risk of the [USB device with VS tied to VBUS](../design/integrated-pcb.md#usb-programming) with XT30 absent. Verify the VS supply range and U4 headroom at cold start, under peak load and when XT30 is unplugged. Measure USB inrush and running current to establish the charger rating. Bound PC-port draw before enumeration and in suspend. | Measure both input drops and hot reverse currents, including leakage into the XT30 sense divider. Check clamp plus ringing, ten-hour XT30 energy loss (PCB03), footprint and ten-board assembly cost. Run the [PCB12 source and programming checks](../design/requirements.md#production-pcb). |
| Bidirectional navigation RS-232 (PCB11) | Check the [DA-15 harness assignments](../design/integrated-pcb.md#smaller-dsub) through to each instrument RX contact. Exercise two distinct data streams both ways with Becker active and check reset and unpowered behavior. | Harness continuity and the [PCB11 first-article test](../design/requirements.md#requirements) |
| Signal layout review | With the signal routes in place, compare U2 charge-pump loops, U3 bypass and bus entry, and U5 bypass and signal paths with the manufacturers’ layout examples (TRS3232E Fig. 8-3, SN65HVD1470 Fig. 41, TMUX1574 Fig. 46). | Routed-board comparison before fabrication plots |
| Flight endurance and optional Bluetooth | Log input Wh and peaks during three Wi-Fi streams with Bluetooth off/on and fallback on Android. Agree acceptable draw using the battery allocation and actual duty cycle. | Measured endurance and transport acceptance |
| Aircraft-specific installation path | Responsible certifying staff to check equipment identity, permitted data connections, possible transmit commands, failure effects, documentation and release route | Installation decision |
| Ten-board landed quote | Designer to confirm H4 suffix, stock with attrition, panel rails, connector soldering, coating/finish and passives. Owner supplies delivery destination for delivered price. | Reproducible order or dated JLCPCB offer |


---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
