<a id="top"></a>

# ✈️ Wi-Fi Bridge: Glider Avionics to OpenSoar

**FLARM · Zander ZS1 glider computer · Becker AR6201 radio → OpenSoar on Android**

A small ESP32 box in the glider that collects data from the cockpit instruments and sends it over Wi-Fi to OpenSoar on a phone or tablet. No cables run to the Android device.

- 📡 **Traffic:** FLARM collision-warning and position data appear in OpenSoar.
- 📈 **Flight data:** the Zander ZS1 sends flight and navigation data.
- 📻 **Radio:** pick an airfield in OpenSoar and its frequency goes into the Becker radio's standby slot.

[![Housing prototype v1, closed: MomoRC logo and 7-20V on the hood, XT30 power socket and DE-9 instrument connector on the end wall](cad/housing/housing-body-v1/preview-closed.png)](cad/housing/housing-body-v1/README.md)

> [!NOTE]
> **Status:** the housing design is accepted for the bench prototype. Physical fit, wiring and electrical checks are pending.
> **Current focus:** 🔧 WP3 wiring and bench validation, with the 💻 [bridge firmware](docs/project/roadmap.md#firmware-plan) in parallel. Driver work waits until WP3 is complete.

📑 **On this page:** <kbd>[1 · 🎯 What it does](#what-it-does)</kbd> <kbd>[2 · 🚦 Project status](#status)</kbd> <kbd>[3 · 🗺️ Documentation map](#documentation)</kbd> <kbd>[4 · ⚠️ Safety](#safety)</kbd>

---

<a id="what-it-does"></a>

## 1. 🎯 What it does

![Overview: glider computer Zander ZS1 and collision warning Classic FLARM send data to the Wi-Fi bridge; radio Becker AR6201 exchanges data both ways; the bridge talks to the navigation app OpenSoar over Wi-Fi](docs/images/overview.svg)

| Device | What the bridge does |
| --- | --- |
| 📈 Glider computer: Zander ZS1 | Sends flight and navigation data to OpenSoar |
| 📡 Collision warning: Classic FLARM | Sends traffic and navigation data to OpenSoar |
| 📻 Radio: Becker AR6201 | Puts the selected airfield frequency into standby; reports radio status where supported |

The bridge never changes the active frequency, and the radio's own controls keep working. Not planned: active tuning, frequency swap, volume and squelch.

<details>
<summary><b>📱 Show the status page, settings and data monitor</b> (click to expand)</summary>

Connect the phone to the bridge's Wi-Fi and open `http://10.10.10.10` (or `http://wifi-bridgetom.local` when the bridge is on your network).

- **Top:** which bridge, firmware, uptime and Wi-Fi signal.
- **One row per device:** green dot while data arrives, the active baud rate and port, connected apps, errors in red. Counters are under *Details*.
- **All settings:** baud, frame and ports per device, network and hotspot. Amber marks a value saved on the bridge instead of the firmware default. Passwords are never shown.
- **Data monitor:** the data passing each device, readable as text or hex, to check that data comes in. Pause and download.
- **Change settings** (password, user `bridge`): baud, frame and ports per device, network and hotspot. Saving restarts the bridge. A Wi-Fi, hotspot or port change runs on trial: press *Keep* within 10 minutes, otherwise the old settings come back by themselves.

<img src="docs/images/status-page.png" width="300" alt="Status page on a phone: housing photo, Zander, FLARM and Becker rows, all settings open"> <img src="docs/images/settings-page.png" width="300" alt="Settings page on a phone: fields per device and for Wi-Fi, with defaults"> <img src="docs/images/monitor-page.png" width="300" alt="Data monitor on a phone with demo NMEA lines from and to the Zander">

More: [status page](firmware/bridge/README.md#status-page), [settings page](firmware/bridge/README.md#settings-page) and [data monitor](firmware/bridge/README.md#data-monitor) details.

</details>

---

<a id="status"></a>

## 2. 🚦 Project status

| Stage | Status |
| --- | --- |
| 🔬 Research and documentation | ✅ Done |
| 📦 Mechanical design | ✅ Accepted — [housing v16](docs/hardware/housing.md); physical fit checks open |
| 🔧 Wiring and bench validation | 🟡 **Current focus** |
| 🛩️ Existing installation details | 🟡 Waiting for equipment information |
| 📶 Bench OTA loader | ✅ Flashed and updatable over Wi-Fi ([M3](docs/project/roadmap.md#milestones)); [ready-made hotspot-only image](firmware/ota-loader/bin/README.md) for the first USB flash |
| 💻 Bridge firmware | 🟡 Step 4 [radio port ownership](docs/project/roadmap.md#firmware-plan) checked on the standalone ESP32. Wired checks for steps 4–5 remain |
| 📻 OpenSoar driver changes | ⏸️ On hold until WP3 is complete |
| ✈️ Aircraft integration | ⬜ Later |

**Next step:** verify the bench supply and delivered parts using the [bring-up checklist](docs/hardware/wiring.md#bring-up). See [WP3 Hardware](docs/project/roadmap.md#work-packages) for the full work package.

---

<a id="documentation"></a>

## 3. 🗺️ Documentation map

| | Section | Page | What you find there |
| :-: | --- | --- | --- |
| 🔧 | **Hardware** | [🔌 Signal wiring](docs/hardware/wiring.md) | Pin-by-pin signal wiring and bench bring-up |
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
> Nothing here is verified on real hardware. Do the [bench checks](docs/project/open-items.md#bench-hardware) before connecting any aircraft equipment. The existing installation stays untouched as a backup.

---

<kbd>[⬆️ Back to top](#top)</kbd>
