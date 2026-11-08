# Project baseline

Updated 2026-11-08. Evidence labels and source IDs are defined in [Sources](sources.md). This file owns requirements, the installed-system inventory, assumptions and risks. Task status belongs in the [roadmap](roadmap.md).

## Scope and requirements

These are target behaviors from the existing project brief, not implemented or accepted results. Priority P0 protects the minimum use case; P1 supports reproducibility. Final physical acceptance is still open for every requirement.

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

| Item | Reported information | Still needed | Evidence/status |
| --- | --- | --- | --- |
| Radio | Target named AR6201; earlier notes also say AR6102 | Exact model/part number, hardware and software revisions; connector occupancy | Reported / identity unresolved |
| ESP32 | Existing serial-to-Wi-Fi bridge | Board/SoC, firmware project and revision, config backup, UART/pin ownership, boot/debug outputs | Reported / artifacts missing |
| Zander | ZS1 navigation input | Connection, signal levels, serial settings, routing and baseline reception | Reported / unmeasured |
| FLARM | Additional serial input | Model, format, levels, serial settings, routing and baseline reception | Reported / unmeasured |
| Android/OpenSoar | Receives navigation via Wi-Fi | App/build version, device entries, drivers, transport and endpoint mapping | Reported / configuration missing |
| Existing wiring | No verified map available | Sanitized as-built map, level converters, UART ownership and power paths | Unknown |
| Navigation baseline | Successful reception reported | Data types, update intervals/gaps, observation duration and reconnect behavior | Unknown |

WP1 fills this table from evidence and adds an as-built map here. A proposed map belongs in [architecture](architecture.md), never in this inventory. Backup material stays private; do not alter or reflash working equipment to obtain it.

## Assumptions to resolve

| ID | Assumption, not a fact | How to resolve / dependency |
| --- | --- | --- |
| A01 | The installed radio is a remotely controllable AR6201 variant. | WP1 identity, then WP2 matching manufacturer documentation. Blocks hardware design. |
| A02 | The existing ESP32 can support an isolated bidirectional radio path. | WP1 resources/configuration, then WP3 routing review. Blocks reuse selection. |
| A03 | The installed OpenSoar build supports the desired driver/transport combination alongside navigation. | WP1 build/configuration, then WP4 application integration review. Blocks end-to-end implementation choice. |
| A04 | Radio status can establish sufficiently fresh active state before standby tuning. | WP2 protocol evidence, WP4 state design and WP7 observation. Blocks safe command semantics. |
| A05 | A dedicated radio TCP endpoint is feasible without disrupting existing feeds. | WP1 topology and WP3 alternatives comparison. Blocks ADR-01 acceptance. |

## Risk register

All risks are open. The package owner records evidence and proposes closure; the project owner accepts material scope/compatibility decisions.

| ID | Priority | Risk and evidence | Mitigation / closure evidence |
| --- | --- | --- | --- |
| K01 | P0 | Wrong device/manual could give incompatible wiring (A01; S03 lists distinct hardware manuals). | WP1/WP2 match model, revision and connector view; WP7 documented bench verification. |
| K02 | P0 | Standby command can include unknown/stale active state (F04). | WP4 explicit state validity and refresh policy; WP5/WP7 startup, reconnect and local-tuning cases. |
| K03 | P0 | Truncated binary writes or false success (F02/F03). | Targeted driver review and regression cases in WP4/WP5; real acceptance evidence in WP7. |
| K04 | P0 | Bridge overflow, multiple writers or debug bytes could corrupt traffic; earlier candidate reviews report these hazards (S05/S06). | WP3 candidate audit, WP6 all-byte/load/failure checks and independent radio routing. |
| K05 | P0 | Shared resources could interrupt navigation (A02/A05). | Record WP1 baseline; compare coexistence under WP6/WP7 conditions. |
| K06 | P0 | Simulator could merely reproduce driver assumptions. | WP2 independent protocol evidence/vectors; label synthetic cases. Simulation never establishes physical acceptance. |
| K07 | P1 | License, dependency or maintenance assumptions may prevent reuse. | WP3 inspect pinned license files and build dependencies. No code copying before permission is established. |
