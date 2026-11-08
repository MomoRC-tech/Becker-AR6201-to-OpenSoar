# Architecture and decisions

Updated 2026-11-08. **One ESP32 accepted as an owner constraint; board, interfaces and detailed routing remain proposed.** The actual installation is still unknown; its inventory belongs in [Project baseline](project-baseline.md#existing-installation--wp1-record).

## Responsibilities and routing

The [README architecture overview](../README.md#architecture-overview) shows the system. The [connection diagram and per-device pin tables](connections.md) define the proposed hardware paths.

OpenSoar handles radio framing, channel conversion, responses and state. The ESP32 forwards counted binary bytes in both directions, including zero and control bytes, across arbitrary TCP read boundaries. It keeps radio traffic separate from navigation and diagnostics. Navigation routing must preserve the required Zander and FLARM data and any existing return traffic.

The conditional radio interface is 3.3 V UART to full-duplex RS-422. The LXNAV RS232-to-RS422 adapter is a product-specific reference; it does not establish a need for an intermediate RS-232 stage here. See the [manual findings](technical-investigation.md#follow-up-source-verification-2026-11-08).

## Hardware proposal — one ESP32

Proposed 2026-11-08, based on S08/S09. No purchase, wiring or firmware changes authorized. Recommend **Espressif ESP32-DevKitC V4 with ESP32-WROOM-32E** as a bench development board: it stays close to the classic ESP32 targeted by both bridge sketches and provides three hardware UARTs. Use the exact WROOM board variant; WROVER reserves GPIO16/17 internally. This development board is not an accepted aircraft installation design.

Prefer separate Zander and FLARM serial inputs and a bidirectional radio UART, with separate network device connections initially. This avoids a new NMEA multiplexer. Existing combined routing may be retained if Tom's evidence shows it is needed and preserves all required messages. OpenSoar's simultaneous device configuration and GPS-source priority still need verification.

### Pinning and cabling

See [Pinning, cabling and routing](connections.md) for the connection diagram, ESP32 allocation, separate Zander/FLARM/Becker pin tables, and power/cable proposals. That document owns the connection details so they are maintained in one place.

### OpenSoar baseline and pending inputs

Use latest released OpenSoar as the owner's planning assumption, pinned for this review to **7.46.26 (2026-10-30), b2e745cfddfe0150f6ed7db467ac18c321b38d01**. The release page lists Android builds and labels Becker support WIP. No installation/update was performed; newer releases must be reviewed rather than silently replacing the pinned baseline.

Tom owns the pending equipment information: exact radio model/part and HW/SW revisions, FLARM model and available ports, Zander cable/connector mapping, existing signal paths and any return traffic, power source/voltage, and occupied connections. No deadline assumed. Until available, these proposals establish resource allocation and research leads, not final wiring.

Verification for this increment: reviewed S09 manufacturer board/UART documentation, ZS1 manual text, FLARM ICD section 5 and OpenSoar release index/notes; reused pinned S08 bridge findings and explicitly labelled prior Becker evidence. Checked document consistency, local references and whitespace. No build, firmware change, schematic electrical validation, board test or navigation/radio measurement. Goal agreement and one-board constraint are accepted; board, wiring, firmware and physical acceptance remain open. The added cable schedule was cross-checked against the existing S09 Zander/GPIO references and explicitly labelled prior Becker manual evidence; FLARM pins, cable specifications and final ground/power design remain unresolved.

## Decision record

Status vocabulary: **proposed → accepted → implemented → verified**, or **rejected/superseded**. Record decision date, owner authorization, evidence and replacement ID when status changes. Acceptance of a proposal is distinct from implementation authorization and bench acceptance.

### ADR-00 — repository as project record

- Status: **accepted for documentation workflow**, 2026-11-08, authorized by the repository-cleanup request.
- Decision: use the linked baseline, sources, architecture and roadmap as the shared record. Chat conclusions become project inputs only after evidence/status is recorded here. Keep one roadmap instead of a second backlog in chat or a new planning service.
- Consequence: each completed package updates only affected records and links its evidence. Existing detailed research remains available.

### ADR-01 — separate transparent radio path

- Status: **proposed**, not implemented. Owner accepted the one-ESP32 constraint on 2026-11-08; detailed interfaces and protocol responsibilities remain proposed.
- Choice: one bidirectional radio endpoint, isolated from existing navigation routing; keep protocol/state handling in OpenSoar. Prefer adapting the installed firmware if it meets R02/R03.
- Alternatives: a separate ESP32 dedicated to the radio (historical alternative, outside the current one-board constraint); adapt a reviewed small bridge; use a larger configurable bridge; implement radio protocol/state in bridge firmware. The last option expands responsibilities and duplicates driver work.
- Tradeoff: reuse can minimize hardware changes, but spare UARTs and reliable buffering are unknown. A dedicated bridge adds hardware but may isolate resource contention.
- Decision gate: WP1 inventory plus WP2 interface evidence and WP3 routing/resource/license comparison. Record selected option and reasons before implementing it.

### ADR-02 — implementation and reuse baseline

- Status: **proposed**, selection open.
- Bounded source evaluation completed 2026-11-08: [Yuri versus basic AlphaLima](reviews/2026-11-08-yuri-bridge.md). Yuri remains the first external candidate based on bounded reads and routing; it still needs radio-only ownership, isolated diagnostics and checked bounded forwarding. Installed firmware comparison and owner selection remain open.
- Use pinned OpenSoaring source S01/S02 as the review baseline; verify the installed build separately. Investigate targeted upstream driver fixes before creating a permanent application fork.
- Candidate evaluation order: installed firmware first; Yuri-rage fork as the first small external bridge candidate; basic AlphaLima for comparison; DroneBridge as a dedicated-link alternative. This is the earlier research recommendation, not a tested ranking. Pro, XCVario and historical drivers remain references in [Related projects](related-projects.md).
- Decision gate: WP3 chooses firmware only after compatibility, required fixes, licenses and reproducibility are documented. WP4 records the minimal driver change plan. No replacement or code copying is approved by listing a candidate.
