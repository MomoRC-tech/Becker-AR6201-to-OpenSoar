# Roadmap and iterative workflow

Updated 2026-11-08. This is the single work queue. The current request authorizes documentation cleanup (WP0); subsequent analysis/implementation packages are planned, not automatically authorized by this list.

## Work queue and milestones

| Package | Priority / status | Depends on | Bounded output and completion check |
| --- | --- | --- | --- |
| WP0 Documentation baseline | P0 / complete; owner acceptance pending | Existing docs | Consistent document roles, traceable findings, work queue and instructions. [Review/evidence](reviews/2026-11-08-documentation.md). |
| WP1 Existing installation | P0 / next; evidence missing | Owner-supplied equipment/configuration information | Complete inventory and as-built map, or explicit gaps with owner and consequence. Detailed task below. |
| WP2 Interface and protocol evidence | P0 / planned | WP1 radio identity for hardware-specific conclusions | Match manufacturer manual to radio; record connector view, signals/settings, framing, response/status rules, channel conversion and independent vectors. Unknown rules remain open, never invented. |
| WP3 Architecture/reuse choice | P0 / planned | WP1 topology/resources; WP2 interface constraints | Compare installed firmware and short-listed alternatives against R02/R03/R07/R08. Update ADR-01/02 with choice, required changes, license evidence, build inputs and owner decision. |
| WP4 Driver change plan | P0 / planned | WP1 deployed build; WP2 protocol evidence; WP3 responsibility split | Reproduce/review each applicable source finding, map actual app state/transport/UI paths, then specify one minimal change and regression check per increment. No bundled driver rewrite. |
| WP5 Host verification | P0 / planned | WP2 independent expectations; accepted WP4 scope | First executable deliverable: a small host harness and one independently supported command/response case. Then separate increments for framing/length/CRC, unknown/stale state, responses and reconnect. Run relevant checks after each change. |
| WP6 Bridge increment | P0 / planned | Accepted WP3 choice; WP5 cases | One isolated bidirectional radio path with exact-byte tests, bounded buffers, checked writes, exclusive command ownership and isolated diagnostics. Verify coexistence against WP1 baseline. |
| WP7 Physical bench acceptance | P0 / planned | R01–R08 software evidence; verified interface/setup; authorized bench work | Observe standby change, unchanged active channel, actual response/readback, local controls and navigation under failure/reconnect. Record tested versions, limitations and owner acceptance. |
| WP8 Aircraft integration planning | Deferred | Successful WP7 and separate scope decision | Define installation/protection/compatibility and applicable approval prerequisites. No installed-aircraft modification is part of WP0–WP7. |

Milestones are evidence gates, not dates: **M1 understood installation** (WP1 + hardware-specific WP2); **M2 selected design** (WP3/WP4 and owner decision); **M3 software evidence** (WP5/WP6); **M4 bench evidence** (WP7). No runtime or hardware milestone is complete.

Public-source analysis can continue when an installed-system input is missing, but must not silently close the dependent package. Before starting WP2–WP7, choose one row or a smaller subtask with its own output and stop point.

## WP1 — existing installation (first priority)

Authorized WP3 subtask completed 2026-11-08: [Yuri/basic AlphaLima source and maintenance comparison](reviews/2026-11-08-yuri-bridge.md), including proposed minimal increments and one verification summary. This does not close WP3 or authorize firmware changes. Next prerequisite remains WP1: identify installed firmware/configuration and UART ownership before selecting a replacement.

**Question:** What is installed now, and which connections must the radio extension preserve?

**Inputs:** exact radio model/part and hardware/software revision; ESP32 board and existing firmware/configuration; OpenSoar version and device settings; known wiring/level converters; a non-invasive observation of current navigation reception. The [inventory](project-baseline.md#existing-installation--wp1-record) lists the missing fields.

**Steps:**

1. Read README, this task and the inventory; inspect only supplied artifacts relevant to the installation.
2. Record each device, UART owner, signal level and serial setting; map each configured network endpoint to a device/driver. Use sanitized aliases where needed. Mark unknown values explicitly.
3. Draw the as-built data paths from evidence, including whether navigation inputs are combined and whether the bridge has a return path.
4. Record current navigation behavior and the conditions of the observation. If no device observation is available, keep the baseline unmeasured.
5. List missing evidence with who can supply it and which next decision it blocks. Propose only the smallest next analysis task.

**Output:** update the existing inventory, assumptions/risks and relevant source entries; add the as-built map in project-baseline.md. Record one concise WP1 verification summary with evidence references. Keep raw private backups outside the public repository.

**Done when:** every inventory field has either a supported value or an explicit gap/owner/consequence; every known connection is mapped; measured versus reported behavior is clear; WP2 prerequisites are identified. A completed inventory with gaps does not mean M1 is reached. M1 requires the gaps needed for hardware/interface decisions to be closed.

**Scope boundary:** no firmware selection, new simulator, flashing, rewiring or radio commands. If physical evidence is unavailable, document the gap rather than substituting default pins/ports/settings.

**Copyable next task:**

> Carry out WP1 in docs/roadmap.md using the supplied installation evidence. Update docs/project-baseline.md and docs/sources.md, preserve unknowns, and add a sourced as-built connection map. Record missing evidence and its owner. Do not implement firmware, choose wiring or control the radio. Finish with the verification result, remaining blockers and smallest next task.

## One-package working loop

1. **Read:** current task, AGENTS.md, repository status and the few relevant sources. Preserve unrelated work.
2. **Bound:** state one question/change, affected requirement IDs, expected artifact and check. If the package is too broad, split it before implementation.
3. **Investigate or change:** use pinned sources and existing tools. Distinguish proposal, authorization, implementation and acceptance.
4. **Verify:** documentation gets reference/consistency checks; changed logic gets relevant automated cases; hardware claims require actual recorded observation.
5. **Record:** update the owning document/ADR/status and one evidence summary. Capture date, revisions, environment, input, expected/actual result and limitations. Link larger evidence under docs/reviews/ only when needed.
6. **Commit:** one coherent change per commit, review the diff and explicit file list, and record the resulting commit in the task report. Check the remote head before publication; never overwrite concurrent work.
7. **Close:** report what changed, verification and unresolved items. Continue within already authorized scope; a plan or recommendation alone does not authorize the next implementation package.

## Verification coverage for later packages

| Layer | Required cases / limit |
| --- | --- |
| Protocol and driver | Independently supported framing/CRC/channel vectors, embedded zero/A5, short/invalid frames, startup with unknown state, stale state after local tuning, separate instances and reconnect. Derive acknowledgment/rejection behavior from WP2; do not assume standalone 0x06/0x15 bytes because the driver defines those constants. |
| Transport and bridge | Exact bytes in both directions, fragmented/coalesced TCP reads, large bursts, slow clients, partial writes, disconnect, multiple-client policy, boot/debug isolation and simultaneous navigation. |
| Physical bench | Selected channel → actual standby display/readback; active unchanged; supported replies distinguished from writes; local controls usable; repeat with connection/power failures and navigation present. |

Synthetic checks are useful regression evidence but do not establish interoperability. Agree measurable traffic/gap/timeout limits from WP1/WP2 before running acceptance tests; do not invent universal thresholds. Any remaining P0 failure blocks bench acceptance.
