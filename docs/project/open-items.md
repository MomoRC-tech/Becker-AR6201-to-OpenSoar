# Open items

[← Project overview](../../README.md)

The one list of what is missing, who supplies it and what it blocks. All items are open.

[Roadmap](roadmap.md) · [Assumptions](../design/requirements.md#assumptions) · [Evidence labels](../research/sources.md#evidence-labels)

## Installed equipment (WP1)

Reported state: [existing installation](existing-installation.md). Closing the interface-critical items is the gate for milestone M1.

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

Design: [connections](../hardware/wiring.md) · [module specs](../hardware/modules.md).

| Item | Needed | How it is closed |
| --- | --- | --- |
| Device connectors | Photos of the installed connectors and exact device models/revisions (from Tom) | Confirms the pin views used in the wiring tables |
| Becker J1 | Installed J1 revision, moulded pin numbers and which pins are already in use | Confirms the J1 pins and the J1-21 ground connection |
| MAX490 board | Which header pin is RO and which is DI; termination; current draw | Bench check on the delivered board |
| GPIO16 input | Voltage at GPIO16 behind the 10 kΩ resistor, also at power-up and power-down | Bench measurement |
| Hailege modules | Continuity of both modules against the inferred pad map; IC marking | Bench check |
| ESP32 board | Module marking and header positions match the DevKitC V4 reference | Visual check on the delivered board |
| UBEC | 5 V output with and without load; fit in the enclosure | Bench measurement |
| Current and fuse | Running current and switch-on inrush at 12 V, to confirm the 2 A fuse | Bench measurement |
| Radio noise | No audible noise or squelch opening from the bridge | Check in the aircraft |
| Return paths | Whether the Zander and FLARM return lines (GPIO33, GPIO27) are needed | Decides whether those wires are fitted |

## Protocol, software and acceptance (WP2, WP4–WP6)

| Item | Needed | Blocks |
| --- | --- | --- |
| Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, channel conversion vectors ([details](../research/becker-driver.md#protocol-details-to-validate-independently)) | Driver plan; independent test vectors |
| Radio configuration | Matching installation manual, serial settings, remote-control configuration | Port configuration |
| Application integration | AR62xx availability over TCP, simultaneous navigation connections, status presentation | Driver plan |
| Firmware selection | Candidate audit, file licenses, reproducible build inputs ([ADR-02](../design/architecture.md#adr-02--implementation-and-reuse-baseline)) | Bridge implementation |
| Driver regression | Embedded zero/A5, short frames, invalid CRC, unknown initial state, two instances, absent/late responses | Software verification (M3) |
| Bridge reliability | Bidirectional all-byte tests, oversized bursts, slow clients, partial writes, reconnects, concurrent navigation | Software verification (M3) |
| Physical acceptance | Frequency change and status readback on the real radio while preserving local controls and navigation | Aircraft integration |
