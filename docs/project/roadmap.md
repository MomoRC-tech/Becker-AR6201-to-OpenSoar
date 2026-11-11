# Roadmap

[← Project overview](../../README.md)

This is the single work queue. Current focus: hardware selection and wiring (WP3); software packages wait until it is finished. Implementation requires separate approval.

## Work queue

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

## Milestones

| Milestone | Gate | Status |
| --- | --- | --- |
| M1 | Installation/interfaces understood | Open |
| M2 | Design and driver plan selected | Open |
| M3 | Software verified | Open |

---

## Current state

| Package | State | Next |
| --- | --- | --- |
| WP1 | First priority. Photos (S13) and equipment report (S21) assessed in the [2026-11-09 review](../research/reviews/2026-11-09-installation-claims.md). No Becker integration established | Close the [installed-equipment items](open-items.md#installed-equipment-wp1); record values in the [existing installation](existing-installation.md) record and sources. Keep backups private |
| WP2 | [Driver research](../research/becker-driver.md) complete (S11): implementation comparisons and protocol leads | Matched radio identity and protocol reference, or independent captures |
| WP3 | **Current focus.** Hardware decisions recorded 2026-11-11: [wiring](../hardware/wiring.md), [modules and parts](../hardware/modules.md#parts-list) | Close the [bench hardware checks](open-items.md#bench-hardware-wp3); then build and bring up the bench bridge |
| WP4 / WP5 | [Send truncation reproduced on host](../research/reviews/2026-11-08-becker-send.md) (S12); byte-span correction passes synthetic cases | Subject to implementation approval: counted-send patch and integration test; caller/state/UI mapping. Active-state freshness and real readback remain open |

---

## Verification coverage (WP5, WP6)

| Layer | Required checks |
| --- | --- |
| Driver/protocol | Independent framing/CRC/channel vectors; zero/A5; short/invalid frames; unknown/stale state; local tuning; separate instances; reconnect; documented responses |
| Bridge | Exact bytes/order; split/coalesced reads; bursts; slow clients; partial writes; disconnect; ownership; boot/debug isolation; concurrent navigation |

> [!IMPORTANT]
> Verify radio acceptance and response bytes independently. Use WP1/WP2 evidence for traffic, gap and timeout limits.
