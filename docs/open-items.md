# Open items

The one list of what is missing, who supplies it and what it blocks. All items are open.

[Roadmap](roadmap.md) · [Assumptions](project-baseline.md#assumptions-to-resolve) · [Evidence labels](sources.md#evidence-labels)

## Installed equipment (WP1)

Reported state: [existing installation](project-baseline.md#existing-installation--wp1-record). Closing the interface-critical items is the gate for milestone M1.

| Item | Needed | Owner | Blocks |
| --- | --- | --- | --- |
| Radio | Type plate, article, hardware/software versions and connector occupancy of the reported AR6201-(022) | Tom | Manual and interface matching |
| Zander ZS1 | Program version, menu/baud settings, cable, levels, returns | Tom | Compatible wiring |
| FLARM | Label and firmware (reported Classic F6/IGC), actual socket, baud/output settings, returns | Tom | Interface match |
| Existing wiring | Trace each selected TX/reference through any conversion to GPIO; identify returns. Absence of RS-232 conversion is conjectured | Tom | Equipment wiring |
| Existing converter | IC, supply, logic levels and installed status of the RS422 TO TTL board | Tom | Backup record |
| Existing ESP32 | Board, firmware, configuration | Tom | Backup record |
| Existing power | Actual UBEC jumper/output, board input, source range, protection, grounding. No measurements yet | Tom / owner | Backup record; protection and current budget |
| Navigation baseline | Data types, intervals/gaps, duration, routing/returns, reconnect behavior | Owner / Tom | Coexistence acceptance limits |
| OpenSoar | Installed build; evidence for the reported drivers/endpoints; OpenVario's role | Owner / Tom | App and bridge configuration |

## Bench hardware (WP3)

Design: [connections](connections.md) · [module specs](module-specs.md).

| Item | Needed | Resolves |
| --- | --- | --- |
| Device connectors | Connector photos; exact model and revisions | Correct pin view; FLARM/Zander/Becker applicability |
| Becker J1 | Installed J1 revision, moulded pin numbers, pair polarity, D_GND bond | Becker wiring and reference |
| MAX490 board | Chip identity, pinout, current draw, termination, protection; UART RX voltage compliance through power-up and power-down | Becker wiring, power rail and ESP32 input safety |
| Hailege H-1-0290 (both) | Both sides, IC markings, continuity map, dimensions, 3.3 V operation | Inferred L/R pad map; equipment cable mapping |
| ESP32 board | Module marking, header positions, regulator limits, current/thermal budget | Header positions, current budget |
| UBEC and source | Delivered part, polarity, 5 V output, source voltage range, protection ratings, clearance | Power path |
| Protection | F1 rating ([sizing inputs](connections.md#f1-sizing-inputs)); reverse-polarity and transient/overvoltage parts and ratings | Protection design; purchases |
| Serial settings and returns | Return-traffic requirements; serial settings per device | Optional return wiring and port configuration |
| Network settings | New-build network configuration and device endpoints | App and bridge configuration |

## Protocol, software and acceptance (WP2, WP4–WP6)

| Item | Needed | Blocks |
| --- | --- | --- |
| Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, channel conversion vectors ([details](technical-investigation.md#protocol-details-to-validate-independently)) | Driver plan; independent test vectors |
| Radio configuration | Matching installation manual, serial settings, remote-control configuration | Wiring and port configuration |
| Application integration | AR62xx availability over TCP, simultaneous navigation connections, status presentation | Driver plan |
| Firmware selection | Candidate audit, file licenses, reproducible build inputs ([ADR-02](architecture.md#adr-02--implementation-and-reuse-baseline)) | Bridge implementation |
| Driver regression | Embedded zero/A5, short frames, invalid CRC, unknown initial state, two instances, absent/late responses | Software verification (M3) |
| Bridge reliability | Bidirectional all-byte tests, oversized bursts, slow clients, partial writes, reconnects, concurrent navigation | Software verification (M3) |
| Physical acceptance | Frequency change and status readback on the real radio while preserving local controls and navigation | Aircraft integration |
