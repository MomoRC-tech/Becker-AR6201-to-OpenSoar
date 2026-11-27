<a id="top"></a>

# ❓ Open items

🏠 [Overview](../../README.md) › 📋 Project › **Open items**

The one list of everything still missing: what is needed, who provides it, and what it unblocks. Other pages link here instead of repeating open questions.

> [!NOTE]
> **Status:** WP3 bench bring-up is complete. The open items cover installation, equipment identification and software acceptance.

<a href="#bench-hardware"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-project-open-items-1-dark.svg"><img src="../images/nav/menu-project-open-items-1.svg" alt="1 · Wiring and parts for installation" height="30"></picture></a> <a href="#housing"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-project-open-items-2-dark.svg"><img src="../images/nav/menu-project-open-items-2.svg" alt="2 · Housing fit and installation" height="30"></picture></a> <a href="#installed-equipment"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-project-open-items-3-dark.svg"><img src="../images/nav/menu-project-open-items-3.svg" alt="3 · Installed equipment" height="30"></picture></a> <a href="#software-and-acceptance"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-project-open-items-4-dark.svg"><img src="../images/nav/menu-project-open-items-4.svg" alt="4 · Software and acceptance — later" height="30"></picture></a> <!-- page-menu -->

Related: 🗺️ [Roadmap](roadmap.md) · 🤔 [Assumptions](../design/requirements.md#assumptions) · 🛩️ [Existing installation](existing-installation.md)

---

<a id="bench-hardware"></a>

## 1. 🔧 Wiring and parts for installation

Bench bring-up is complete. These items remain before the bridge is connected in the aircraft; the [installation checks](../hardware/wiring.md#bring-up) are steps 14–16 there.

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | Fuse | Select fuse rating and speed from the running current (60–90 mA at 12 V, S147) and the wire protection requirements; switch-on inrush is not measured | Owner selection |
| 2 | F1 holder | Inline holder in the 12 V harness at its supply end is selected; holder part and fuse format ([protection](../hardware/power.md#protection)) | Owner selection with item 1 |
| 3 | Supply-cable protection | Owner confirmation of cable length, routing and wire specification; where F1 sits at the supply end ([protection](../hardware/power.md#protection)) | Owner supplies harness details; agree cable protection before aircraft connection |
| 4 | Power connector fit and retention | Adhesive compatibility with the XT30 body and PETG, wire support and insulated terminations ([selected arrangement](../hardware/housing.md#connectors)) | Connector-panel print and assembly check |
| 5 | Combined DE-9 instrument connection | Exact mating parts, wire-gauge compatibility, screw threads, backshell, strain relief and housing mounting. Identify the loose conductors and verify shield termination against the [selected pinout](../hardware/wiring.md#de9-pinout) and [Becker wiring](../hardware/wiring.md#becker). | Owner selects parts and mounting details. Check fit and continuity before connection. |
| 6 | Device connectors | Photos of the installed connectors (from Tom); device models are [installed-equipment items 1–3](#installed-equipment) | Confirms the pin views in the wiring tables |
| 7 | Becker J1 | Installed J1 revision, moulded pin numbers and which pins are already used | Confirms the J1 pins and the J1-21 ground |
| 8 | Radio noise | No audible noise or squelch opening caused by the bridge | Check in the aircraft |

---

<a id="housing"></a>

## 2. 📦 Housing fit and installation

Checks on the [housing](../hardware/housing.md) bench prototype as built, and on its eventual placement in the aircraft.

### 2.1 Installation and environment

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | Location in the aircraft | Which shaded spot the bridge sits in, maximum envelope (L × W × H) at that spot, orientation, and which structure the cable ties through the four base tie loops go around | Owner / Tom; photo and measurement of the spot |
| 2 | Cable exits | Direction from which the 12 V harness and the instrument cables arrive at the connector side; free space in front of it for the mating plugs, DE-9 backshell and cable bend radius | Owner, from item 1 |
| 3 | [Winter moisture and summer heat](../hardware/housing.md#climate) | Coldest intended start, wetting after covered storage, hottest parked mounting spot and powered temperature rise | Owner / bench builder. Use the checks below before deciding on a different hood. |
| 4 | Distance to compass and radio antenna | Minimum distance from the magnetic compass (UBEC coil, ferrite ring, steel screws and inserts) and from the VHF antenna cable | Owner / Tom; compass check in the aircraft |
| 5 | Wi-Fi path | Antenna position relative to the phone and surrounding structure | Owner selection; check in the aircraft |

**Item 3 — proposed climate checks**

- [ ] At the proposed spot, log air temperature and relative humidity through damp nights, thaw and a sunny closed-canopy stop. Record cover and canopy position. With the bridge unpowered, compare a board or connector surface temperature with the local dew point. Look for droplets, tide marks and corrosion inside the hood and at both plugs. A dry afternoon inspection can miss overnight dew.
- [ ] Read both RS-232 chip markings and the MAX490 suffix. Check operating and storage limits for the delivered boards, UBEC and actual filament. Record any missing limit and the margin available at the coldest intended start and hottest operating condition.
- [ ] On the bench, reproduce the hot mounting-site air temperature and orientation after an unpowered soak. Run the fully wired bridge on its normal supply with sustained Wi-Fi and representative serial traffic. Log input volts and amps, air beside the box and RS-232 boards, surface temperatures at the ESP32 regulator, UBEC and loaded PETG clamp, and the bridge's [chip temperature](../../firmware/bridge/README.md#chip-temperature), until stable. Check for resets, serial errors and loss of retention after cooling.
- [ ] Fit the roof-grille hood on the in-use base with the original four M3 × 6 screws. Check seating and side-wall flex during removal. Compare with the original hood at the same orientation, ambient temperature and steady electrical load. Record the temperature rise above ambient with each hood.
- [ ] If considering a solid hood, repeat with the slots temporarily blanked on the dry bench, at the same ambient, load and clearances. Compare each temperature rise above local ambient. Stop before an applicable limit. Advance the preferred configuration only with dry boards and connectors, temperature margin and retained fit. Resolve any water path, unknown critical rating or loss of margin before installation.

### 2.2 Construction

Fit and assembly of the bench build passed (S156).

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | Internal harness retention | Check the [floor retainers](../hardware/housing.md#floor-guide), the [ferrite cradle](../hardware/housing.md#ferrite-cradle) and support at the DE-9 breakout, signal runs and board branches against [R11](../design/requirements.md#requirements): movement, chafing and continuity after representative vibration, and cradle retention at the expected loaded temperature | Physical vibration and heat checks |
| 2 | Ground path rating | Rate the full ground path through the [GND-J splice](../hardware/housing.md#internal-routing) for the documented fault currents | Owner check against the [power page](../hardware/power.md) |

---

<a id="installed-equipment"></a>

## 3. 🛩️ Installed equipment

Facts about the equipment already in the aircraft, mostly from Tom. What is known so far: [existing installation](existing-installation.md). Closing the interface items is milestone M1.

| # | Item | What is needed | From | Unblocks |
| :-: | --- | --- | --- | --- |
| 1 | Radio | Type plate, article number, hardware/software versions of the reported AR6201-(022); J1 occupancy is [wiring item 7](#bench-hardware) | Tom | Matching manual and interface |
| 2 | Zander ZS1 | Program version, baud and output settings, cable | Tom | Serial settings |
| 3 | FLARM | Model and firmware (reported Classic, IGC series 6), socket used, baud and output settings | Tom | Serial settings |
| 4 | Existing wiring | How each device signal reaches the existing ESP32, including any converters | Tom | Backup record |
| 5 | Existing converter | Chip, supply and logic level of the RS422 TO TTL board | Tom | Backup record |
| 6 | Existing ESP32 | Board, firmware, configuration | Tom | Backup record |
| 7 | Existing power | UBEC jumper/output, board input, grounding | Tom / owner | Backup record |
| 8 | Navigation baseline | Data types, update gaps, reconnect behaviour of today's setup | Owner / Tom | Acceptance limits for the new bridge |
| 10 | Radio ground | Whether both P1 ground pins (13, 25) run to the battery minus, and whether the radio's grounding bolt is connected | Tom / owner | Confirms the radio cannot return its current through the bridge ([ground](../hardware/power.md#ground)) |

---

<a id="software-and-acceptance"></a>

## 4. 💻 Software and acceptance — later

See the [bridge firmware plan](roadmap.md#firmware-plan) and the [stock OpenSoar driver setup](../research/becker-driver.md#consequences).

| # | Item | What is needed | Unblocks |
| :-: | --- | --- | --- |
| 1 | Protocol | Compare the [public frame and channel mapping](../research/becker-driver.md#protocol-details) with a two-direction capture on the matching radio. Establish status timing at start, on reconnect and after changes, and distinguish echo, rejection and applied state. Confirm whether a standby-only command synchronizes the built-in head. If a paired write is required, establish how the radio handles a concurrent local change. [Issue #4](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/4) | Standby tuning |
| 2 | Radio configuration | Match the manual to the installed hardware and CM/CH versions, identify occupied J1 pins and verify the serial settings on the [selected interface 2](../hardware/wiring.md#becker). The bridge uses 9600 8N1 as a bench starting point. Record the Tandem setting and have the owner or installer decide before changing it. [Adapter evidence](../research/becker-driver.md#manuals) | Port configuration |
| 3 | Application integration | Check the stock OpenSoar AR62xx driver over TCP alongside navigation | Application acceptance |
| 4 | Network settings | Network to join in the aircraft and the passwords of the new bridge; hotspot name ([ADR-05](../design/architecture.md#adr-05)), address and ports ([firmware plan](roadmap.md#firmware-plan)) are selected | App and bridge configuration |
| 5 | Bridge reliability | Byte-exact tests both ways, bursts, slow clients, partial writes, reconnects, navigation in parallel | Software verification (M4) |
| 6 | Physical acceptance | Check local controls and navigation on the matching bench setup. For standby tuning, compare app result, captured bytes and both front-panel channels at startup, after reconnect and around a local tune or swap. Verify the requested standby and preservation of the active channel set by the pilot. | Radio acceptance |
| 8 | Sustained load and heap | Run an hour at 160 MHz with independent Zander and FLARM feeds, Becker loop and supply telemetry. Record lost bytes, worst delay, UART errors, restarts and lowest heap in hotspot and bench network mode. Earlier heap observations: S199, S211 and S220. S188 exercised Zander, Becker and voltage | Bridge reliability |
| 9 | Settings under power loss | Interrupt a save and an automatic rollback on a spare ESP32, then check that the stored configuration is complete (S162) | Physical settings persistence |
| 11 | Becker owner handover | The 0.5.29 DE-9 loop passed with zero stale bytes (S234). Check delayed replies with the matching radio | Radio ownership |
| 12 | Remaining review checks | Code review findings 6–9: UART FIFO margin during flash writes, ROM output on the Zander transmit pin, apps held during a navigation scan, low items | Bridge reliability |
| 13 | Phone reconnect | Normal build, one phone running stock OpenSoar with hotspot mobile data on. Network mode uses the bench PC. The second-phone background check is follow-up coverage | Step 5 acceptance |

The six-connection limit is deferred for the one-phone workflow. Revisit it if normal reconnects lose data or two-phone operation becomes important. The general TCP-to-UART write refactor is deferred in [issue #3](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/3).

### 4.1 One-phone acceptance (item 13)

Use the [normal build](../../firmware/bridge/README.md#build), one [TCP port entry per device](../manual/user-manual.md#connect-opensoar), and a changing synthetic navigation sentence. Disconnect the PC's test clients and leave mobile data on. Never press *Reconnect* during a case.

| Case | Evidence to record |
| --- | --- |
| Restart OpenSoar, then switch phone Wi-Fi off and back on | Time to a new socket per port and to the first fresh sentence in OpenSoar |
| Web restart, then hard power cut of the bench bridge | Start timing at restart or power restoration. Note whether mobile internet still works |
| Locked phone during a restart or cut | Whether it rejoined and received data before unlock, and when data resumed after unlock |
| Stock AR62xx entry | Verify the connection with the Becker loop. Gate actual tuning on item 1 and matching-radio checks in items 2 and 6 |
| Bench PC in network mode | Verify automatic recovery and changing data with the PC as the app listener (S219) |
| Second phone, follow-up | With the main phone already receiving, observe the first phone while the second app is backgrounded or its screen is locked |

Use [bridge status and log counters](../../firmware/bridge/README.md#status-page) for socket times. Use the app's device list or a changing clock display for fresh data. Record the app and firmware versions and which ports had a feed. Earlier Pixel tests showed hotspot recovery with synthetic Zander data and sometimes waited for unlock (S215, S220). Reset recovery was measured on the bench (S223). The subsequent slot-reuse fix was checked on the host (S224). On normal 0.5.29, the owner confirmed fresh Zander GPS after a Wi-Fi toggle, app restart and hard power cut. The web restart recovered all three instrument sockets (S233). During a web restart with the phone locked, all three sockets recovered after unlock, and the owner reported GPS recovery (S234).

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
