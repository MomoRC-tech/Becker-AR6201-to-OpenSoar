# Architecture

## Responsibilities and routing

| Component | Responsibility |
| --- | --- |
| OpenSoar | Radio framing, channel conversion, state, responses |
| ESP32 | Counted binary forwarding; separate radio/navigation/diagnostic paths |
| Navigation | Preserve required messages, displays and return traffic |
| Radio interface | Proposed 3.3 V UART ↔ full-duplex RS-422; no intermediate RS-232 stage required by the proposal |

[Overview](../README.md#architecture-overview) · [Connections](connections.md)

## Hardware proposal — one ESP32

| Item | Selection / status |
| --- | --- |
| Board | AZ-Delivery ESP32 WROOM-32 family; exact model/regulator/header TBD |
| Navigation | 2 × HW-044 at 3.3 V; continuity and interface checks pending |
| Radio | THVD1451D/SOIC breakout proposed; no galvanic isolation |
| Supply | Nominal 12 V battery → BEC (reported 2S–4S input), 5 V → board → 3.3 V converters |
| USB | Disconnect BEC 5 V before USB powering; no added power-selection circuit |
| Routing | Separate UART/device endpoints proposed; return traffic and GPS priority TBD |
| Firmware | Unselected; Yuri first external candidate, AlphaLima basic comparator |
| OpenSoar review baseline | 7.46.26, `b2e745cfddfe0150f6ed7db467ac18c321b38d01`; Becker WIP (S09); installed build unknown |

### Pinning and cabling

[Connection tables](connections.md) own GPIOs, equipment pins, power and cabling.

### OpenSoar baseline and pending inputs

Reported equipment/endpoints: [WP1 inventory](project-baseline.md#existing-installation--wp1-record). Tom: confirm radio type plate/HW/SW, Classic FLARM port/firmware, ZS1 version/cable/conversion map, serial settings, return traffic and UBEC output. Owner: separate new-build board/BEC details and installed OpenSoar configuration. Reports do not select new-build network/serial settings.

## Decision record

Selection ≠ implementation ≠ physical acceptance. IDs retained for references.

### ADR-00 — repository as project record

One queue: [roadmap](roadmap.md). Requirements: baseline; evidence: sources; design: architecture/connections.

### ADR-01 — separate transparent radio path

**Proposed:** one bidirectional radio endpoint; protocol/state in OpenSoar. Selection requires equipment interfaces and routing/resource/license review (WP1–WP3).

### ADR-02 — implementation and reuse baseline

**Open:** firmware selection. [Yuri/basic comparison](reviews/2026-11-08-yuri-bridge.md): Yuri needs exclusive radio ownership, diagnostic isolation and checked bounded writes. DroneBridge remains an alternative; multi-UART capability unverified. Review compatibility, file licenses and reproducible build inputs before reuse. Plan targeted OpenSoar fixes under WP4.

### ADR-03 — new hardware with the existing setup as backup

**Selected:** new bridge, one ESP32, separate interfaces/power/harness. Preserve existing hardware, firmware and configuration; no harvesting or reflashing. Compatibility depends on equipment interfaces, not spare resources in the backup.

### ADR-04 — selected bench hardware and power path

**Selected:** board/module families and power path above. **Proposed:** THVD1451D, input fuse, reverse-polarity and transient/overvoltage protection. Exact parts, ratings and electrical acceptance open. [Bench package](bench-build.md).
