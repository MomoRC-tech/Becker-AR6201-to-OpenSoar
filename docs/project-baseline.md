# Project baseline

[Evidence labels](sources.md) · [Work queue](roadmap.md)

## Scope and requirements

One new ESP32 bridge; preserve the existing setup as backup. All physical acceptance open.

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

Deferred: active tuning, swap, volume/squelch, Larus, general XCSoar support, aircraft installation. Repository license undecided.

## New build selections

| Item | Current selection | Missing / consequence |
| --- | --- | --- |
| ESP32 | AZ-Delivery WROOM-32 | Owner: exact board/regulator/header; current/thermal budget |
| Navigation | 2 × HW-044; apparent MAX3232 | Build operator: IC/header/DB9 continuity and 3.3 V checks |
| Supply | Nominal 12 V battery → 2S–4S-rated BEC → 5 V board → 3.3 V converters | Owner: BEC model, chemistry/voltage range; protection ratings |
| Radio | THVD1451D proposed | Matched radio interface and bench validation |

[Design](architecture.md) · [BOM/pinning](connections.md). USB and BEC powering mutually exclusive; manual disconnection.

## Existing installation — WP1 record

**Reported (S00):** DG200 navigation → ESP32 Wi-Fi → OpenSoar. No measured baseline.

| Item | Known | Missing / owner / consequence |
| --- | --- | --- |
| Radio | AR6201 target; not visible in photos | Tom: part/HW/SW/connector occupancy → manual matching |
| ESP32 | WROOM-family board, micro-USB, BOOT/RST (S13) | Tom: board/firmware/configuration → backup record |
| Zander | ZS1 input reported | Tom: connector/levels/settings/returns → compatible wiring |
| FLARM | Serial input reported | Tom: model/ports/levels/settings/returns → interface choice |
| OpenSoar | Wi-Fi reception reported | Owner/Tom: build, device entries, drivers/endpoints → configuration |
| Converter | RS422 TO TTL board, Y/Z/B/A labels (S13 P1) | Tom: IC/supply/logic levels and installed status → baseline |
| Power | HOBBYWING UBEC label, 3A, blue jumper (S13 P2) | Tom: source/protection/grounding; output unmeasured |
| Wiring | Partial assembly/harness photos | Tom: external connector map/directions → new harness |
| Navigation | Reception reported | Owner/Tom: data types, intervals/gaps, duration, reconnect → acceptance limits |

Keep logs, identifiers, credentials and raw backups private. Preserve backup hardware/harness unchanged.

### Pending configuration

Navigation topology/return traffic unknown. Review baseline: OpenSoar 7.46.26 (S09); installed build unknown.

### Tom’s photo evidence — 2026-11-08

S13: four private photos; visible features only. No powered/continuity tests or complete equipment-to-GPIO path.

| Photo | Rechecked source: visible observation | Inference / limit |
| --- | --- | --- |
| P1 | Blue board marked RS422 TO TTL; four screw terminals marked Y, Z, B, A; VCC/GND markings at the wired header; IC marking too blurred to identify | Relevant candidate for the proposed radio interface. Separate terminal pairs suggest a full-duplex module, but the IC/circuit must establish direction, polarity, supply, logic levels, termination and isolation. TTL wording does not establish 3.3 V compatibility. No connection to the radio or pictured ESP32 is traceable |
| P2 | HOBBYWING UBEC-labelled module, visible 3A marking, blue jumper, stripboard and soldered wiring | Likely the bridge’s regulator; exact model/revision, jumper function/setting and delivered voltage are unverified. S14 describes a similar 5/6 V selectable product, not a measurement of this unit |
| P3 | ESP32 WROOM-family development board; module marking appears to read ESP-WROOM-32; micro-USB, BOOT/RST buttons, pin labels and attached wiring visible | Supports the classic ESP32/WROOM family as a reuse lead. Does not establish an official DevKitC V4, WROOM-32E, board schematic, firmware, spare UARTs or complete GPIO assignments |
| P4 | Wider view of the board, green enclosure and partly covered cable assembly | Supports assembly context; covered joins and off-frame endpoints prevent an end-to-end wiring map. Wire colour alone does not establish function or voltage |

| Segment | Supported map | Unresolved endpoint / function |
| --- | --- | --- |
| Enclosure assembly | P2–P4 show the ESP32 board, stripboard, UBEC module and wiring as parts of the photographed assembly | Exact interconnections, supply entry, regulator output destination and any concealed interface circuits |
| External harness | P3/P4 show conductors from the harness reaching the board area; sections are covered | Connector contacts, GPIOs, serial directions and which branch serves Zander or FLARM |
| RS-422 module | P1 shows wires attached at its logic-side header and four labelled field terminals | Far ends of those wires, field-side cable, radio connection and relationship to the enclosure assembly |
| Navigation over Wi-Fi | S00 reports navigation reception through the ESP32 into OpenSoar | Whether feeds are combined, network transport/endpoints, UART settings and return traffic; not visible or measured in S13 |

#### Comparison with the proposal

Existing assembly retained as backup ([ADR-03](architecture.md#adr-03--new-hardware-with-the-existing-setup-as-backup)); no component reuse planned.

**WP1 verification:** four photos inspected; S14 UBEC family reference checked. No voltage/continuity, firmware, navigation or radio tests. WP1/M1 open.

## Assumptions to resolve

| ID | Assumption, not a fact | How to resolve / dependency |
| --- | --- | --- |
| A01 | The installed radio is a remotely controllable AR6201 variant. | WP1 identity, then WP2 matching manufacturer documentation. Blocks hardware design. |
| A02 | Retired: spare capacity in existing ESP32. | Backup preserved; new bridge capacity covered by A06 |
| A03 | The installed OpenSoar build supports the desired driver/transport combination alongside navigation. | WP1 build/configuration, then WP4 application integration review. Blocks end-to-end implementation choice. |
| A04 | Radio status can establish sufficiently fresh active state before standby tuning. | WP2 protocol evidence, WP4 state design and WP7 observation. Blocks safe command semantics. |
| A05 | A dedicated radio TCP endpoint is feasible without disrupting existing feeds. | WP1 topology and WP3 alternatives comparison. Blocks ADR-01 acceptance. |
| A06 | The selected new ESP32 board and firmware can carry all required navigation paths plus an isolated bidirectional radio path. | WP1 equipment requirements, WP3 resource allocation and WP6/WP7 coexistence verification; existing backup operation does not prove this |

## Risk register

All risks open.

| ID | Priority | Risk and evidence | Mitigation / closure evidence |
| --- | --- | --- | --- |
| K01 | P0 | Wrong device/manual could give incompatible wiring (A01; S03 lists distinct hardware manuals). | WP1/WP2 match model, revision and connector view; WP7 documented bench verification. |
| K02 | P0 | Standby command can include unknown/stale active state (F04). | WP4 explicit state validity and refresh policy; WP5/WP7 startup, reconnect and local-tuning cases. |
| K03 | P0 | Truncated binary writes or false success (F02/F03). | Targeted driver review and regression cases in WP4/WP5; real acceptance evidence in WP7. |
| K04 | P0 | Bridge overflow, multiple writers or debug bytes could corrupt traffic; earlier candidate reviews report these hazards (S05/S06). | WP3 candidate audit, WP6 all-byte/load/failure checks and independent radio routing. |
| K05 | P0 | Shared resources on the new bridge could interrupt navigation (A05/A06). | Record WP1 baseline; compare coexistence under WP6/WP7 conditions. |
| K06 | P0 | Simulator could merely reproduce driver assumptions. | WP2 independent protocol evidence/vectors; label synthetic cases. Simulation never establishes physical acceptance. |
| K07 | P1 | License, dependency or maintenance assumptions may prevent reuse. | WP3 inspect pinned license files and build dependencies. No code copying before permission is established. |
