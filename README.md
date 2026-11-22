<a id="top"></a>

# ✈️ Wi-Fi Bridge: Glider Avionics to OpenSoar

<p align="center">
  <a href="docs/images/product-teaser.png"><img src="docs/images/product-teaser.png" width="100%" alt="Your glider instruments. On your phone. Illustrated glider cockpit with a compact Wi-Fi Bridge tucked behind the instrument panel and a clearly labelled enlarged product view. Wi-Fi connects it to OpenSoar on a phone. Features: Classic FLARM traffic and position, Zander ZS1 flight and navigation data, planned Becker AR6201 radio standby tuning, and browser setup and monitoring."></a>
</p>

A small ESP32 box in the glider collects data from the cockpit instruments and sends it over Wi-Fi to OpenSoar on a phone or tablet: FLARM and Zander data as standard NMEA over TCP, the radio on its own port. No cables run to the Android device.

📑 **On this page:** <kbd>[1 · 🎯 What it does](#what-it-does)</kbd> <kbd>[2 · 🚦 Project status](#status)</kbd> <kbd>[3 · 🗺️ Documentation map](#documentation)</kbd> <kbd>[4 · ⚠️ Safety](#safety)</kbd>

---

<a id="what-it-does"></a>

## 1. 🎯 What it does

| Device | What the bridge does |
| --- | --- |
| 📈 Glider computer: Zander ZS1 | Sends flight and navigation data to OpenSoar |
| 📡 Collision warning: Classic FLARM | Sends traffic and navigation data to OpenSoar |
| 📻 Radio: Becker AR6201 | Connects the built-in OpenSoar Becker AR62xx driver to the radio over a dedicated TCP port |

Use the [stock OpenSoar driver](docs/research/becker-driver.md#consequences). Standby tuning awaits protocol evidence and verification on the radio.

<details>
<summary><b>📱 Show the status page, settings and data monitor</b> (click to expand)</summary>

Connect the phone to the bridge's Wi-Fi and open `http://10.10.10.10` (or `http://wifi-bridgetom.local` when the bridge is on your network).

- **Top:** which bridge, firmware, uptime and Wi-Fi signal.
- **One row per device:** green dot while data arrives, the active baud rate and port, connected apps, errors in red. Counters are under *Details*.
- **All settings:** baud, frame and ports per device, network and hotspot. Amber marks a value saved on the bridge instead of the firmware default. Passwords are never shown.
- **Detect** on either navigation input checks its baud rate and recognizes Zander or FLARM sentences. Review the suggestion or set the port manually.
- **Data monitor:** the data passing each device, readable as text or hex, to check that data comes in. Pause and download.
- **Change settings** (password, user `bridge`): baud, frame and ports per device, network and hotspot. Saving restarts the bridge. A Wi-Fi, hotspot or port change runs on trial: press *Keep* within 10 minutes, otherwise the old settings come back by themselves.

<img src="docs/images/status-page.png" width="300" alt="Status page on a phone: housing photo, Zander, FLARM and Becker rows, all settings open"> <img src="docs/images/settings-page.png" width="300" alt="Settings page on a phone: fields per device and for Wi-Fi, with defaults"> <img src="docs/images/monitor-page.png" width="300" alt="Data monitor on a phone with demo NMEA lines from and to the Zander">

More: [status page](firmware/bridge/README.md#status-page), [settings page](firmware/bridge/README.md#settings-page) and [data monitor](firmware/bridge/README.md#data-monitor) details.

</details>

---

<a id="status"></a>

## 2. 🚦 Project status

> [!NOTE]
> **Status:** the assembled 12 V bench bridge passed the Becker loopback and separate Zander and FLARM input tests. [Remaining bench checks](docs/hardware/wiring.md#bring-up) cover the boards, supply and voltage accuracy.
> **Current focus:** 🔧 WP3 wiring and bench validation, with the 💻 [bridge firmware](docs/project/roadmap.md#firmware-plan) in parallel.

| Stage | Status |
| --- | --- |
| 🔬 Research and documentation | ✅ Done |
| 📦 Mechanical design | ✅ Accepted — [housing v16](docs/hardware/housing.md); bench fit passed, vibration and heat checks open |
| 🔧 Wiring and bench validation | 🟡 **Current focus** |
| 🛩️ Existing installation details | 🟡 Waiting for equipment information |
| 📶 Bench OTA loader | ✅ Flashed and updatable over Wi-Fi ([M3](docs/project/roadmap.md#milestones)); [ready-made hotspot-only image](firmware/ota-loader/bin/README.md) for the first USB flash |
| 💻 Bridge firmware | 🟡 Step 4 [radio port](docs/project/roadmap.md#firmware-plan) done on the wired bench bridge. Step 5 passed the FLARM and radio slow-client checks. The full three-port run and OpenSoar phone checks remain |
| 📻 OpenSoar radio connection | 🟡 Stock driver selected. Phone and radio checks remain |
| ✈️ Aircraft integration | ⬜ Later |

<img src="docs/images/becker-bridge-exploded.gif" width="600" alt="Exploded-view loop of the bridge housing: the hood screws and hood lift off and fly out of view, then the RS-232 boards, ESP32 and MAX490 with UBEC rise out of the base tray and settle back">

**Next step:** run the [bench checks](docs/hardware/wiring.md#bring-up) on the built bridge. See [WP3 Hardware](docs/project/roadmap.md#work-packages) for the full work package.

---

<a id="documentation"></a>

## 3. 🗺️ Documentation map

| | Section | Page | What you find there |
| :-: | --- | --- | --- |
| 🔧 | **Hardware** | [🔌 Connect the bridge to your glider](docs/hardware/wiring.md#overview) | Instrument plug solder-side view, pinout and bench checks |
| | | [⚡ Power and grounding](docs/hardware/power.md) | Supply, ground loops and currents |
| | | [🧾 Schematic](docs/hardware/schematic.md) | All boards and connections on one KiCad sheet |
| | | [🧱 Modules and parts](docs/hardware/modules.md) | Module details, parts list |
| | | [📦 Housing](docs/hardware/housing.md) | Mechanical design |
| | | [🪛 Wire installation](docs/hardware/wire-installation.md) | Assembling the harness step by step |
| | | [📐 CAD downloads and credits](cad/reference/README.md) | Reference models, original sources and reuse terms |
| 🧩 | **Design** | [🏗️ Architecture](docs/design/architecture.md) | How the bridge is built and why |
| | | [🎯 Requirements](docs/design/requirements.md) | Requirements, assumptions, risks |
| 📋 | **Project** | [🗺️ Roadmap](docs/project/roadmap.md) | Work plan |
| | | [💶 Hardware cost](docs/project/cost.md) | Cost breakdown for one bridge |
| | | [❓ Open items](docs/project/open-items.md) | Everything still missing |
| | | [🛩️ Existing installation](docs/project/existing-installation.md) | The bridge already in the aircraft |
| 🔬 | **Research** | [📚 Sources](docs/research/sources.md) | Evidence |
| | | [📻 Becker driver](docs/research/becker-driver.md) | Driver analysis |
| | | [📶 Bridge firmware](docs/research/bridge-firmware.md) | Firmware analysis |
| | | [🗂️ Reviews](docs/research/reviews) | Dated review records |
| 💾 | **Firmware** | [📶 OTA loader](firmware/ota-loader/README.md) | Bench loader source, variants and build script; [public image](firmware/ota-loader/bin/README.md) |
| | | [💻 Bridge firmware](firmware/bridge/README.md) | Bridge sketch from Yuri's ESP-Serial-Bridge, project settings and build script |

Working on the repository? Read the [work instructions](AGENTS.md) first.

---

<a id="safety"></a>

## 4. ⚠️ Safety

> [!WARNING]
> Keep aircraft equipment disconnected during bench checks. Aircraft connection requires separate approval and verification of the installed wiring and radio revision. The existing installation stays untouched as a backup.

---

<kbd>[⬆️ Back to top](#top)</kbd>
