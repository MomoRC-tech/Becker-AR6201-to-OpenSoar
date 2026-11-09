# Roadmap

Single work queue. Listed work is not implementation authorization.

## Work queue and milestones

| Package | Status | Prerequisite | Output / completion check |
| --- | --- | --- | --- |
| WP0 Documentation | Complete; acceptance pending | Existing records | Concise, consistent docs; [verification](reviews/2026-11-08-documentation.md) |
| WP1 Installation | Open; photos assessed | Equipment/configuration evidence | Inventory, connector map, navigation baseline; gaps with owner/consequence |
| WP2 Interfaces/protocol | Open; driver research complete | Radio identity | Matched manual, settings, framing, response rules, independent vectors |
| WP3 Architecture/reuse | Open; bench design documented | WP1/WP2 | Board/interfaces, firmware, required fixes, licenses, build inputs |
| WP4 Driver plan | Planned; send investigation complete | WP1–WP3 | One minimal change and regression check per increment |
| WP5 Host verification | Partial: send regression only | WP2 expectations; approved WP4 scope | Framing, state, response and reconnect cases |
| WP6 Bridge | Planned | Selected WP3 design; WP5 | Isolated bidirectional radio path; exact bytes, bounded/checked writes, single owner, navigation coexistence |
| WP7 Bench acceptance | Open | Verified interfaces/software; authorized bench work | Actual standby/readback, active unchanged, local controls, navigation, failure/reconnect |
| WP8 Aircraft integration | Deferred | WP7; separate authorization | Installation, protection and approval requirements |

| Milestone | Gate | Status |
| --- | --- | --- |
| M1 | Installation/interfaces understood | Open |
| M2 | Design and driver plan selected | Open |
| M3 | Software verified | Open |
| M4 | Physical bench accepted | Open |

## Bounded WP3 bench hardware package — 2026-11-08

[Wiring guide](connections.md): diagrams/pin tables. [Bench reference](bench-build.md): BOM, protection, checks. Next: exact board/regulator, BEC/source limits, HW-044 continuity map, power budget and protection values. Harness/protection BOM not purchase-ready.

## Bounded WP4/WP5 send investigation — 2026-11-08

[S12](reviews/2026-11-08-becker-send.md): host reproduction of truncated writes; byte-span correction passes synthetic cases. Next, subject to implementation approval: counted-send patch/integration test, caller/state/UI mapping. Active-state freshness and real readback remain open.

## Bounded WP2 driver research — complete, 2026-11-08

[S11](technical-investigation.md#becker-driver-follow-up--2026-11-08): implementation comparisons and protocol leads. Next: matched radio identity/protocol reference or independent captures. WP2 remains open.

## WP1 — existing installation (first priority)

| Needed | Owner | Blocks |
| --- | --- | --- |
| Radio model/part/HW/SW, connector occupancy | Tom | Manual/interface matching |
| FLARM model/ports; Zander cable map; levels/settings | Tom | Equipment wiring |
| Navigation routing/returns, update gaps, reconnect behavior | Tom/owner | Coexistence baseline |
| OpenSoar build, drivers, endpoints | Owner/Tom | App/bridge configuration |
| Board/BEC models, source range | Owner | Protection/current budget |

Update [inventory](project-baseline.md#existing-installation--wp1-record) and sources with supported values or explicit gaps. Keep backups private. No flashing, rewiring or radio commands. M1 requires closure of interface-critical gaps.

## One-package working loop

Read relevant sources/status → bound one change → execute → verify → update owning document → review/stage task files → commit/push.

## Verification coverage for later packages

| Layer | Required checks |
| --- | --- |
| Driver/protocol | Independent framing/CRC/channel vectors; zero/A5; short/invalid frames; unknown/stale state; local tuning; separate instances; reconnect; documented responses |
| Bridge | Exact bytes/order; split/coalesced reads; bursts; slow clients; partial writes; disconnect; ownership; boot/debug isolation; concurrent navigation |
| Bench | Actual standby/readback; active unchanged; local controls; navigation; connection/power failures |

Transport success ≠ radio acceptance. ACK constants do not establish response bytes. Traffic/gap/timeout limits require WP1/WP2 evidence. Any remaining P0 failure blocks bench acceptance.

## Documentation readability review — 2026-11-08

[Verification](reviews/2026-11-08-documentation.md). Documentation only; no application/device tests.
