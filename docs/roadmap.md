# Roadmap

> [!IMPORTANT]
> This is the single work queue. Implementation requires separate approval.

## Work queue and milestones

| Package | Status | Prerequisite | Output / completion check |
| --- | --- | --- | --- |
| WP0 Documentation | Complete; acceptance pending | Existing records | Current requirements, design, evidence and work queue |
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

## WP3 — bench hardware

[Wiring and parts](connections.md#parts-list). [ESP32 header reference](module-specs.md#esp32-pinout) checked against the DevKitC V4 guide. Open checks cover physical board match and regulator capacity, BEC/source limits, H-1-0290 continuity, 3.3 V full-duplex RS-422 board selection and electrical checks, power budget and protection values.

The Becker board requires a 3.3 V supply and ESP32-compatible UART logic. Model and 3.3 V rail remain open.

The [Hailege pin map](module-specs.md#hailege-board-pad-reference) is inferred from the supplied image and S27. ZS1 D3029 pp.46/48 and Classic FLARM v221 pp.6/7 were rechecked. Confirm pad continuity on both boards, enclosure clearance, equipment connector views and signal references. Electrical and device checks remain open.

The [power diagram](connections.md#power) shows F1 near the battery. [Sizing inputs](connections.md#f1-sizing-inputs) are open. A separate reverse-polarity stage depends on the BEC's documented input protection.

> [!IMPORTANT]
> Hold harness and protection purchases until the wiring and power checks are complete.

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

> [!IMPORTANT]
> Verify radio acceptance and response bytes independently. Use WP1/WP2 evidence for traffic, gap and timeout limits.
