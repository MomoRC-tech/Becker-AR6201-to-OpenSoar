<a id="top"></a>

# ❓ Open items

🏠 [Overview](../../README.md) › 📋 Project › **Open items**

The one list of everything still missing: what is needed, who provides it, and what it unblocks. Other pages link here instead of repeating open questions.

> [!NOTE]
> **Status:** all items open. Work top-down — the bench hardware checks are the current focus.

📑 **On this page:** <kbd>[1 · 🔧 Bench hardware — current focus](#bench-hardware)</kbd> <kbd>[2 · 📦 Housing — next](#housing)</kbd> <kbd>[3 · 🛩️ Installed equipment](#installed-equipment)</kbd> <kbd>[4 · 💻 Software and acceptance — later](#software-and-acceptance)</kbd>

Related: 🗺️ [Roadmap](roadmap.md) · 🤔 [Assumptions](../design/requirements.md#assumptions) · 🛩️ [Existing installation](existing-installation.md)

---

<a id="bench-hardware"></a>

## 1. 🔧 Bench hardware — current focus

Checks on the delivered parts before the bridge is connected to any device. Step-by-step order: [bench bring-up](../hardware/wiring.md#bring-up).

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | MAX490 board | Which header pin is RO and which is DI; termination; current draw | Bench check on the delivered board |
| 2 | GPIO16 input | High and low level at GPIO16 behind the divider, also at power-up and power-down | Bench measurement |
| 3 | RS-232 modules | Continuity of both modules against the inferred pad map; IC marking | Bench check |
| 4 | ESP32 board | Module marking and header positions match the DevKitC V4 reference | Visual check |
| 5 | UBEC | 5 V output with and without load; fit in the enclosure | Bench measurement |
| 6 | Current and fuse | Running current and switch-on inrush at 12 V; select fuse rating and speed using these measurements and the wire protection requirements | Bench measurement and owner selection |
| 8 | Device connectors | Photos of the installed connectors and exact device models (from Tom) | Confirms the pin views in the wiring tables |
| 9 | Becker J1 | Installed J1 revision, moulded pin numbers and which pins are already used | Confirms the J1 pins and the J1-21 ground |
| 10 | Radio noise | No audible noise or squelch opening caused by the bridge | Check in the aircraft |
| 11 | Power connector fit and retention | Actual XT30 dimensions, printed pocket fit, adhesive compatibility with connector and housing materials, wire support and insulated terminations ([selected arrangement](../hardware/housing.md#connectors)) | Fit-test print and assembly check before final housing design |
| 12 | Supply-cable protection | Owner confirmation of cable length, routing and wire specification; determine protection for the cable upstream of F1 ([power wiring](../hardware/power.md#supply)) | Owner supplies harness details; agree cable protection before aircraft connection |
| 13 | F1 holder and fuse format — deferred | Exact holder, fuse format, mounting dimensions and replacement access; only the in-housing location is selected ([power wiring](../hardware/housing.md#connectors)) | Resume owner selection before housing cutout design |
| 14 | Combined DE-9 instrument connection | Pin assignments for the nine signals; exact mating parts, wire-gauge compatibility, screw threads, backshell, strain relief and housing mounting; identify existing loose conductors and shield termination | Owner selects details; fit and continuity checks before connection |

---

<a id="housing"></a>

## 2. 📦 Housing — next

Mechanical design of the bridge ([housing](../hardware/housing.md)). Connector and F1 details are items 11, 13 and 14 above.

| # | Item | What is needed | How it is closed |
| :-: | --- | --- | --- |
| 1 | Part sizes | Measure the delivered ESP32 board, XT30, DE-9 socket and UBEC with leads | Bench measurement |
| 2 | Material and manufacturing | How the housing is made (for example 3D print) and from what | Owner selection |
| 3 | Location in the aircraft | Where the bridge sits and how it is attached | Owner / Tom |
| 4 | Module mounting and layout | How each module is held, and where it sits | Owner selection, fit test |
| 5 | Access | Whether the UBEC jumper, Micro-USB and F1 are reachable without opening the housing | Owner selection |
| 6 | Wi-Fi path | Antenna position relative to the phone and surrounding structure | Owner selection; check in the aircraft |

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
