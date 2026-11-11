<a id="top"></a>

# 🎯 Requirements

🏠 [Overview](../../README.md) › 🧩 Design › **Requirements**

What the bridge must do, what the plan assumes, and what could go wrong.

> [!NOTE]
> **Status:** requirements agreed; none is verified on hardware yet. P0 = must have, P1 = should have.

📑 **Contents**

1. [Requirements](#requirements)
2. [Assumptions](#assumptions)
3. [Risks](#risks)

Related: 🏗️ [Architecture](architecture.md) · ❓ [Open items](../project/open-items.md) · 🗺️ [Roadmap](../project/roadmap.md)

---

<a id="requirements"></a>

## 1. ✅ Requirements

| ID | Prio | The bridge must… | How it is checked |
| --- | :-: | --- | --- |
| R01 | P0 | Put the selected airfield frequency into standby and never change the active frequency. | Readback from the real radio |
| R02 | P0 | Keep Zander and FLARM data reaching OpenSoar. | Compare with today's installation, then test both together with the radio path (WP6) |
| R03 | P0 | Pass radio bytes both ways exactly — no truncation, conversion, filtering or mixing with other data. | Byte-exact tests including zero/A5 bytes, split and bursty traffic, slow readers (WP5/WP6) |
| R04 | P0 | Tell "sent", "accepted by the radio" and "confirmed state" apart, and report failures or unknown state. | Tests for valid, missing, late and malformed replies and disconnects (WP5); a successful write alone never passes |
| R05 | P0 | Leave the radio's own controls working when the bridge disconnects or loses power. | Test on the real radio |
| R06 | P0 | Match the electrical interface and serial settings of the installed equipment. | Matching manual, connector view and settings; no incompatible signal levels on ESP32 pins |
| R07 | P1 | Be reproducible and reuse existing work where possible. | Recorded versions and configuration; license check before reuse |
| R08 | P1 | Read radio state where supported and show what OpenSoar can display. | Driver and UI review (WP2/WP4) |
| R09 | P0 | Use a full-duplex RS-422 board for the Becker, with a safe signal level at the ESP32. | [Becker wiring](../hardware/wiring.md#becker) and bench check |

**Not planned:** active tuning, frequency swap, volume and squelch, Larus, general XCSoar support. Aircraft installation comes later. The repository license is still to be chosen.

---

<a id="assumptions"></a>

## 2. 🤔 Assumptions

Things the plan relies on but that are not yet confirmed.

| ID | Assumption | Confirmed by | What it blocks |
| --- | --- | --- | --- |
| A01 | The installed radio is an AR6201 variant that supports remote control. | Radio identity (WP1), matching manual (WP2) | Final wiring |
| A03 | The installed OpenSoar build supports the Becker driver over TCP alongside the navigation feeds. | Installed build (WP1), integration review (WP4) | End-to-end software choice |
| A04 | Radio status gives a fresh enough active frequency before a standby command. | Protocol evidence (WP2), state design (WP4) | Safe command design |
| A05 | A separate radio TCP endpoint works without disturbing the navigation feeds. | Topology (WP1), design comparison (WP3) | [ADR-01](architecture.md#adr-01) |
| A06 | The ESP32 can carry both navigation paths plus the radio path. | UART plan (done), coexistence tests (WP6) | Bridge acceptance |

---

<a id="risks"></a>

## 3. ⚠️ Risks

All risks are open.

| ID | Prio | Risk | How it is reduced |
| --- | :-: | --- | --- |
| K01 | P0 | The wrong manual or radio variant leads to incompatible wiring — Becker publishes separate manuals per hardware generation (S03). | Match model, revision and connector view before wiring |
| K02 | P0 | A standby command carries an unknown or stale active frequency (F04). | Explicit state validity in the driver; startup, reconnect and local-tuning tests |
| K03 | P0 | Binary writes are truncated or report false success (F02/F03). | Driver fix and regression tests (WP4/WP5) |
| K04 | P0 | Bridge overflow, several writers or debug output corrupt radio traffic (S05/S06). | Firmware audit, byte-exact load and failure tests, separate radio path |
| K05 | P0 | The new bridge interrupts navigation data. | Compare with today's installation; coexistence tests (WP6) |
| K06 | P0 | A simulator only repeats the driver's own assumptions. | Independent protocol evidence; label synthetic tests; simulation is never acceptance |
| K07 | P1 | Licenses or dependencies prevent reuse. | Check license files and build dependencies; no code copied without permission |
| K08 | P1 | Switching noise from the UBEC or ESP32 is heard in the VHF radio or opens its squelch. | Bench and aircraft check with the bridge on and off ([bench bring-up](../hardware/wiring.md#bring-up)) |

---

⬆️ [Back to top](#top) · 🏠 [Overview](../../README.md)
