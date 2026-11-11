# Architecture

[← Project overview](../../README.md)

How the bridge is split up, which hardware is selected and the decisions behind it. Selections are design decisions; implementation and physical acceptance are tracked separately.

## Responsibilities and routing

| Component | Responsibility |
| --- | --- |
| OpenSoar | Radio framing, channel conversion, state, responses |
| ESP32 | Counted binary forwarding; separate radio/navigation/diagnostic paths |
| Navigation | Preserve required messages, displays and return traffic |
| Radio interface | ESP32 UART ↔ selected 5 V MAX490 full-duplex RS-422 board; [UART interface details and checks](../hardware/modules.md#becker-max490-board) |

[Wiring](../hardware/wiring.md) · [Modules and parts](../hardware/modules.md) · [Requirements](requirements.md)

---

## Hardware — one ESP32

| Item | Selection |
| --- | --- |
| Board | AZ-Delivery ESP32 WROOM dev module |
| Zander, FLARM | 2 × Hailege H-1-0290 RS-232 modules at 3.3 V |
| Becker | 5 V MAX490 RS-422 board; receiver output to the ESP32 through 10 kΩ |
| UARTs | UART0 (remapped) Zander, UART1 FLARM, UART2 Becker |
| Supply | 12 V battery → 2 A fuse → Hobbywing UBEC at 5 V → ESP32 and MAX490; ESP32 3.3 V → RS-232 modules |
| Firmware | Not selected (software waits for the hardware design); candidates in [bridge firmware](../research/bridge-firmware.md) |
| OpenSoar review baseline | 7.46.26, `b2e745cfddfe0150f6ed7db467ac18c321b38d01` (S09); installed build unknown |

Details: [wiring](../hardware/wiring.md) · [modules and parts](../hardware/modules.md).

The installed equipment and its reported endpoints are in [existing installation](../project/existing-installation.md).

---

## Decision record

### ADR-00 — repository as project record

One queue: [roadmap](../project/roadmap.md). Requirements: [requirements](requirements.md); missing evidence: [open items](../project/open-items.md); evidence: [sources](../research/sources.md); design: architecture/connections.

### ADR-01 — separate transparent radio path

**Proposed:** one bidirectional radio endpoint; protocol/state in OpenSoar. Selection requires equipment interfaces and routing/resource/license review (WP1–WP3).

### ADR-02 — implementation and reuse baseline

**Open:** firmware selection. [Yuri/basic comparison](../research/reviews/2026-11-08-yuri-bridge.md): Yuri needs exclusive radio ownership, diagnostic isolation and checked bounded writes. DroneBridge remains an alternative; multi-UART capability unverified. Review compatibility, file licenses and reproducible build inputs before reuse. Plan targeted OpenSoar fixes under WP4.

### ADR-03 — new hardware with the existing setup as backup

**Selected:** one ESP32 with separate interfaces, power and harness. Keep the existing hardware, firmware and configuration as backup. Match equipment interfaces before building.

### ADR-04 — selected bench hardware and power path

**Selected:** ESP32, two Hailege RS-232 modules and the Hobbywing UBEC at 5 V. Becker: 5 V MAX490 board, receiver output to GPIO16 through a 10 kΩ series resistor, J1-21 bonded to the board ground. UART plan: UART0 (remapped) Zander, UART1 FLARM, UART2 Becker. Power protection: 2 A fuse plus the UBEC's reverse-input protection. [Wiring and parts](../hardware/modules.md#parts-list).
