<a id="top"></a>

# 🎯 Requirements

🏠 [Overview](../../README.md) › 🧩 Design › **Requirements**

What the bridge must do, what the plan assumes, and what could go wrong.

> [!NOTE]
> **Status:** requirements agreed; none is verified on hardware yet. P0 = must have, P1 = should have.

<a href="#requirements"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-design-requirements-1-dark.svg"><img src="../images/nav/menu-design-requirements-1.svg" alt="1 · Requirements" height="30"></picture></a> <a href="#assumptions"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-design-requirements-2-dark.svg"><img src="../images/nav/menu-design-requirements-2.svg" alt="2 · Assumptions" height="30"></picture></a> <a href="#risks"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-design-requirements-3-dark.svg"><img src="../images/nav/menu-design-requirements-3.svg" alt="3 · Risks" height="30"></picture></a> <!-- page-menu -->

Related: 🏗️ [Architecture](architecture.md) · ❓ [Open items](../project/open-items.md) · 🗺️ [Roadmap](../project/roadmap.md)

---

<a id="requirements"></a>

## 1. ✅ Requirements

| ID | Prio | The bridge must… | How it is checked |
| --- | :-: | --- | --- |
| R01 | P0 | Put the selected airfield frequency into standby and never change the active frequency. Deferred pending [protocol evidence](../project/open-items.md#software-and-acceptance). | Readback from the real radio |
| R02 | P0 | Keep Zander and FLARM data reaching OpenSoar. | Compare with today's installation, then test both together with the radio path (WP6) |
| R03 | P0 | Pass radio bytes both ways exactly — no truncation, conversion, filtering or mixing with other data. | Byte-exact tests including zero/A5 bytes, split and bursty traffic, slow readers (WP5/WP6) |
| R04 | P0 | For future standby tuning, distinguish sending, radio acceptance and confirmed state, and report failures or unknown state. | Tests for valid, missing, late and malformed replies and disconnects |
| R05 | P0 | Leave the radio's own controls working when the bridge disconnects or loses power. | Test on the real radio |
| R06 | P0 | Match the electrical interface and serial settings of the installed equipment. | Matching manual, connector view and settings; no incompatible signal levels on ESP32 pins |
| R07 | P1 | Be reproducible and reuse existing work where possible. | Recorded versions and configuration; license check before reuse |
| R08 | P1 | Read radio state where supported and show what OpenSoar can display. | Driver and UI review (WP2/WP4) |
| R09 | P0 | Use a full-duplex RS-422 board for the Becker, with a safe signal level at the ESP32. | [Becker wiring](../hardware/wiring.md#becker) and bench check |
| R10 | P1 | Monitor the quality and stability of the Wi-Fi link to the phone from the ESP32 without interrupting bridge traffic. | In WP6, assess which metrics the firmware can observe (S55), how to report them and what counts as a useful reading. Verify on the bench with the phone during normal traffic and a deliberate disconnect and reconnect. |
| R11 | P0 | Restrain long internal wire runs against lifting, lateral motion and rubbing under vibration. Support branches near connectors, solder joints and the ferrite, with enough service slack to unload the terminations (S80). | Check the assembled harness with the actual wire and retainers for movement, pinch points, chafing and load at the terminations. Inspect and check continuity after a representative vibration test before aircraft integration ([housing item 20](../project/open-items.md#housing)) |
| R12 | P1 | Join the configured Wi-Fi network at start-up; if that fails, open its own password-protected hotspot ([ADR-05](architecture.md#adr-05)). | On the bench: start with the network available, then with it switched off; the bridge must be reachable in both cases |
| R13 | P1 | Accept firmware updates over Wi-Fi, so the housing stays closed. Every firmware image keeps this ability and the selected partition scheme ([ADR-05](architecture.md#adr-05)). | One OTA upload in each Wi-Fi mode of R12; repeat for every new firmware image |
| R14 | P1 | Show bridge, Wi-Fi and per-device status on a web page, without disturbing bridge traffic. | Open the page on the phone during the byte-exact tests of R03; the results must not change ([firmware plan](../project/roadmap.md#firmware-plan)) |
| R15 | P1 | Let the owner change serial, Wi-Fi, hotspot and port settings on a password-protected page without rebuilding the firmware. Stored settings survive restarts and OTA updates; a Wi-Fi, hotspot or port change that is not confirmed reverts by itself ([settings page](../project/roadmap.md#settings-page)). | Step 3a bench checks: change, restart, OTA update, unconfirmed change, wrong password, power loss during a save, reset |
| R16 | P1 | Show the data passing each device in readable form (text or hex, both directions), without disturbing bridge traffic. | Step 3b: looped data appears in the right port and direction; loop test passes with the monitor open ([data monitor](../project/roadmap.md#data-monitor)) |
| R17 | P1 | Show what the bridge does (start-up and restart reason, Wi-Fi, app connections, radio ownership, settings, updates, errors) on a web page, readable by kind and importance, exportable as text, and kept across a restart; never sent to a device port. | Step 3c: each kind of event appears with the right level and source; filters, Copy and Save work on the phone; the lines before a restart stay readable; loop test passes with the log open ([web log](../project/roadmap.md#web-log)) |
| R18 | P1 | Measure the bridge's 12 V supply and report it to OpenSoar and on the status page, with a chart of the minute values (mean, lowest, highest) over the time the bridge is on, up to 12 hours, without touching the forwarded device data ([ADR-07](architecture.md#adr-07)). | Step 3d: readings against a multimeter from 7 to 20 V; OpenSoar shows the value; the chart covers the time since start; loop test passes with the sensor port connected ([supply voltage](../project/roadmap.md#supply-voltage)) |

**Not planned:** active tuning, frequency swap, volume and squelch, Larus, general XCSoar support. Aircraft installation comes later. The repository license is still to be chosen.

---

<a id="assumptions"></a>

## 2. 🤔 Assumptions

Things the plan relies on but that are not yet confirmed.

| ID | Assumption | Confirmed by | What it blocks |
| --- | --- | --- | --- |
| A01 | The installed radio is an AR6201 variant that supports remote control. | Radio identity (WP1), matching manual (WP2) | Final wiring |
| A03 | The installed OpenSoar build supports the Becker driver over TCP alongside the navigation feeds. | Installed build (WP1), integration review (WP4) | End-to-end software choice |
| A04 | Radio status gives a fresh enough active frequency before a standby command. | Protocol evidence (WP2), stock driver bench check (WP4) | Standby tuning acceptance |
| A05 | A separate radio TCP endpoint works without disturbing the navigation feeds. | Topology (WP1), coexistence tests (WP6) | Radio path acceptance ([ADR-01](architecture.md#adr-01)) |
| A06 | The ESP32 can carry both navigation paths plus the radio path. | UART plan (done), coexistence tests (WP6) | Bridge acceptance |

---

<a id="risks"></a>

## 3. ⚠️ Risks

All risks are open.

| ID | Prio | Risk | How it is reduced |
| --- | :-: | --- | --- |
| K01 | P0 | The wrong manual or radio variant leads to incompatible wiring — Becker publishes separate manuals per hardware generation (S03). | Match model, revision and connector view before wiring |
| K02 | P0 | A standby command carries an unknown or stale active frequency (F04). | Check both frequencies on the radio during [stock driver validation](../research/becker-driver.md#consequences), including after a local frequency change |
| K03 | P0 | Binary writes are truncated or report false success (F02/F03). | Compare stock app traffic to the expected frame and confirm the result on the radio (S12) |
| K04 | P0 | Bridge overflow, several writers or debug output corrupt radio traffic (S05/S06). | Firmware audit, byte-exact load and failure tests, separate radio path |
| K05 | P0 | The new bridge interrupts navigation data. | Compare with today's installation; coexistence tests (WP6) |
| K06 | P0 | A simulator only repeats the driver's own assumptions. | Independent protocol evidence; label synthetic tests; simulation is never acceptance |
| K07 | P1 | Licenses or dependencies prevent reuse. | Check license files and build dependencies; no code copied without permission |
| K08 | P1 | Switching noise from the UBEC or ESP32 is heard in the VHF radio or opens its squelch. | Bench and aircraft check with the bridge on and off ([bench bring-up](../hardware/wiring.md#bring-up)) |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
