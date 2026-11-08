# Project baseline

Updated 2026-11-08. Evidence labels and source IDs are defined in [Sources](sources.md). This file owns requirements, the installed-system inventory, assumptions and risks. Task status belongs in the [roadmap](roadmap.md).

## Scope and requirements

The owner agreed the first-version goal on 2026-11-08: standby transfer while preserving active frequency, local controls and navigation, with honest failure/unknown-state reporting. One ESP32 is the accepted architecture constraint. The owner subsequently chose new, separate hardware while retaining the existing setup unchanged as backup (S15; [ADR-03](architecture.md#adr-03--new-hardware-with-the-existing-setup-as-backup)). These are agreed goals and constraints, not implemented or physically accepted results. Priority P0 protects the minimum use case; P1 supports reproducibility. Final physical acceptance is still open for every requirement.

| ID | Priority | Requirement | Acceptance evidence / package |
| --- | --- | --- | --- |
| R01 | P0 | Transfer the selected airfield channel into standby; preserve the actual active channel. | WP7: compare selected value, command/readback and radio display before/after, including after local tuning and reconnect. |
| R02 | P0 | Preserve Zander and FLARM reception. | WP1 records the baseline; WP6/WP7 repeat it under radio traffic and reconnects. Define acceptable update gaps from the baseline before testing. |
| R03 | P0 | Carry radio bytes both ways without truncation, line conversion, filtering or mixing with diagnostics/navigation. | WP5/WP6: counted all-byte patterns, embedded zero/A5, split/coalesced reads, oversized bursts, slow readers and partial writes; compare exact bytes and order. |
| R04 | P0 | Distinguish transport completion, radio acceptance and verified state; report failure or unknown state honestly. | WP2 defines actual response rules. WP5/WP7 test valid replies, rejection if supported, silence, malformed/late replies and disconnect. A successful write alone never passes. |
| R05 | P0 | Keep local radio controls usable when the bridge disconnects or loses power. | WP7: observed bench behavior with bridge connected, disconnected and off. |
| R06 | P0 | Match electrical interface and serial configuration to the actual equipment. | WP2 records exact applicable manual, connector view, directions, levels and settings; WP7 verifies the documented bench setup. No direct incompatible signals on ESP32 GPIO. |
| R07 | P1 | Make results reproducible and minimize custom development. | WP3 compares reuse options and licenses; WP5–WP7 record versions, configuration, steps, expected/actual results and limitations. |
| R08 | P1 | Receive radio state where supported, and specify what OpenSoar can display. | WP2/WP4 establish readback and UI capability; WP5/WP7 verify it or record the limitation before acceptance. |

Direct active tuning, swap, volume/squelch, Larus integration, general XCSoar support and aircraft installation are deferred. Existing code for those functions is not a requirement to implement them now. Choosing a license for this repository also remains open; public visibility alone does not establish reuse permission.

## Existing installation — WP1 record

Reported context from the previous README/brief (S00): a DG200 installation receives navigation data in OpenSoar through an ESP32. It is not a measured baseline. Do not copy private logs, SSIDs, aircraft identifiers, full serial numbers or credentials into this public repository. Record sanitized conclusions and a non-sensitive evidence reference/date instead.

| Item | Reported / visible information | Still needed, owner and consequence | Evidence/status |
| --- | --- | --- | --- |
| Radio | Becker AR6201 is the project target; not identified by the photos | Tom: installed unit’s part number, HW/SW revisions and connector occupancy; blocks matching the manual/interface | Reported / installed variant unverified |
| ESP32 | Photos show a WROOM-family module on a development board with micro-USB, BOOT/RST buttons and two long pin rows | Tom: board/firmware identity and configuration document the backup; internal UART/GPIO ownership is useful context but no longer a prerequisite for allocating the new board's resources | S13 P3/P4 rechecked photo source; exact board and firmware unknown; preserved under ADR-03 |
| Zander | ZS1 navigation input reported; endpoints not traceable in photos | Tom: cable/connector map, signal levels, serial settings, routing and return-traffic needs; blocks preserving the existing path | Reported / unmeasured |
| FLARM | Additional serial input reported; model not shown | Tom: model, occupied ports, format, levels, serial settings, routing and return-traffic needs; blocks interface and port choice | Reported / unmeasured |
| Android/OpenSoar | Receives navigation via Wi-Fi | Owner, with Tom if needed: app/build version, device entries, drivers, transport and endpoint mapping; blocks app/bridge configuration comparison | Reported / configuration missing |
| Serial converters | Separate board marked RS422 TO TTL, with Y/Z/B/A terminals and a wired logic header | Tom: identity, supply/logic levels, wiring and whether installed document the baseline. New converters need their own matched specifications; no existing module is selected for reuse. Navigation converters remain unidentified | S13 P1 rechecked photo source; compatibility unknown |
| Bridge power | HOBBYWING UBEC-labelled module in the assembly, with a blue jumper visible | Tom: available supply source/protection and grounding constrain the new design; old regulator identity/output document the backup. New supply circuitry requires separate selection. S14 is a product-family lead only | S13 P2 rechecked photo source; no voltage measurement |
| Existing wiring | ESP32, stripboard, power module, enclosure and partly covered harness visible | Tom: instrument-side connector map, signal references, serial directions and power paths; blocks compatible new wiring. Preserve the original harness as backup | S13 partial photo map below; no continuity evidence |
| Navigation baseline | Successful reception reported | Owner/Tom: data types, update intervals/gaps, observation duration and reconnect behavior; blocks defining and verifying preservation of existing reception | Reported / unmeasured |

The partial as-built record below contains only supported observations and explicit gaps. Proposed wiring remains in [connections](connections.md). Backup material stays private; do not alter or reflash working equipment to obtain it.

### Owner clarification — 2026-11-08

- **Reported:** Zander/FLARM topology and any OpenSoar return traffic are unknown. Tom can supply radio identity/revisions, FLARM model, occupied connectors, wiring and power information; obtaining this will take time.
- **Accepted constraint:** one ESP32. A board/topology proposal is requested; it does not identify the existing board or authorize replacement.
- **Assumption / planning target:** latest released OpenSoar. S09 identifies 7.46.26 as the latest listed release on the review date; installed build/configuration remains unknown.
- **Consequence:** final connector pinning, voltage conversion, power design, serial settings and navigation directionality await Tom’s information. Public-source architecture work may continue as a proposal.

### Tom’s photo evidence — 2026-11-08

The owner supplied four photos attributed to Tom (S13). This is visual evidence of the pictured hardware, not a powered test or confirmation that every item is currently connected. Raw images remain outside version control; aircraft markings and other identifiers are omitted here.

| Photo | Rechecked source: visible observation | Inference / limit |
| --- | --- | --- |
| P1 | Blue board marked RS422 TO TTL; four screw terminals marked Y, Z, B, A; VCC/GND markings at the wired header; IC marking too blurred to identify | Relevant candidate for the proposed radio interface. Separate terminal pairs suggest a full-duplex module, but the IC/circuit must establish direction, polarity, supply, logic levels, termination and isolation. TTL wording does not establish 3.3 V compatibility. No connection to the radio or pictured ESP32 is traceable |
| P2 | HOBBYWING UBEC-labelled module, visible 3A marking, blue jumper, stripboard and soldered wiring | Likely the bridge’s regulator; exact model/revision, jumper function/setting and delivered voltage are unverified. S14 describes a similar 5/6 V selectable product, not a measurement of this unit |
| P3 | ESP32 WROOM-family development board; module marking appears to read ESP-WROOM-32; micro-USB, BOOT/RST buttons, pin labels and attached wiring visible | Supports the classic ESP32/WROOM family as a reuse lead. Does not establish an official DevKitC V4, WROOM-32E, board schematic, firmware, spare UARTs or complete GPIO assignments |
| P4 | Wider view of the board, green enclosure and partly covered cable assembly | Supports assembly context; covered joins and off-frame endpoints prevent an end-to-end wiring map. Wire colour alone does not establish function or voltage |

#### Partial as-built connection map

This table maps the visible segments without inventing electrical connections. No equipment-to-GPIO path is established by these photos.

| Segment | Supported map | Unresolved endpoint / function |
| --- | --- | --- |
| Enclosure assembly | P2–P4 show the ESP32 board, stripboard, UBEC module and wiring as parts of the photographed assembly | Exact interconnections, supply entry, regulator output destination and any concealed interface circuits |
| External harness | P3/P4 show conductors from the harness reaching the board area; sections are covered | Connector contacts, GPIOs, serial directions and which branch serves Zander or FLARM |
| RS-422 module | P1 shows wires attached at its logic-side header and four labelled field terminals | Far ends of those wires, field-side cable, radio connection and relationship to the enclosure assembly |
| Navigation over Wi-Fi | S00 reports navigation reception through the ESP32 into OpenSoar | Whether feeds are combined, network transport/endpoints, UART settings and return traffic; not visible or measured in S13 |

#### Comparison with the proposal

Historical photo assessment below, before the owner's new-hardware decision. Its hardware-reuse recommendations are **superseded by [ADR-03](architecture.md#adr-03--new-hardware-with-the-existing-setup-as-backup)**; the visible findings and compatibility uncertainties remain valid.

| Part | Disposition | Reason / next evidence |
| --- | --- | --- |
| Existing ESP32 | **Unresolved; retain as a reuse candidate** | Photos give no reason by themselves to require replacement. Identify board, firmware and allocated resources before applying the proposed GPIO table |
| RS422 TO TTL module | **Unresolved** | Its stated function is relevant, but the photos cannot establish electrical compatibility or that it is already in use. Obtain an IC marking or exact product reference before choosing it |
| UBEC and power wiring | **Unresolved** | A regulator is present; a suitable output rail and board input are not established. Do not assume 5 V from appearance or wire colour |
| Navigation wiring / converters | **Unresolved** | Concealed or off-frame circuitry cannot be treated as absent. Preserve existing paths while obtaining the map |

Initial follow-up from the photo assessment: identify whether P1 is installed or a proposed addition, its IC/product identity, and the ESP32 firmware/GPIO assignments. ADR-03 changes the priority to instrument identity, external interfaces, serial settings, navigation behavior and supply requirements for the new build. Exact internal backup-board details are supporting context rather than a hardware-reuse gate. The photo assessment itself selected no components.

**WP1 photo increment verification:** inspected all four supplied photos and cross-checked the UBEC product-family lead S14; separated visible labels from inferred functions and unknown connections. Checked updated document consistency, relative links/anchors and whitespace. Public changes contain sanitized text only. No build, firmware access, continuity/voltage measurement, live navigation observation or radio test was performed. This increment supports R02/R06/R07 evidence collection; WP1 and M1 remain open.

## Assumptions to resolve

| ID | Assumption, not a fact | How to resolve / dependency |
| --- | --- | --- |
| A01 | The installed radio is a remotely controllable AR6201 variant. | WP1 identity, then WP2 matching manufacturer documentation. Blocks hardware design. |
| A02 | Superseded: the existing ESP32 must support the additional radio path. | ADR-03 preserves it as backup; its spare capacity no longer gates the new design. See A06 for the new bridge |
| A03 | The installed OpenSoar build supports the desired driver/transport combination alongside navigation. | WP1 build/configuration, then WP4 application integration review. Blocks end-to-end implementation choice. |
| A04 | Radio status can establish sufficiently fresh active state before standby tuning. | WP2 protocol evidence, WP4 state design and WP7 observation. Blocks safe command semantics. |
| A05 | A dedicated radio TCP endpoint is feasible without disrupting existing feeds. | WP1 topology and WP3 alternatives comparison. Blocks ADR-01 acceptance. |
| A06 | The selected new ESP32 board and firmware can carry all required navigation paths plus an isolated bidirectional radio path. | WP1 equipment requirements, WP3 resource allocation and WP6/WP7 coexistence verification; existing backup operation does not prove this |

## Risk register

All risks are open. The package owner records evidence and proposes closure; the project owner accepts material scope/compatibility decisions.

| ID | Priority | Risk and evidence | Mitigation / closure evidence |
| --- | --- | --- | --- |
| K01 | P0 | Wrong device/manual could give incompatible wiring (A01; S03 lists distinct hardware manuals). | WP1/WP2 match model, revision and connector view; WP7 documented bench verification. |
| K02 | P0 | Standby command can include unknown/stale active state (F04). | WP4 explicit state validity and refresh policy; WP5/WP7 startup, reconnect and local-tuning cases. |
| K03 | P0 | Truncated binary writes or false success (F02/F03). | Targeted driver review and regression cases in WP4/WP5; real acceptance evidence in WP7. |
| K04 | P0 | Bridge overflow, multiple writers or debug bytes could corrupt traffic; earlier candidate reviews report these hazards (S05/S06). | WP3 candidate audit, WP6 all-byte/load/failure checks and independent radio routing. |
| K05 | P0 | Shared resources on the new bridge could interrupt navigation (A05/A06). | Record WP1 baseline; compare coexistence under WP6/WP7 conditions. |
| K06 | P0 | Simulator could merely reproduce driver assumptions. | WP2 independent protocol evidence/vectors; label synthetic cases. Simulation never establishes physical acceptance. |
| K07 | P1 | License, dependency or maintenance assumptions may prevent reuse. | WP3 inspect pinned license files and build dependencies. No code copying before permission is established. |
