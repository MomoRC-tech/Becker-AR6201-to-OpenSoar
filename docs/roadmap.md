# Roadmap

Single work queue. Listed work is not implementation authorization.

## Work queue and milestones

| Package | Status | Prerequisite | Output / completion check |
| --- | --- | --- | --- |
| WP0 Documentation | Complete; acceptance pending | Existing records | Concise, consistent docs; [verification](reviews/2026-11-08-documentation.md) |
| WP1 Installation | Open; photos and equipment report assessed | Equipment/configuration evidence | Inventory, connector map, navigation baseline; gaps with owner/consequence |
| WP2 Interfaces/protocol | Open; driver research complete | Radio identity | Matched manual, settings, framing, response rules, independent vectors |
| WP3 Architecture/reuse | Open; bench design documented | WP1/WP2 | Board/interfaces, firmware, required fixes, licenses, build inputs |
| WP4 Driver plan | Planned; send investigation complete | WP1–WP3 | One minimal change and regression check per increment |
| WP5 Host verification | Partial: send regression only | WP2 expectations; approved WP4 scope | Framing, state, response and reconnect cases |
| WP6 Bridge | Planned | Selected WP3 design; WP5 | Isolated bidirectional radio path; exact bytes, bounded/checked writes, single owner, navigation coexistence |
| WP8 Aircraft integration | Deferred | Verified hardware and separate authorization | Installation, protection and approval requirements |

| Milestone | Gate | Status |
| --- | --- | --- |
| M1 | Installation/interfaces understood | Open |
| M2 | Design and driver plan selected | Open |
| M3 | Software verified | Open |

---

## Bounded WP3 bench hardware package — 2026-11-08

[Wiring and parts](connections.md#parts-list). Open: ESP32 header/regulator checks, BEC/source limits, H-1-0290 continuity, RS-422 logic compatibility and termination (S28–S29), power budget and protection values. Hold harness/protection purchases.

---

## Bounded WP3 RS-232 board selection — 2026-11-09

Selected: 2 × Hailege H-1-0290 for Zander/FLARM; replaces HW-044. [S26](sources.md) records supplied specifications; [converter reference](connections.md#hailege-board-pad-reference) uses text dimensions **9.4 × 15.9 mm**. Board acceptance, pad mapping, equipment connectors and enclosure clearance remain open.

Verification: selection/specifications and affected wiring/preview labels checked for consistency; 121 relative file/anchor references, three SVG structures and whitespace checked; updated diagrams visually checked in local renders. Documentation only; no build, electrical or device tests. WP3/M2 remain open.

---

## Bounded WP3 Hailege pinout — 2026-11-09

[Connections](connections.md#hailege-board-pad-reference): supplied board image, inferred eight-pad map, conditional Zander/Classic FLARM connections and proposed ESP32 GPIOs. [S27](sources.md) supplier comparator; no Hailege-specific schematic or measured sample match. Next: confirm IC/pad continuity on both boards, equipment cable/view and signal-reference compatibility.

Verification: S27 schematic/photos cross-checked with supplied image; ZS1 pp.46/48 and Classic FLARM pp.6/7 pin references rechecked. 97 relative file/anchor references and whitespace checked; pad-table directions compared with the reference schematic/photos. Supplied image preserved byte-for-byte. No firmware, electrical or device tests; WP3/M2 remain open.

---

## Bounded WP4/WP5 send investigation — 2026-11-08

[S12](reviews/2026-11-08-becker-send.md): host reproduction of truncated writes; byte-span correction passes synthetic cases. Next, subject to implementation approval: counted-send patch/integration test, caller/state/UI mapping. Active-state freshness and real readback remain open.

---

## Bounded WP2 driver research — complete, 2026-11-08

[S11](technical-investigation.md#becker-driver-follow-up--2026-11-08): implementation comparisons and protocol leads. Next: matched radio identity/protocol reference or independent captures. WP2 remains open.

---

## WP1 — existing installation (first priority)

[2026-11-09 report verification](reviews/2026-11-09-installation-claims.md): supplied Zander PDFs retained; Classic FLARM references, driver field support and reported endpoints recorded. No Becker integration established. Source review complete for this increment; WP1/WP2/M1 remain open.

| Needed | Owner | Blocks |
| --- | --- | --- |
| Confirm AR6201-(022): type plate/article/HW/SW, connector occupancy | Tom | Manual/interface matching |
| Confirm Classic FLARM F6/IGC and ZS1 version; actual port/cable/conversion map; baud/output menus | Tom | Equipment wiring; resolve conjectured absence of RS-232 conversion |
| Navigation routing/returns, update gaps, reconnect behavior | Tom/owner | Coexistence baseline |
| OpenSoar build and evidence for reported drivers/endpoints; clarify OpenVario's role | Owner/Tom | App/bridge configuration |
| Existing board/firmware, UBEC output/board input; separate new-build BEC/source limits | Tom/owner | Backup record and protection/current budget |

Record supported values and gaps in the [inventory](project-baseline.md#existing-installation--wp1-record) and sources. Keep backups private. Scope is evidence review. M1 requires closure of interface-critical gaps.

---

## One-package working loop

Read relevant sources/status → bound one change → execute → verify → update owning document → review/stage task files → commit/push.

---

## Verification coverage for later packages

| Layer | Required checks |
| --- | --- |
| Driver/protocol | Independent framing/CRC/channel vectors; zero/A5; short/invalid frames; unknown/stale state; local tuning; separate instances; reconnect; documented responses |
| Bridge | Exact bytes/order; split/coalesced reads; bursts; slow clients; partial writes; disconnect; ownership; boot/debug isolation; concurrent navigation |

Transport success ≠ radio acceptance. ACK constants do not establish response bytes. Traffic/gap/timeout limits require WP1/WP2 evidence.

---

## Documentation readability review — 2026-11-08

[Verification](reviews/2026-11-08-documentation.md). Documentation only; no application/device tests.

---

## Bounded WP0 Mermaid overview correction — 2026-11-09

[Connections guide](connections.md#logical-connections): separate forward and return arrows, compact Markdown tables, cropped Hailege image and shorter connector notes. Checked local Mermaid default and Dagre layouts. Live GitHub preview at `4ace6ba` confirmed 14 separate arrows, three devices on the left, cropped image, warning callout and section dividers. GitHub controls native table padding.
