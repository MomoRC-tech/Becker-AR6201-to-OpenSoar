<a id="top"></a>

# 🎯 Requirements

🏠 [Overview](../../README.md) › 🧩 Design › **Requirements**

What the bridge must do, what the plan assumes, and what could go wrong.

> [!NOTE]
> **Status:** requirements agreed; none is verified on hardware yet. P0 = must have, P1 = should have.

📑 **On this page:** <kbd>[1 · ✅ Requirements](#requirements)</kbd> <kbd>[2 · 🤔 Assumptions](#assumptions)</kbd> <kbd>[3 · ⚠️ Risks](#risks)</kbd>

Related: 🏗️ [Architecture](architecture.md) · ❓ [Open items](../project/open-items.md) · 🗺️ [Roadmap](../project/roadmap.md)

---

<a id="requirements"></a>

## 1. ✅ Requirements

| ID | Prio | The bridge must… | How it is checked |
| --- | :-: | --- | --- |
| R01 | P0 | Put the selected airfield frequency into standby and never change the active frequency. Deferred pending [protocol evidence](../project/open-items.md#software-and-acceptance). | Readback from the real radio |
| R02 | P0 | Keep Zander and FLARM data reaching OpenSoar. | Compare with today's installation, then test both together with the radio path (WP6) |
| R03 | P0 | Pass radio bytes both ways exactly — no truncation, conversion, filtering or mixing with other data. | Byte-exact tests including zero/A5 bytes, split and bursty traffic, slow readers (WP5/WP6) |
| R04 | P0 | For future standby tuning, distinguish sending, radio acceptance and confirmed state, and report failures or unknown state. | Tests for valid, missing, late and malformed replies and disconnects |
| R05 | P0 | Leave the radio's own controls working when the bridge disconnects or loses power. | Test on the real radio |
| R06 | P0 | Match the electrical interface and serial settings of the installed equipment. | Matching manual, connector view and settings; no incompatible signal levels on ESP32 pins |
| R07 | P1 | Be reproducible and reuse existing work where possible. | Recorded versions and configuration; license check before reuse |
| R08 | P1 | Read radio state where supported and show what OpenSoar can display. | Driver and UI review (WP2/WP4) |
| R09 | P0 | Use a full-duplex RS-422 board for the Becker, with a safe signal level at the ESP32. | [Becker wiring](../hardware/wiring.md#becker) and bench check |
| R10 | P1 | Monitor the quality and stability of the Wi-Fi link to the phone from the ESP32 without interrupting bridge traffic. | In WP6, assess which metrics the firmware can observe (S55), how to report them and what counts as a useful reading. Verify on the bench with the phone during normal traffic and a deliberate disconnect and reconnect. |
| R11 | P0 | Restrain long internal wire runs against lifting, lateral motion and rubbing under vibration. Support branches near connectors, solder joints and the ferrite, with enough service slack to unload the terminations (S80). | Check the assembled harness with the actual wire and retainers for movement, pinch points, chafing and load at the terminations. Inspect and check continuity after a representative vibration test before aircraft integration ([housing item 20](../project/open-items.md#housing)) |
| R12 | P1 | Join the configured Wi-Fi network at start-up; if that fails, open its own password-protected hotspot ([ADR-05](architecture.md#adr-05)). | On the bench: start with the network available, then with it switched off; the bridge must be reachable in both cases |
| R13 | P1 | Accept firmware updates over Wi-Fi, so the housing stays closed. Every firmware image keeps this ability and the selected partition scheme ([ADR-05](architecture.md#adr-05)). | One OTA upload in each Wi-Fi mode of R12; repeat for every new firmware image |
| R14 | P1 | Show bridge, Wi-Fi and per-device status on a web page, without disturbing bridge traffic. | Open the page on the phone during the byte-exact tests of R03; the results must not change ([firmware plan](../project/roadmap.md#firmware-plan)) |
| R15 | P1 | Let the owner change serial, Wi-Fi, hotspot and port settings on a password-protected page without rebuilding the firmware. Stored settings survive restarts and OTA updates; a Wi-Fi, hotspot or port change that is not confirmed reverts by itself ([settings page](../project/roadmap.md#settings-page)). | Step 3a bench checks: change, restart, OTA update, unconfirmed change, wrong password, power loss during a save, reset |
| R16 | P1 | Show the data passing each device in readable form (text or hex, both directions), without disturbing bridge traffic. | Step 3b: looped data appears in the right port and direction; loop test passes with the monitor open ([data monitor](../project/roadmap.md#data-monitor)) |
| R17 | P1 | Show what the bridge does (start-up and restart reason, Wi-Fi, app connections, radio ownership, settings, updates, errors) on a web page, readable by kind and importance, exportable as text, and kept across a restart; never sent to a device port. | Step 3c: each kind of event appears with the right level and source; filters, Copy and Save work on the phone; the lines before a restart stay readable; loop test passes with the log open ([web log](../project/roadmap.md#web-log)) |
| R18 | P1 | Measure the bridge's 12 V supply and report it to OpenSoar and on the status page, with a chart of the minute values (mean, lowest, highest) over the time the bridge is on, up to 12 hours, without touching the forwarded device data ([ADR-07](architecture.md#adr-07)). | Step 3d: readings against a multimeter from 7 to 20 V; OpenSoar shows the value; the chart covers the time since start; loop test passes with the sensor port connected ([supply voltage](../project/roadmap.md#supply-voltage)) |

<a id="production-pcb"></a>

**Compact production PCB** — These requirements apply to the integrated design on this branch. The [design and order plan](integrated-pcb.md) owns component selections and the [open items](../project/open-items.md#pcb-study) track missing installation inputs.

| ID | The production bridge must… | Acceptance evidence |
| --- | --- | --- |
| PCB01 | Integrate the existing modules’ functions and their support components on one PCB of at most four copper layers. Use the selected shielded ESP32 module with its PCB antenna. A stacked daughterboard is acceptable if needed. Prefer components on one side and justify any second-side assembly against its extra cost. | Native schematic, fit study, layer count, assembly drawing and quote. |
| PCB02 | Fit a smaller version of the existing housing. Mount the right-angle two-row DA-15 and XT30 on the existing front panel. Retain the XT30 position and mounting geometry from the 48 × 46.5 mm baseline at `75f9453`. Preserve instrument assignments 1–9, add [Zander TX on 10 and FLARM TX on 11](integrated-pcb.md#smaller-dsub), and carry insertion and extraction loads through the housing. | Compare outer and mated envelopes with the original. Check plug access, retention, PCB strain, antenna and service clearance on assembled hardware. |
| PCB03 | Minimize energy drawn from the glider battery on long flights. Present the cost and energy trade-off for significant choices. | Log voltage and input current at the XT30 throughout representative flight traffic. Compare Wh and peak current over the same duty cycle and 10-hour reference mission. A numerical endurance target has yet to be set. |
| PCB04 | Use the ESP32-WROOM-32E-H4 with its PCB antenna on the 3.3 V rail. Give Becker, Zander and FLARM one hardware UART each and support all three simultaneously over Wi-Fi. Bluetooth Classic Serial Port Profile (SPP) support is required for an additional or fallback Android connection. | Check exact module availability. Exercise three independent Wi-Fi streams, add an SPP client, then test SPP fallback with the target Android flight app. Check bytes, reconnects and input energy in each mode. |
| PCB05 | Select the components, PCB material and assembly process for the expected covered outdoor winter storage, possible condensation and hot summer cockpit. Manage heat inside the smaller enclosure. | Source exact part limits, measure site conditions, and verify cold start, hot closed-box operation and post-condensation performance. Record operating, storage, soldering and housing temperature limits separately. |
| PCB06 | Design for a production run of ten. Show expensive parts, unit and lot costs, and cheaper technically credible alternatives. Supply a JLCPCB quote or a checked fabrication and assembly package the owner can order. | Costed BOM with exact MPNs and stock, assembly options and fees, fabrication plots and drills, BOM/CPL reconciliation, preview and first-article test instructions. |
| PCB07 | Keep aircraft-installation rules and effects on connected instruments in view. Collect the evidence for design decisions and distinguish calculations, supplier specifications and physical tests. | [Source register](../research/sources.md) and aircraft-specific review with the responsible certifying staff. |
| PCB08 | Provide labeled top-side test pads for a handheld probe and hand-soldered extension wires. Use Ø2.5 mm of exposed metal as the production layout target, including ground and shared fixture pads. Cover the raw and protected supply, 3.3 V with a nearby ground, power-good, battery-sense output, reset, boot and serial logic. Existing fixture pads may serve both purposes. | Supply a pad-to-net map and a probe-access drawing. With both harness plugs mated and the lid removed, probe the component side to demonstrate rail and startup measurements, programming and a loopback through each instrument port on a first article. |
| PCB09 | Provide a normally empty 2.54 mm J3 expansion row for a plug-in daughterboard or directly soldered wires. Expose GND, 3.3 V, SDA and SCL. | Publish the [J3 pinout](integrated-pcb.md#expansion), usable modes, voltage and available current. Verify plug clearance, wire restraint and antenna clearance. Exercise a representative peripheral while all three instrument links are active and confirm normal boot with the expansion absent or fitted. |
| PCB10 | Use fabrication margins above the selected process limits for ordinary traces, gaps, vias, holes, board edges and markings. Size the power paths and all three instrument returns to XT30 minus for their load, duration, copper tolerance and temperature. Check the 3.2 A instrument ground-fault case. | Save enforceable KiCad rules, inspect refilled copper and production artwork, document supplier land-pattern exceptions and verify critical paths on a first article. |
| PCB11 | Provide independent bidirectional RS-232 ports for the Zander ZS1 and Classic FLARM. Each port must receive from its instrument and transmit to that same instrument through the production harness. | On the first article, use two RS-232 bench peers at the harness interface. Exchange distinct byte patterns in both directions on both ports while the Becker link is active. Verify byte-for-byte delivery to the intended port only. |
| PCB12 | Start and run from an ordinary 5 V USB supply of sufficient current with XT30 disconnected, without USB data or Power Delivery negotiation. Run from XT30 alone at 7–20 V and give XT30 priority when both are connected. Support USB programming with XT30 powering the board. Minimize forward loss in both source diodes. Allow either connection or removal order, block backfeed at both inputs and measure only XT30 for battery telemetry. | Characterize the accepted USB-only startup risk below 5 V at the receptacle. Establish the minimum supply rating under startup and running load. Start from a standalone charger and from XT30 separately. Flash from a PC with XT30 connected. Test both insertion and removal orders, scope 3.3 V, measure reverse current at both inputs and verify telemetry with XT30 absent and present. |
| PCB13 | Provide green and red side-emitting status LEDs. Green LED 1 indicates the 3.3 V rail before firmware starts and adds a heartbeat while firmware runs. Red LED 2 indicates valid instrument traffic. Place both near an edge for viewing through the ventilation slots where feasible. | Observe LED 1 during reset and normal operation. Feed valid and invalid data to check LED 2. Check both viewing positions with the lid fitted and the board in its mounting orientation. |

**Not planned:** active tuning, frequency swap, volume and squelch, Larus, general XCSoar support. Aircraft installation comes later. The repository license is still to be chosen.

<a id="pcb-schematic"></a>

### 1.1 Integrated PCB supplier circuit requirements

Use these limits to check the [KiCad schematic](../../electronics/integrated/compact-bridge.kicad_sch). Example component values are identified separately from operating limits. [Source register S181–S185](../research/sources.md#source-register)

| Circuit | Supplier guidance | Comparison with the saved schematic |
| --- | --- | --- |
| U1 ESP32 supply and boot | Supply 3.0–3.6 V with at least 0.5 A available. Figure 9 shows 22 µF + 100 nF locally. Allow ≥50 µs from stable supply to EN high and ≥50 µs low for reset. Keep GPIO12 low at boot. Download mode needs GPIO0 and GPIO2 low. | C2/C3 are **10 µF + 100 nF**. R1/C1 match the suggested 10 kΩ/1 µF EN delay. TP3–TP6 expose EN, BOOT and UART0. R2 pulls GPIO0 high for normal boot. GPIO2/12 retain internal pulldowns. The H4 has no PSRAM, so GPIO16 is available. |
| U2 TRS3232E RS-232 | At 3.3 V ±0.3 V use four 100 nF charge-pump capacitors and bypass VCC with 100 nF. Tie unused driver inputs to a defined logic level. | C4/C5 span their respective pump pins, C6/C7 connect the positive and negative pump rails to ground, and C8 bypasses VCC. GPIO33 drives U2.11 for Zander. GPIO27 drives U2.10 for FLARM. Both outputs reach J1. Check the actual capacitor voltage ratings and effective capacitance. |
| U3 SN65HVD1470 full duplex | Supply both VCC pins at 3.0–3.6 V, ground both GND pins, bypass locally with 100 nF and define both enables. Keep bus common mode within −7 to +12 V. Choose termination for the cable and load. | C9 bypasses VCC. Receiver enabled, driver enabled through R4, data input pulled high. Y/Z drive J1-8/9 and A/B receive from J1-4/5. No external termination or bus TVS is fitted. |
| U4 LMR33610 buck | Use TI’s 3.3 V/400 kHz design as the reference. Check effective input/output capacitance, hot inductor current and loss. Operate at VIN 3.8–36 V. Keep EN and PG within VIN + 0.3 V during collapse. Assess reverse current if output charge can hold VOUT above VIN. | The front-panel design uses the 100 kΩ/43.2 kΩ divider in TI Table 9-2. L1 at 6.8 µH and four 22 µF outputs follow Table 9-3. It uses two 10 µF input capacitors, a 220 nF input bypass, a 100 nF BOOT-to-SW capacitor, a 1 µF VCC bypass, a higher-resistance L1 and a 10 kΩ PG pull-up. S220. |
| U5 TMUX1574 and U1 GPIO34 ADC | Bypass VDD with 100 nF. For switch supply below 1.5 V keep signal pins at 0–3.6 V. At ≥1.5 V the upper limit is the lower of twice VDD and 5.5 V. Espressif recommends a 100 nF filter at the ADC pin. | C20 bypasses U5. With U5 powered and enabled, PG low selects ground and PG high selects the divider. GPIO34 is on ADC1. C18 is on the **divider side** of U5. Check ADC settling after switching and GPIO34 voltage as 3.3 V falls. |
| Q1, D1 and D2 input protection | Keep Q1 within ±20 V gate-to-source. Use D1’s voltage at its actual current and D2’s clamp at the applicable pulse current and duration. Coordinate the external fuse with the input fault envelope. | Q1 drain faces the raw input and its source feeds the protected rail. D1 cathode goes to source and anode to gate. D2 cathode goes to the protected rail and anode to ground. These polarities agree with the intended paths. |

---

<a id="assumptions"></a>

## 2. 🤔 Assumptions

Things the plan relies on but that are not yet confirmed.

| ID | Assumption | Confirmed by | What it blocks |
| --- | --- | --- | --- |
| A01 | The installed radio is an AR6201 variant that supports remote control. | Radio identity (WP1), matching manual (WP2) | Final wiring |
| A03 | The installed OpenSoar build supports the Becker driver over TCP alongside the navigation feeds. | Installed build (WP1), integration review (WP4) | End-to-end software choice |
| A04 | Radio status gives a fresh enough active frequency before a standby command. | Protocol evidence (WP2), state design (WP4) | Safe command design |
| A05 | A separate radio TCP endpoint works without disturbing the navigation feeds. | Topology (WP1), coexistence tests (WP6) | Radio path acceptance ([ADR-01](architecture.md#adr-01)) |
| A06 | The ESP32 can carry both navigation paths plus the radio path. | UART plan (done), coexistence tests (WP6) | Bridge acceptance |

---

<a id="risks"></a>

## 3. ⚠️ Risks

All risks are open.

| ID | Prio | Risk | How it is reduced |
| --- | :-: | --- | --- |
| K01 | P0 | The wrong manual or radio variant leads to incompatible wiring — Becker publishes separate manuals per hardware generation (S03). | Match model, revision and connector view before wiring |
| K02 | P0 | A standby command carries an unknown or stale active frequency (F04). | Block tuning in the [readback-only patch](../research/becker-driver.md#consequences) |
| K03 | P0 | Binary writes are truncated or report false success (F02/F03). | Verify zero writes from tuning calls in the readback-only patch (S164) |
| K04 | P0 | Bridge overflow, several writers or debug output corrupt radio traffic (S05/S06). | Firmware audit, byte-exact load and failure tests, separate radio path |
| K05 | P0 | The new bridge interrupts navigation data. | Compare with today's installation; coexistence tests (WP6) |
| K06 | P0 | A simulator only repeats the driver's own assumptions. | Independent protocol evidence; label synthetic tests; simulation is never acceptance |
| K07 | P1 | Licenses or dependencies prevent reuse. | Check license files and build dependencies; no code copied without permission |
| K08 | P1 | Switching noise from the UBEC or ESP32 is heard in the VHF radio or opens its squelch. | Bench and aircraft check with the bridge on and off ([bench bring-up](../hardware/wiring.md#bring-up)) |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
