# Existing installation

[← Project overview](../../README.md)

The WP1 record of the bridge already in the aircraft: DG200 Zander/FLARM → ESP32 Wi-Fi → OpenSoar, no working Becker integration (reported, S21). No measured baseline. The assembly stays as backup ([ADR-03](../design/architecture.md#adr-03--new-hardware-with-the-existing-setup-as-backup)); no component reuse planned.

[Claim assessment and verification, 2026-11-09](../research/reviews/2026-11-09-installation-claims.md) · [Open items](open-items.md#installed-equipment-wp1) · [Evidence labels](../research/sources.md#evidence-labels)

## Reported equipment

| Item | Reported / known |
| --- | --- |
| Radio | AR6201-(022), ETSO-2C37e/2C38e reported (S21); consistent manual entry (S10) |
| ESP32 | WROOM-family board, micro-USB, BOOT/RST (S13) |
| Zander | ZS1; FLARM input unused, no FLARM connected, reported (S21) |
| FLARM | Classic FLARM IGC, series 6 reported (S21); F6 manual candidate (S23) |
| OpenSoar | Wi-Fi reception, drivers/endpoints reported (S21), below |
| Converter | RS422 TO TTL board, Y/Z/B/A labels (S13 P1) |
| Power | Nominal 12 V → Hobbywing UBEC 3A 2–6S → ESP32; separate FLARM/Zander supplies reported (S21) |
| Wiring | Partial photos; unmatched Zander/FLARM diagrams (S25); absence of RS-232 conversion only conjectured |
| Navigation | Reception reported |

Missing evidence and owners: [open items](open-items.md#installed-equipment-wp1).

> [!IMPORTANT]
> Keep logs, identifiers, credentials and raw backups private. Preserve the backup hardware and harness.

## Reported configuration

| Device | Reported TCP endpoint | Reported OpenSoar driver |
| --- | --- | --- |
| Zander | `10.10.10.10:10110` | `Zander / SDI` |
| FLARM | `10.10.10.10:2000` | `FLARM` |
| Becker | `10.10.10.10:4352` | `AR62xx`; entry/intention only, no integrated radio |

Separate navigation paths reported. Wiring, return traffic, installed OpenSoar build and OpenVario’s role are open. Review baseline is OpenSoar 7.46.26 (S09/S24). See S24 for Zander fields consumed.

## Photo evidence

S13: four private photos from Tom, inspected 2026-11-08; visible features only. No powered/continuity tests or complete equipment-to-GPIO path.

| Photo | Rechecked source: visible observation | Inference / limit |
| --- | --- | --- |
| P1 | Blue board marked RS422 TO TTL; four screw terminals marked Y, Z, B, A; VCC/GND markings at the wired header; IC marking too blurred to identify | Existing-installation evidence. Separate terminal pairs suggest a full-duplex module, but the IC/circuit must establish direction, polarity, supply, logic levels, termination and isolation. TTL wording does not establish 3.3 V compatibility. No connection to the radio or pictured ESP32 is traceable |
| P2 | HOBBYWING UBEC-labelled module, visible 3A marking, blue jumper, stripboard and soldered wiring | Likely the bridge’s regulator; exact model/revision, jumper function/setting and delivered voltage are unverified. S14 describes a similar 5/6 V selectable product, not a measurement of this unit |
| P3 | ESP32 WROOM-family development board; module marking appears to read ESP-WROOM-32; micro-USB, BOOT/RST buttons, pin labels and attached wiring visible | Supports the classic ESP32/WROOM family as a reuse lead. Does not establish an official DevKitC V4, WROOM-32E, board schematic, firmware, spare UARTs or complete GPIO assignments |
| P4 | Wider view of the board, green enclosure and partly covered cable assembly | Supports assembly context; covered joins and off-frame endpoints prevent an end-to-end wiring map. Wire colour alone does not establish function or voltage |

| Segment | Supported map | Unresolved endpoint / function |
| --- | --- | --- |
| Enclosure assembly | P2–P4 show the ESP32 board, stripboard, UBEC module and wiring as parts of the photographed assembly | Exact interconnections, supply entry, regulator output destination and any concealed interface circuits |
| External harness | P3/P4 show conductors from the harness reaching the board area; sections are covered | Connector contacts, GPIOs, serial directions and which branch serves Zander or FLARM |
| RS-422 module | P1 shows wires attached at its logic-side header and four labelled field terminals | Far ends of those wires, field-side cable, radio connection and relationship to the enclosure assembly |
| Navigation over Wi-Fi | S21 reports separate Zander and FLARM TCP paths into OpenSoar | Live endpoint mapping, UART settings and return traffic remain unverified |
