# Project baseline

[Evidence labels](sources.md) · [Work queue](roadmap.md)

## Scope and requirements

One new ESP32 bridge; preserve the existing setup as backup. Physical operation remains unverified.

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

Deferred: active tuning, swap, volume/squelch, Larus, general XCSoar support, aircraft installation. Repository license undecided.

---

## New build selections

| Item | Current selection | Missing / consequence |
| --- | --- | --- |
| ESP32 | AZ-Delivery ESP32 WROOM dev module (owner selection) | Confirm header positions, regulator limits and current/thermal budget |
| Navigation | 2 × Hailege H-1-0290; reported MAX3232CSE, 9.4 × 15.9 mm (S26) | S27 inferred pad map; build operator: confirm IC/pad continuity, dimensions and 3.3 V operation |
| Supply | Nominal 12 V battery → 2S–4S-rated BEC → 5 V ESP32 → 3.3 V navigation converters | Owner: BEC model, chemistry/voltage range; protection ratings |
| Radio | Assembled RS422-to-TTL board (owner selection, S28) | 5 V and header directions in S29. MAX490ESA owner confirmed. Board dimensions of 50 × 27 mm reported in S30. ESP32 level interface and radio match pending |

[Design](architecture.md) · [Wiring](connections.md) · [Parts list](connections.md#parts-list). USB and BEC powering mutually exclusive; manual disconnection.

---

## Existing installation — WP1 record

**Reported (S00/S21):** DG200 Zander/FLARM → ESP32 Wi-Fi → OpenSoar; no working Becker integration. [2026-11-09 claim assessment](reviews/2026-11-09-installation-claims.md). No measured baseline.

| Item | Known | Missing / owner / consequence |
| --- | --- | --- |
| Radio | AR6201-(022), ETSO-2C37e/2C38e reported (S21); consistent manual entry (S10) | Tom: type plate/article/HW/SW/connector occupancy → installed manual matching |
| ESP32 | WROOM-family board, micro-USB, BOOT/RST (S13) | Tom: board/firmware/configuration → backup record |
| Zander | ZS1; FLARM input unused, no FLARM connected, reported (S21) | Tom: program version, menu/baud, cable/levels/returns → compatible wiring |
| FLARM | Classic FLARM IGC, series 6 reported (S21); F6 manual candidate (S23) | Tom: label/firmware, actual socket, baud/output/returns → interface match |
| OpenSoar | Wi-Fi reception, drivers/endpoints reported (S21), below | Owner/Tom: installed build and configuration evidence → confirmation |
| Converter | RS422 TO TTL board, Y/Z/B/A labels (S13 P1) | Tom: IC/supply/logic levels and installed status → baseline |
| Power | Nominal 12 V → Hobbywing UBEC 3A 2–6S → ESP32; separate FLARM/Zander supplies reported (S21) | Tom: actual jumper/output, board input, source range/protection/grounding; no measurements |
| Wiring | Partial photos; unmatched Zander/FLARM diagrams (S25); absence of RS-232 conversion only conjectured | Tom: trace each selected TX/reference through any conversion to GPIO; identify returns |
| Navigation | Reception reported | Owner/Tom: data types, intervals/gaps, duration, reconnect → acceptance limits |

Keep logs, identifiers, credentials and raw backups private. Preserve backup hardware/harness unchanged.

### Pending configuration

| Device | Reported TCP endpoint | Reported OpenSoar driver |
| --- | --- | --- |
| Zander | `10.10.10.10:10110` | `Zander / SDI` |
| FLARM | `10.10.10.10:2000` | `FLARM` |
| Becker | `10.10.10.10:4352` | `AR62xx`; entry/intention only, no integrated radio |

Separate navigation paths reported. Wiring, return traffic, installed OpenSoar build and OpenVario’s role are open. Review baseline is OpenSoar 7.46.26 (S09/S24). See S24 for Zander fields consumed.

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

**WP1 verification:** [current package summary](reviews/2026-11-09-installation-claims.md#verification), including retained S13 photo evidence. WP1/M1 open.

---

## Assumptions to resolve

| ID | Assumption, not a fact | How to resolve / dependency |
| --- | --- | --- |
| A01 | The installed radio is a remotely controllable AR6201 variant. | WP1 identity, then WP2 matching manufacturer documentation. Blocks hardware design. |
| A02 | Retired: spare capacity in existing ESP32. | Backup preserved; new bridge capacity covered by A06 |
| A03 | The installed OpenSoar build supports the desired driver/transport combination alongside navigation. | WP1 build/configuration, then WP4 application integration review. Blocks end-to-end implementation choice. |
| A04 | Radio status can establish sufficiently fresh active state before standby tuning. | WP2 protocol evidence and WP4 state design. Actual state freshness remains unverified. Blocks safe command semantics. |
| A05 | A dedicated radio TCP endpoint is feasible without disrupting existing feeds. | WP1 topology and WP3 alternatives comparison. Blocks ADR-01 acceptance. |
| A06 | The selected new ESP32 board and firmware can carry all required navigation paths plus an isolated bidirectional radio path. | WP1 equipment requirements, WP3 resource allocation and WP6 bridge coexistence checks. New hardware behavior remains unverified. |

---

## Risk register

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
