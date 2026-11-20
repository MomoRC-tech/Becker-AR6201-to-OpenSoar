<a id="top"></a>

# ❓ Open items

🏠 [Overview](../../README.md) › 📋 Project › **Open items**

The one list of everything still missing: what is needed, who provides it, and what it unblocks. Other pages link here instead of repeating open questions.

> [!NOTE]
> **Status:** all listed items are open. Current focus: bench hardware. Housing checks carry forward to prototype assembly and installation.

📑 **On this page:** <kbd>[1 · 🔧 Bench hardware — current focus](#bench-hardware)</kbd> <kbd>[2 · 📦 Housing fit and installation](#housing)</kbd> <kbd>[3 · 🛩️ Installed equipment](#installed-equipment)</kbd> <kbd>[4 · 💻 Software and acceptance — later](#software-and-acceptance)</kbd>

Related: 🗺️ [Roadmap](roadmap.md) · 🤔 [Assumptions](../design/requirements.md#assumptions) · 🛩️ [Existing installation](existing-installation.md)

---

<a id="bench-hardware"></a>

## 1. 🔧 Bench hardware — current focus

Start with the delivered boards and power supply, with all instruments disconnected. See [bench bring-up](../hardware/wiring.md#bring-up) for the order; installation checks are identified separately there.

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | MAX490 board | Current draw | Bench check on the delivered board |
| 3 | RS-232 replacement modules | IC marking, continuity against the [pad map](../hardware/modules.md#rs232-module) and R1 as the TTL output | [Loose-board check](../hardware/wiring.md#bring-up) before fitting a replacement |
| 4 | ESP32 board | Module marking and header positions match the DevKitC V4 reference | Visual check |
| 5 | UBEC | 5 V output with and without load; match the physical input/output polarity and XT30 cups to the labelled routing endpoints; fit in the enclosure | Bench measurement |
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

The bridge firmware follows its [plan](roadmap.md#firmware-plan) in parallel with the bench build. Driver work is on hold until the hardware is finished.

| # | Item | What is needed | Unblocks |
| :-: | --- | --- | --- |
| 1 | Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, channel vectors ([details](../research/becker-driver.md#protocol-details)) | Driver plan; independent test vectors |
| 2 | Radio configuration | Matching manual; serial settings (the bridge uses 9600 8N1 until confirmed, [check](../research/reviews/2026-11-17-bridge-projects-check.md#other-projects)); remote control and Tandem mode enabled, as LXNAV and SD-Link require; this changes how the radio behaves, so the owner or installer decides before it is switched on | Port configuration |
| 3 | Application integration | AR62xx driver over TCP alongside navigation; status display | Driver plan |
| 5 | Network settings | Network to join in the aircraft and the passwords of the new bridge; hotspot name ([ADR-05](../design/architecture.md#adr-05)), address and ports ([firmware plan](roadmap.md#firmware-plan)) are selected | App and bridge configuration |
| 6 | Driver regression | Zero/A5 bytes, short frames, bad CRC, unknown start state, two instances, missing or late replies | Software verification (M4) |
| 7 | Bridge reliability | Byte-exact tests both ways, bursts, slow clients, partial writes, reconnects, navigation in parallel | Software verification (M4) |
| 8 | Physical acceptance | Frequency change and readback on the real radio, local controls and navigation unaffected | Aircraft integration |
| 9 | Firmware review checks | Cross-port delay during bulk writes at low baud and remaining connection capacity. Repeated clean closes on quiet Zander and the ordinary two-app connection load passed (S160). [Firmware review](../research/reviews/2026-11-19-firmware-0.5.4-review.md#missing-checks) | Severity of findings 1 and 4 |
| 10 | mDNS memory error | One `mdns_networking: Cannot allocate memory` error while the radio port refused repeated clients (S150): cause, and whether it repeats or affects the ports | Bridge reliability |
---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
