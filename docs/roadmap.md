# Roadmap and iterative workflow

Updated 2026-11-08. This is the single work queue. WP0 documentation cleanup, architecture/hardware clarification, the bounded WP3 source comparison and the Becker driver research below have been documented. Tom’s supplied photos have now been assessed as a bounded WP1 evidence increment. Firmware selection and implementation remain open; listed packages are not automatically authorized by this queue.

## Work queue and milestones

| Package | Priority / status | Depends on | Bounded output and completion check |
| --- | --- | --- | --- |
| WP0 Documentation baseline | P0 / complete; owner acceptance pending | Existing docs | Consistent document roles, traceable findings, work queue and instructions. [Review/evidence](reviews/2026-11-08-documentation.md). |
| WP1 Existing installation | P0 / in progress; photo increment complete, identity/configuration gaps remain | Owner-supplied equipment/configuration information | Complete inventory and as-built map, or explicit gaps with owner and consequence. [Photo assessment and verification](project-baseline.md#toms-photo-evidence--2026-11-08); detailed task below. |
| WP2 Interface and protocol evidence | P0 / open; bounded driver research complete | WP1 radio identity for hardware-specific conclusions | Match manufacturer manual to radio; record connector view, signals/settings, framing, response/status rules, channel conversion and independent vectors. [S11 driver research](technical-investigation.md#becker-driver-follow-up--2026-11-08) adds source leads, not independent protocol validation. Unknown rules remain open, never invented. |
| WP3 Architecture/reuse choice | P0 / planned | WP1 topology/resources; WP2 interface constraints | Compare installed firmware and short-listed alternatives against R02/R03/R07/R08. Update ADR-01/02 with choice, required changes, license evidence, build inputs and owner decision. |
| WP4 Driver change plan | P0 / planned | WP1 deployed build; WP2 protocol evidence; WP3 responsibility split | Reproduce/review each applicable source finding, map actual app state/transport/UI paths, then specify one minimal change and regression check per increment. No bundled driver rewrite. |
| WP5 Host verification | P0 / planned | WP2 independent expectations; accepted WP4 scope | First executable deliverable: a small host harness and one independently supported command/response case. Then separate increments for framing/length/CRC, unknown/stale state, responses and reconnect. Run relevant checks after each change. |
| WP6 Bridge increment | P0 / planned | Accepted WP3 choice; WP5 cases | One isolated bidirectional radio path with exact-byte tests, bounded buffers, checked writes, exclusive command ownership and isolated diagnostics. Verify coexistence against WP1 baseline. |
| WP7 Physical bench acceptance | P0 / planned | R01–R08 software evidence; verified interface/setup; authorized bench work | Observe standby change, unchanged active channel, actual response/readback, local controls and navigation under failure/reconnect. Record tested versions, limitations and owner acceptance. |
| WP8 Aircraft integration planning | Deferred | Successful WP7 and separate scope decision | Define installation/protection/compatibility and applicable approval prerequisites. No installed-aircraft modification is part of WP0–WP7. |

Milestones are evidence gates, not dates: **M1 understood installation** (WP1 + hardware-specific WP2); **M2 selected design** (WP3/WP4 and owner decision); **M3 software evidence** (WP5/WP6); **M4 bench evidence** (WP7). No runtime or hardware milestone is complete.

Public-source analysis can continue when an installed-system input is missing, but must not silently close the dependent package. Before starting WP2–WP7, choose one row or a smaller subtask with its own output and stop point.

## Bounded WP4/WP5 send investigation — 2026-11-08

Owner-requested historical comparison and host reproduction are complete. [Review S12](reviews/2026-11-08-becker-send.md) demonstrates binary truncation in OpenSoar 7.46.26 and tests a one-call byte-span correction. The committed harness is a transport regression, with radio protocol vectors, full application implementation and physical acceptance still open. Next driver increment is the counted-send patch and its integration test, subject to implementation authorization. State validity and real readback remain required for the end-to-end goal.

## Bounded WP2 driver research — complete, 2026-11-08

The owner's request to find useful Becker driver material produced the [S11 source comparison and verification summary](technical-investigation.md#becker-driver-follow-up--2026-11-08). It identifies useful parts of Werner86's PR 449 and LK8000, their remaining defects, the reported control-head reason for paired writes, and a manufacturer protocol-documentation contact lead. It does not complete WP2/WP4 or approve code reuse, implementation or radio commands.

Next evidence prerequisite: Tom's radio identity and installed configuration, then a matching protocol reference or independently captured behavior. Alongside the isolated send correction proposed in S12, a bounded WP4 follow-up is to map the pinned OpenSoar standby caller, receive-state lifecycle and displayed result before specifying state/response changes. Exact response semantics and active-channel preservation during local tuning remain unresolved.

## WP1 — existing installation (first priority)

Authorized WP3 subtask completed 2026-11-08: [Yuri/basic AlphaLima source and maintenance comparison](reviews/2026-11-08-yuri-bridge.md), including proposed minimal increments and one verification summary. This does not close WP3 or authorize firmware changes. Next prerequisite remains WP1: identify installed firmware/configuration and UART ownership before selecting a replacement.

**Question:** What is installed now, and which connections must the radio extension preserve?

Photo increment completed 2026-11-08: S13 records Tom’s four supplied images; S14 supplies a limited UBEC product-family reference. The [baseline](project-baseline.md#toms-photo-evidence--2026-11-08) now records visible hardware, a partial as-built connection map, proposal comparison, gap owners and one verification summary. The WROOM-family ESP32, RS422 TO TTL board and UBEC are reuse leads, not accepted components. Next: Tom to identify whether the converter is installed, its IC/product, and the current firmware/GPIO assignments. Radio revisions, navigation paths/settings, power details and live baseline remain unresolved; WP1/M1 are not closed.

Planning background: owner agreed the first-version goal and one ESP32 on 2026-11-08. README reflects that agreement. The [hardware proposal](architecture.md#hardware-proposal--one-esp32) records a bench board candidate, conditional GPIO allocation, interface research and the verification summary (S09). Tom will supply the remaining equipment/wiring/power information when available; navigation return traffic is unknown. Final board/interface/firmware selection remains open. The [connection diagram and per-device pinning/cabling tables](connections.md) remain proposals for comparison with Tom’s setup; retain working arrangements or propose justified improvements rather than treating either the current setup or this draft as fixed. Complete the comparison as missing evidence arrives, before final wiring or implementation.

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
6. **Commit and push:** review the diff and stage only relevant files. Make one coherent change per commit, check the remote head before pushing, and record the commit and push result in the task report. Preserve concurrent work.
7. **Close:** report what changed, verification and unresolved items. Continue within already authorized scope; a plan or recommendation alone does not authorize the next implementation package.

## Verification coverage for later packages

| Layer | Required cases / limit |
| --- | --- |
| Protocol and driver | Independently supported framing/CRC/channel vectors, embedded zero/A5, short/invalid frames, startup with unknown state, stale state after local tuning, separate instances and reconnect. Derive acknowledgment/rejection behavior from WP2; do not assume standalone 0x06/0x15 bytes because the driver defines those constants. |
| Transport and bridge | Exact bytes in both directions, fragmented/coalesced TCP reads, large bursts, slow clients, partial writes, disconnect, multiple-client policy, boot/debug isolation and simultaneous navigation. |
| Physical bench | Selected channel → actual standby display/readback; active unchanged; supported replies distinguished from writes; local controls usable; repeat with connection/power failures and navigation present. |

Synthetic checks are useful regression evidence but do not establish interoperability. Agree measurable traffic/gap/timeout limits from WP1/WP2 before running acceptance tests; do not invent universal thresholds. Any remaining P0 failure blocks bench acceptance.

## Documentation readability review — 2026-11-08

README now leads with the three-device integration goal and owns the single architecture overview. Architecture owns rationale and decisions; connections.md owns the detailed routing diagram and separate device pin tables. Agreement history remains in the baseline/decision records, outside the summary. Existing conditional pin references and unresolved choices are preserved. Verification: checked local links/anchors, signal IDs and diagram/table mapping, and whitespace; no new technical claims, builds or hardware tests. Mermaid source reviewed; rendered output not visually verified.
