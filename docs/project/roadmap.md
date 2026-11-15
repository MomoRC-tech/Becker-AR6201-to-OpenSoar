<a id="top"></a>

# 🗺️ Roadmap

🏠 [Overview](../../README.md) › 📋 Project › **Roadmap**

The single work queue: which work package comes next and what finishes it.

> [!NOTE]
> **Status:** WP3 continues with wiring and bench validation. Software packages (WP4–WP6) wait until WP3 is complete. Every implementation step needs the owner's approval.

📑 **On this page:** <kbd>[1 · 📦 Work packages](#work-packages)</kbd> <kbd>[2 · 🏁 Milestones](#milestones)</kbd> <kbd>[3 · ✅ Test coverage for the software phase](#test-coverage)</kbd>

Related: ❓ [Open items](open-items.md) · 🎯 [Requirements](../design/requirements.md)

---

<a id="work-packages"></a>

## 1. 📦 Work packages

| Package | Status | Next step | Done when |
| --- | --- | --- | --- |
| **WP3 Hardware** | 🟡 **Current focus** — [housing v16](../hardware/housing.md) accepted for the bench prototype (S93); wiring and validation remain | Start with [bench bring-up](../hardware/wiring.md#bring-up): verify the UBEC and identify the delivered boards before soldering. Run the [housing fit checks](open-items.md#housing) during assembly; fit the harness before cutting. | Wiring design complete, bench bridge built and all bench checks passed |
| WP1 Installed equipment | 🟡 Photos and equipment report reviewed ([review](../research/reviews/2026-11-09-installation-claims.md)) | Get the [installed-equipment facts](open-items.md#installed-equipment) from Tom | Inventory, connector map and navigation baseline complete |
| WP0 Documentation | ✅ Done | — | — |
| WP2 Interfaces and protocol | ⏸️ Driver research done ([Becker driver](../research/becker-driver.md)) | Matching radio protocol reference or independent captures | Manual, settings, framing and reply rules confirmed |
| WP4 Driver plan | ⏸️ Send defect analysed | After hardware: counted-send fix and caller/state/UI mapping | One small change and regression test per step |
| WP5 Host tests | ⏸️ Send regression only ([review](../research/reviews/2026-11-08-becker-send.md)) | After WP4 scope is approved | Framing, state, reply and reconnect cases pass |
| WP6 Bridge firmware | ⏸️ Planned | After WP3 and WP5, assess Wi-Fi link monitoring ([R10](../design/requirements.md#requirements)) during firmware planning | Radio path byte-exact, single owner, navigation unaffected. Verify link monitoring if included in the approved scope |
| WP8 Aircraft integration | ⬜ Later | Needs verified hardware and separate approval | Installation, protection and approval requirements met |

<a id="claude-handover"></a>

### 1.1 Claude handover and recovery

| Restore point | How to reopen |
| --- | --- |
| Claude handover, published Step 4 | Git commit [`7207a63`](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/commit/7207a63), including the [Step 4 F3D archive at that commit](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/blob/7207a63d5f699fc7f18654ec6b911e9aa324b8e3/cad/housing/housing-body-v1/housing-power-routing.f3d). In Fusion, open version 10 of `Housing body - revised power routing` from its version history. |
| Step 5 continuation draft | Open version 11 of the same Fusion document. It includes Claude's unsaved Step 5 geometry and Codex's four routing corrections. The exact unsaved Claude draft had no separate saved version. |

Work loop for each step:

```mermaid
flowchart LR
    A[Read the current state] --> B[Choose one small change]
    B --> C[Do it]
    C --> D[Check it]
    D --> E[Update the page that owns it]
    E --> F[Commit and push]
    F -.-> A
```

---

<a id="milestones"></a>

## 2. 🏁 Milestones

| Milestone | Reached when | Status |
| --- | --- | --- |
| M1 | Installed equipment and interfaces are understood | ⬜ Open |
| M2 | Hardware design and driver plan are selected | 🟡 Mechanical design accepted; WP3 wiring and bench validation in progress |
| M3 | Bench ESP32 updatable over Wi-Fi: OTA loader flashed over USB, joins the configured network or opens its hotspot, and accepts an OTA upload in both modes ([ADR-05](../design/architecture.md#adr-05), [bring-up](../hardware/wiring.md#bring-up) step 3) | ⬜ Open |
| M4 | Software verified | ⬜ Open |

---

<a id="test-coverage"></a>

## 3. ✅ Test coverage for the software phase

| Layer | Tests |
| --- | --- |
| Driver and protocol | Independent framing, CRC and channel vectors; zero and A5 bytes; short or invalid frames; unknown or stale state; local tuning; two instances; reconnect; documented replies |
| Bridge | Exact bytes and order; split and merged reads; bursts; slow clients; partial writes; disconnects; single writer; boot and debug output kept away from devices; navigation in parallel |

Radio acceptance and reply bytes are checked on the real radio, not inferred from a successful write.

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
