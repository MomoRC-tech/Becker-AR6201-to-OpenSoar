# Architecture

## Responsibilities and routing

| Component | Responsibility |
| --- | --- |
| OpenSoar | Radio framing, channel conversion, state, responses |
| ESP32 | Counted binary forwarding; separate radio/navigation/diagnostic paths |
| Navigation | Preserve required messages, displays and return traffic |
| Radio interface | ESP32 UART ↔ assembled RS422-to-TTL board. Logic compatibility and module pin functions open |

[Overview](../README.md#architecture-overview) · [Connections](connections.md)

---

## Hardware proposal — one ESP32

| Item | Selection / status |
| --- | --- |
| Board | AZ-Delivery ESP32 WROOM dev module (owner selection). Verify header positions and regulator limits |
| Navigation | 2 × Hailege H-1-0290 at 3.3 V (S26); continuity and interface checks pending |
| Radio | Owner-selected assembled RS422-to-TTL board (S28). 5 V supply per S29. MAX490ESA owner confirmed (S30). RXD level conversion required for the documented circuit. TXD compatibility and isolation open |
| Supply | Nominal 12 V battery → BEC (reported 2S–4S input), 5 V → ESP32 → 3.3 V navigation converters. Becker module from the protected 5 V rail |
| USB | Disconnect BEC 5 V before USB powering; no added power-selection circuit |
| Routing | Separate UART/device endpoints proposed; return traffic and GPS priority TBD |
| Firmware | Unselected; Yuri first external candidate, AlphaLima basic comparator |
| OpenSoar review baseline | 7.46.26, `b2e745cfddfe0150f6ed7db467ac18c321b38d01`; Becker WIP (S09); installed build unknown |

### Pinning and cabling

[Connection tables](connections.md) own GPIOs, equipment pins, power and cabling.

### OpenSoar baseline and pending inputs

[WP1 inventory](project-baseline.md#existing-installation--wp1-record) records reported equipment and endpoints. [Open checks and owners](roadmap.md#wp1--existing-installation-first-priority). New-build network and serial settings remain open.

---

## Decision record

Selection ≠ implementation ≠ physical acceptance. IDs retained for references.

### ADR-00 — repository as project record

One queue: [roadmap](roadmap.md). Requirements: baseline; evidence: sources; design: architecture/connections.

### ADR-01 — separate transparent radio path

**Proposed:** one bidirectional radio endpoint; protocol/state in OpenSoar. Selection requires equipment interfaces and routing/resource/license review (WP1–WP3).

### ADR-02 — implementation and reuse baseline

**Open:** firmware selection. [Yuri/basic comparison](reviews/2026-11-08-yuri-bridge.md): Yuri needs exclusive radio ownership, diagnostic isolation and checked bounded writes. DroneBridge remains an alternative; multi-UART capability unverified. Review compatibility, file licenses and reproducible build inputs before reuse. Plan targeted OpenSoar fixes under WP4.

### ADR-03 — new hardware with the existing setup as backup

**Selected:** one ESP32 with separate interfaces, power and harness. Keep the existing hardware, firmware and configuration as backup. Match equipment interfaces before building.

### ADR-04 — selected bench hardware and power path

**Selected:** boards and navigation power path above. The assembled RS-422 module supersedes the bare THVD1451D proposal. **Proposed:** input fuse, reverse-polarity and transient/overvoltage protection. Exact parts, ratings and electrical acceptance open. [Wiring and parts](connections.md#parts-list).
