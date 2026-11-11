# Requirements

[← Project overview](../../README.md)

What the bridge must do, the assumptions behind the plan and the risks. One new ESP32 bridge; the existing setup stays as backup. Physical operation is unverified.

[Open items](../project/open-items.md) · [Roadmap](../project/roadmap.md) · [Evidence labels](../research/sources.md#evidence-labels)

## Requirements

| ID | Priority | Requirement | Acceptance evidence / package |
| --- | --- | --- | --- |
| R01 | P0 | Transfer the selected airfield channel into standby; preserve the actual active channel. | Actual standby readback and active-channel preservation remain unverified. |
| R02 | P0 | Preserve Zander and FLARM reception. | WP1 records the navigation baseline. WP6 covers bridge coexistence. Actual navigation behavior remains unverified. |
| R03 | P0 | Carry radio bytes both ways without truncation, line conversion, filtering or mixing with diagnostics/navigation. | WP5/WP6: counted all-byte patterns, embedded zero/A5, split/coalesced reads, oversized bursts, slow readers and partial writes; compare exact bytes and order. |
| R04 | P0 | Distinguish transport completion, radio acceptance and verified state; report failure or unknown state honestly. | WP2 defines actual response rules. WP5 covers valid replies, rejection if supported, silence, malformed/late replies and disconnect. A successful write alone never passes. |
| R05 | P0 | Keep local radio controls usable when the bridge disconnects or loses power. | Local-control behavior with the new bridge remains unverified. |
| R06 | P0 | Match electrical interface and serial configuration to the actual equipment. | WP2 records exact applicable manual, connector view, directions, levels and settings. No direct incompatible signals on ESP32 GPIO. |
| R07 | P1 | Make results reproducible and minimize custom development. | WP3 compares reuse options and licenses; WP5–WP6 record versions, configuration, steps, expected/actual results and limitations. |
| R08 | P1 | Receive radio state where supported, and specify what OpenSoar can display. | WP2/WP4 establish readback and UI capability; WP5 covers the supported software behavior and limitations. |
| R09 | P0 | Use a full-duplex RS-422 board with UART levels within ESP32 limits for Becker. | WP3 board selection, pinout and electrical checks. |

**Deferred:** active tuning, swap, volume/squelch, Larus, general XCSoar support, aircraft installation. Repository license undecided.

---

## Assumptions

| ID | Assumption, not a fact | How to resolve / dependency |
| --- | --- | --- |
| A01 | The installed radio is a remotely controllable AR6201 variant. | WP1 identity, then WP2 matching manufacturer documentation. Blocks hardware design. |
| A03 | The installed OpenSoar build supports the desired driver/transport combination alongside navigation. | WP1 build/configuration, then WP4 application integration review. Blocks end-to-end implementation choice. |
| A04 | Radio status can establish sufficiently fresh active state before standby tuning. | WP2 protocol evidence and WP4 state design. Actual state freshness remains unverified. Blocks safe command semantics. |
| A05 | A dedicated radio TCP endpoint is feasible without disrupting existing feeds. | WP1 topology and WP3 alternatives comparison. Blocks ADR-01 acceptance. |
| A06 | The selected new ESP32 board and firmware can carry all required navigation paths plus an isolated bidirectional radio path. | WP1 equipment requirements, WP3 resource allocation and WP6 bridge coexistence checks. New hardware behavior remains unverified. |

---

## Risks

All risks open.

| ID | Priority | Risk and evidence | Mitigation / closure evidence |
| --- | --- | --- | --- |
| K01 | P0 | Wrong device/manual could give incompatible wiring (A01; S03 lists distinct hardware manuals). | WP1/WP2 match model, revision and connector view. |
| K02 | P0 | Standby command can include unknown/stale active state (F04). | WP4 explicit state validity and refresh policy; WP5 startup, reconnect and local-tuning cases. |
| K03 | P0 | Truncated binary writes or false success (F02/F03). | Targeted driver review and regression cases in WP4/WP5. Actual radio acceptance remains unverified. |
| K04 | P0 | Bridge overflow, multiple writers or debug bytes could corrupt traffic; earlier candidate reviews report these hazards (S05/S06). | WP3 candidate audit, WP6 all-byte/load/failure checks and independent radio routing. |
| K05 | P0 | Shared resources on the new bridge could interrupt navigation (A05/A06). | Record WP1 baseline; compare coexistence under WP6 conditions. |
| K06 | P0 | Simulator could merely reproduce driver assumptions. | WP2 independent protocol evidence/vectors; label synthetic cases. Simulation never establishes physical acceptance. |
| K07 | P1 | License, dependency or maintenance assumptions may prevent reuse. | WP3 inspect pinned license files and build dependencies. No code copying before permission is established. |
