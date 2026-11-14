<a id="top"></a>

# 🗺️ Roadmap

🏠 [Overview](../../README.md) › 📋 Project › **Roadmap**

The single work queue: which work package comes next and what finishes it.

> [!NOTE]
> **Status:** WP3 hardware is the current focus. Software packages (WP4–WP6) wait until the hardware is finished. Every implementation step needs the owner's approval.

📑 **On this page:** <kbd>[1 · 📦 Work packages](#work-packages)</kbd> <kbd>[2 · 🏁 Milestones](#milestones)</kbd> <kbd>[3 · ✅ Test coverage for the software phase](#test-coverage)</kbd>

Related: ❓ [Open items](open-items.md) · 🎯 [Requirements](../design/requirements.md)

---

<a id="work-packages"></a>

## 1. 📦 Work packages

| Package | Status | Next step | Done when |
| --- | --- | --- | --- |
| **WP3 Hardware** | 🟡 **Current focus** — XT30, ferrite, RS-232 and MAX490 logic routes are modelled in the [routing study](../hardware/housing.md#internal-routing); the [routing review](../research/reviews/2026-11-14-wire-routing.md) decisions are made (levels and lanes, junctions, wire types) | Re-route the harness in CAD on the selected lanes, following steps 3–6 of the [routing review plan](../research/reviews/2026-11-14-wire-routing.md#plan): lanes component, power, signals and resistors, DE-9 ground and Becker pairs, then one harness table with cutting lengths. Then set terminations, bends and service slack, and choose the centre-floor guide and wire retention. Fit-print the cable saddles, [ferrite cradle](../hardware/housing.md#ferrite-cradle), [UBEC carrier](../hardware/housing.md#ubec-carrier) and base. Collect the remaining housing inputs and complete the [bench hardware checks](open-items.md#bench-hardware) | Wiring design complete, bench bridge built and all bench checks passed |
| WP1 Installed equipment | 🟡 Photos and equipment report reviewed ([review](../research/reviews/2026-11-09-installation-claims.md)) | Get the [installed-equipment facts](open-items.md#installed-equipment) from Tom | Inventory, connector map and navigation baseline complete |
| WP0 Documentation | ✅ Done | — | — |
| WP2 Interfaces and protocol | ⏸️ Driver research done ([Becker driver](../research/becker-driver.md)) | Matching radio protocol reference or independent captures | Manual, settings, framing and reply rules confirmed |
| WP4 Driver plan | ⏸️ Send defect analysed | After hardware: counted-send fix and caller/state/UI mapping | One small change and regression test per step |
| WP5 Host tests | ⏸️ Send regression only ([review](../research/reviews/2026-11-08-becker-send.md)) | After WP4 scope is approved | Framing, state, reply and reconnect cases pass |
| WP6 Bridge firmware | ⏸️ Planned | After WP3 and WP5, assess Wi-Fi link monitoring ([R10](../design/requirements.md#requirements)) during firmware planning | Radio path byte-exact, single owner, navigation unaffected. Verify link monitoring if included in the approved scope |
| WP8 Aircraft integration | ⬜ Later | Needs verified hardware and separate approval | Installation, protection and approval requirements met |

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
| M2 | Hardware design and driver plan are selected | 🟡 Hardware design in progress |
| M3 | Software verified | ⬜ Open |

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
