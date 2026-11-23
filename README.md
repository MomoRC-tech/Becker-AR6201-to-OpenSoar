<a id="top"></a>

# ✈️ Wi-Fi Bridge: Glider Avionics to OpenSoar

<p align="center">
  <a href="docs/images/product-teaser.png"><img src="docs/images/product-teaser.png" width="100%" alt="Your glider instruments. On your phone. Illustrated glider cockpit with a compact Wi-Fi Bridge tucked behind the instrument panel and a clearly labelled enlarged product view. Wi-Fi connects it to OpenSoar on a phone. Features: Classic FLARM traffic and position, Zander ZS1 flight and navigation data, planned Becker AR6201 radio standby tuning, and browser setup and monitoring."></a>
</p>

A small ESP32 box in the glider collects data from the cockpit instruments and sends it over Wi-Fi to OpenSoar on a phone or tablet. No cables run to the phone.

<p align="center">
<a href="docs/manual/user-manual.md"><picture><source media="(prefers-color-scheme: dark)" srcset="docs/images/nav/card-use-dark.svg"><img src="docs/images/nav/card-use.svg" alt="Use the bridge: connect, pages, behaviour" width="32%"></picture></a>
<a href="docs/hardware/build-guide.md"><picture><source media="(prefers-color-scheme: dark)" srcset="docs/images/nav/card-build-dark.svg"><img src="docs/images/nav/card-build.svg" alt="Build one: parts to bench check" width="32%"></picture></a>
<a href="docs/README.md"><picture><source media="(prefers-color-scheme: dark)" srcset="docs/images/nav/card-reference-dark.svg"><img src="docs/images/nav/card-reference.svg" alt="Reference: design, project, research" width="32%"></picture></a>
</p>

---

<a id="what-it-does"></a>

## 1. 🎯 What it does

| Device | What you get in OpenSoar |
| --- | --- |
| 📈 Glider computer: Zander ZS1 | Flight and navigation data |
| 📡 Collision warning: Classic FLARM | Traffic and position |
| 📻 Radio: Becker AR6201 | The built-in OpenSoar Becker driver reaches the radio over its own port; standby tuning follows once the protocol is confirmed |

<a href="docs/manual/user-manual.md#web-pages"><img src="docs/images/status-start.png" width="220" align="right" alt="Status page on a phone: Zander, FLARM and Becker receiving, one app connected to each"></a>

Check and set it up from the phone's browser: live status per device, settings, a data monitor and a log ([web pages](docs/manual/user-manual.md#web-pages)).

<br clear="right">

---

<a id="status"></a>

## 2. 🚦 Status

> [!NOTE]
> **Status:** the bench bridge is built and passed all bench checks; the firmware is in bench acceptance. Aircraft installation comes later. Details: [roadmap](docs/project/roadmap.md#work-packages).

---

<a id="safety"></a>

## 3. ⚠️ Safety

> [!WARNING]
> Keep aircraft equipment disconnected during bench checks. Aircraft connection requires separate approval and verification of the installed wiring and radio revision. The existing installation stays untouched as a backup.

Working on the repository? Read the [work instructions](AGENTS.md) first.

---

<kbd>[⬆️ Back to top](#top)</kbd>
