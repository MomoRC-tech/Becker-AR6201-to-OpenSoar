<a id="top"></a>

# ❓ Open items

🏠 [Overview](../../README.md) › 📋 Project › **Open items**

The one list of everything still missing: what is needed, who provides it, and what it unblocks. Other pages link here instead of repeating open questions.

> [!NOTE]
> **Status:** all listed items are open. Bench validation is complete; the items concern installation, installed equipment and software acceptance.

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
| 9 | OpenSoar | Installed build, driver and endpoint settings, role of the OpenVario | Owner / Tom | App configuration |
| 10 | Radio ground | Whether both P1 ground pins (13, 25) run to the battery minus, and whether the radio's grounding bolt is connected | Tom / owner | Confirms the radio cannot return its current through the bridge ([ground](../hardware/power.md#ground)) |

---

<a id="software-and-acceptance"></a>

## 4. 💻 Software and acceptance — later

See the [bridge firmware plan](roadmap.md#firmware-plan) and the [stock OpenSoar driver setup](../research/becker-driver.md#consequences).

| # | Item | What is needed | Unblocks |
| :-: | --- | --- | --- |
| 1 | Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, channel vectors ([details](../research/becker-driver.md#protocol-details)) | Standby tuning |
| 2 | Radio configuration | Matching manual; serial settings (the bridge uses 9600 8N1 until confirmed, [check](../research/reviews/2026-11-17-bridge-projects-check.md#other-projects)); remote control and Tandem mode enabled, as LXNAV and SD-Link require; this changes how the radio behaves, so the owner or installer decides before it is switched on | Port configuration |
| 3 | Application integration | Record the installed OpenSoar version and check its built-in AR62xx driver over TCP alongside navigation | Application acceptance |
| 4 | Network settings | Network to join in the aircraft and the passwords of the new bridge; hotspot name ([ADR-05](../design/architecture.md#adr-05)), address and ports ([firmware plan](roadmap.md#firmware-plan)) are selected | App and bridge configuration |
| 5 | Bridge reliability | Byte-exact tests both ways, bursts, slow clients, partial writes, reconnects, navigation in parallel | Software verification (M4) |
| 6 | Physical acceptance | Check local controls and navigation on the matching bench setup. For standby tuning, verify the requested standby and unchanged active frequency on the radio | Radio acceptance |
| 7 | Firmware review checks | Cross-port delay during bulk writes at low baud and remaining connection capacity. Repeated clean closes on quiet Zander and the ordinary two-app connection load passed (S160). [Firmware review](../research/reviews/2026-11-19-firmware-0.5.4-review.md#missing-checks) | Severity of findings 1 and 4 |
| 8 | Heap dips | Wi-Fi packet buffers take the free heap to near 0 for seconds at a time, at 80 MHz in about half of the idle seconds on the home network; the `mdns_networking: Cannot allocate memory` errors (S150, S197) are the same dips. Much less often at 160 MHz, selected for 0.5.19 (S199); chip about 1 °C warmer under load (S201). A clean one-hour run at 160 MHz without other load on the bench PC | Bridge reliability |
| 9 | Settings under power loss | Interrupt a save and an automatic rollback on a spare ESP32, then check that the stored configuration is complete (S162) | Physical settings persistence |
| 11 | Radio owner change | Bytes the previous owner sent can still be on the UART when the next owner is accepted, so their echo or the radio's reply may reach the next owner. Measured with `tools/radio_robustness_test.py`: 113 and 119 bytes reached the next owner (S177). Owner decides how to handle it | Radio ownership |
| 12 | Remaining review checks | Code review findings 6–9: UART FIFO margin during flash writes, ROM output on the Zander transmit pin, apps held during a navigation scan, low items | Bridge reliability |
| 13 | Phone reconnect and two-phone checks | Stock OpenSoar on a real Android phone: hotspot Wi-Fi loss and bridge power cycle with mobile data on, network-mode power cycle with phone Wi-Fi left on, and its AR62xx driver on the radio port (item 3). With a second phone, check that the first keeps receiving while the second app is in the background or its screen is off (S190) | Step 5 acceptance |

### 4.1 Phone reconnect options (item 13)

| Case | Recommended change | Other options |
| --- | --- | --- |
| Hotspot with mobile data on | The bridge-side fix for power cuts also covers this case: after an automatic rejoin with mobile data on, connections from the hotspot to the app's *TCP port* reached the phone while the app's own clients stayed dead (S206). Upstream alternative: propose an Android option to bind local TCP device sockets to the selected Wi-Fi. Reopen them when Android reports the Wi-Fi network has returned. See S192, S193 and S196 | [Phone workaround](../manual/user-manual.md#troubleshooting). Hotspot DHCP without router does not help: after a Wi-Fi cut Android again sends app traffic over mobile data (S198). A shared Wi-Fi network with real internet access can avoid the default-network problem, with a router and uplink if they are unavailable. The bridge's standalone hotspot has no internet uplink |
| Power cut in network mode | **Owner selection:** the bridge-side fix, the bridge dials OpenSoar's *TCP port* (S200). It fits OpenSoar's code and documented use (S204): replace on accept is implicit but stable since 2020; after a receive error OpenSoar rebuilds the listener within 30–60 s, so the bridge keeps retrying. Owner phone check passed on shared Wi-Fi and on the hotspot with mobile data on (S206). Firmware plan: [step 4a](roadmap.md#firmware-plan). Upstream alternative: TCP client keepalive and use the existing retry path. As a starting point on Android, try 5 s idle and three probes 3 s apart (about 14 s to detect an idle connection whose probes all go unanswered, plus retry time). See S193–S195 | Planned restarts are handled: the bridge closes all app connections first (0.5.18, S198); only power cuts remain. Phone: Wi-Fi off and on reopens all ports (S197, emulator). Bridge-side alternative with the stock app: the bridge dials OpenSoar's *TCP port* listener, which replaces a stale session at once (S200); needs the phone's address, an outgoing connection per port, one phone per port and the Becker owner rule, so a firmware mode of its own; might also help with mobile data on, since the phone answers incoming connections on Wi-Fi (untested). A silence timeout can work for a feed with a known cadence. Quiet FLARM and Becker ports need transport-level detection. See [manual recovery](../manual/user-manual.md#troubleshooting) |

Prefer upstream issues and shared TCP transport changes for both apps. Get owner approval before posting. The app changes require development and phone test time, with no additional hardware or paid service required. The phone workarounds require no purchase. The power-cut fix uses TCP probes and adds no bytes to the instruments' serial streams.

**Acceptance:** stock OpenSoar shows fresh data again by itself after a connection drop; a new socket on the bridge is only an intermediate result. Feed a changing synthetic Zander or FLARM sentence (for example GPRMC with the current time), keep a quiet port and the stock Becker entry configured, never press *Reconnect*. For each case record separately:

| Measured | From |
| --- | --- |
| Time to a new bridge socket per port | `/api/status` `tcp_clients` every 2 s, `/api/log` |
| Time to the first fresh sentence in OpenSoar | Device list (*GPS fix* / *No data*) or the clock InfoBox, timed screenshots |

- [x] Emulator pre-check (bench PC, no owner): web restart, firmware update and settings rollback with 0.5.19+, network mode: fresh data 6–9 s after the restart (S201). S179 and S182 cover the emulator's NAT limits; a power cut cannot be simulated there
- [ ] Owner phone on the shared Wi-Fi (network mode): web restart, then a hard bridge power cut with the phone's Wi-Fi left on
- [ ] Owner phone on the hotspot with mobile data on: Wi-Fi cut and bridge power cycle
- [x] Owner phone, no firmware change (S206, XCSoar): the bench PC connects to OpenSoar's *TCP port* device on the phone twice (old connection silent, new one sending), on the shared Wi-Fi and on the hotspot with mobile data on (S200 on a real phone): fresh data ≤ 6 s after the new connection, also after an automatic hotspot rejoin with mobile data on
- [ ] For an app change: a quiet healthy port stays connected, and other apps keep internet access with mobile data on

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
