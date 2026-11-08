# Architecture and decisions

Updated 2026-11-08. **One ESP32 in new, separate hardware is accepted; the existing setup stays unchanged as backup (ADR-03).** The owner selected the AZ-Delivery WROOM-32 family, HW-044 navigation modules and BEC/board-regulator power path (ADR-04); exact variants, radio interface and detailed routing remain open. Tom’s photos partially identify the existing hardware; exact revisions, settings and electrical paths remain open. The inventory and [photo comparison](project-baseline.md#toms-photo-evidence--2026-11-08) belong in [Project baseline](project-baseline.md#existing-installation--wp1-record).

## Responsibilities and routing

The [README architecture overview](../README.md#architecture-overview) shows the system. The [connection diagram and per-device pin tables](connections.md) define the proposed hardware paths.

OpenSoar handles radio framing, channel conversion, responses and state. The ESP32 forwards counted binary bytes in both directions, including zero and control bytes, across arbitrary TCP read boundaries. It keeps radio traffic separate from navigation and diagnostics. Navigation routing must preserve the required Zander and FLARM data and any existing return traffic.

The conditional radio interface is 3.3 V UART to full-duplex RS-422. The LXNAV RS232-to-RS422 adapter is a product-specific reference; it does not establish a need for an intermediate RS-232 stage here. See the [manual findings](technical-investigation.md#follow-up-source-verification-2026-11-08).

## Hardware proposal — one ESP32

The original Espressif ESP32-DevKitC V4/WROOM-32E recommendation (S09) is superseded by the owner's **AZ-Delivery ESP32 WROOM-32** selection (S16). The exact board model/regulator/header layout still needs identification; a vendor Dev Kit C V4 page is a reference, not confirmation of the owner's variant. Preserve the classic ESP32 GPIO allocation as a proposal and verify the exposed pins on the chosen board. The [bench package](connections.md#bench-build-package--2026-11-08) uses GPIO names and verified converter functions rather than assuming header positions.

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
- Choice: one bidirectional radio endpoint, isolated from navigation routing; keep protocol/state handling in OpenSoar. Select firmware for the new bridge against R02/R03, preserving the original setup as backup under ADR-03.
- Alternatives: a separate ESP32 dedicated to the radio (historical alternative, outside the current one-board constraint); adapt a reviewed small bridge; use a larger configurable bridge; implement radio protocol/state in bridge firmware. The last option expands responsibilities and duplicates driver work.
- Tradeoff: the new bridge needs its own verified UART allocation and reliable buffering. The separate backup preserves the original setup; it does not establish coexistence or radio acceptance on the new bridge.
- Decision gate: WP1 inventory plus WP2 interface evidence and WP3 routing/resource/license comparison. Record selected option and reasons before implementing it.

### ADR-02 — implementation and reuse baseline

- Status: **proposed**, selection open.
- Bounded source evaluation completed 2026-11-08: [Yuri versus basic AlphaLima](reviews/2026-11-08-yuri-bridge.md). Yuri remains the first external candidate based on bounded reads and routing; it still needs radio-only ownership, isolated diagnostics and checked bounded forwarding. Compatibility with the instrument requirements and firmware selection remain open.
- Use pinned OpenSoaring source S01/S02 as the review baseline; verify the installed build separately. Investigate targeted upstream driver fixes before creating a permanent application fork.
- Candidate evaluation order for the new hardware: Yuri-rage fork as the first small external bridge candidate; basic AlphaLima for comparison; DroneBridge as a dedicated-link alternative. Installed firmware, if identified, may inform compatibility or be evaluated for a separate new build; adapting the backup in place is superseded by ADR-03. This remains a research recommendation, not a tested ranking. Pro, XCVario and historical drivers remain references in [Related projects](related-projects.md).
- Decision gate: WP3 chooses firmware only after compatibility, required fixes, licenses and reproducibility are documented. WP4 records the minimal driver change plan. No replacement or code copying is approved by listing a candidate.

### ADR-03 — new hardware with the existing setup as backup

- Status: **accepted build strategy**, 2026-11-08, by the owner's explicit instruction (S15). Hardware is not yet built or verified.
- Decision: build a new, separate bridge with one ESP32 and its own interfaces and power circuitry. Preserve the existing assembly, firmware, configuration and harness as the backup; do not harvest its components or reflash it for development.
- Supersedes: the S13 photo assessment's hardware-reuse recommendation and the preference to adapt the installed bridge in place. Reuse of suitable software and architecture remains available for the new build.
- Consequence: the old board's spare UARTs and exact internal GPIO allocation no longer constrain the new design. Instrument identity, external connectors/signals, serial settings, navigation return paths, OpenSoar configuration and supply requirements still determine compatibility. Photos remain evidence about the backup and its interfaces.
- Next: resolve those equipment interfaces and specify the new board, converters, power circuitry and connection schedule. Final component selection, physical assembly, bench acceptance and later aircraft integration remain distinct steps; the backup's reported operation does not validate the new hardware.

Verification for this decision increment: checked README, inventory, assumptions, active architecture/reuse guidance, connection scope, sources and roadmap for consistency with S15; checked relative links/anchors and whitespace. Historical photo findings remain labelled, with superseded reuse guidance linked here. Documentation only; no purchases, builds, flashing, wiring or device tests performed.

### ADR-04 — selected bench hardware and power path

- Status: **owner-selected families and power topology**, 2026-11-08 (S16); not electrically verified. The user requested the bench schematic/parts/connection package and then selected AZ-Delivery WROOM-32, the pictured HW-044 modules, a model-aircraft 5 V BEC and board-derived 3.3 V.
- Decision: use two new HW-044 modules for the proposed navigation paths, subject to module/interface checks. The owner reports a nominal 12 V aircraft battery feeding a BEC rated for 2S–4S input. Power the board from the BEC's 5 V output and converters from its 3.3 V regulator. Preserve ADR-03's backup intact. The owner will manually prevent simultaneous USB/BEC powering; no extra USB power-selection or back-feed circuitry is planned.
- Proposed engineering details: THVD1451D on a SOIC breakout for the isolated bench radio path; input fuse and reverse-polarity protection; transient/output-overvoltage protection selected from the actual source/BEC/board limits. The radio data path is separate; the proposed transceiver is not galvanically isolated.
- Consequence: the [connection document](connections.md#bench-build-package--2026-11-08) owns the BOM, schematic, continuity checks, power budget and one verification summary. Unknown protection values and module/connector mappings remain explicit. This selection does not approve substitutions, establish aircraft suitability or close WP1/WP2/WP7.
