# Becker AR6201 to OpenSoar

Select an airfield in OpenSoar on Android and transfer its frequency to the Becker radio's **standby** slot while preserving the active frequency and existing Zander/FLARM feeds. An ESP32 Wi-Fi/serial bridge is the proposed transport. Prefer reuse of the existing installation and available software.

**Status — 2026-11-08:** Documentation and investigation only. This repository contains no firmware, simulator or automated runtime tests. Navigation reception is reported in earlier project notes; radio control and compatibility with the installed equipment have not been demonstrated. Even the exact radio model/revision remains to be confirmed.

## Start here

**Next: WP1 — record the existing installation.** Identify the radio, ESP32 and installed OpenSoar build; document current connections and missing evidence. Do not select replacement firmware or wiring from an example diagram. The [bounded WP1 task](docs/roadmap.md#wp1--existing-installation-first-priority) defines the inputs, output and completion criteria.

| Document | Owns |
| --- | --- |
| [Project baseline](docs/project-baseline.md) | Requirements, installed-system inventory, assumptions and risks |
| [Sources and evidence](docs/sources.md) | Evidence labels, pinned sources, rechecked facts and verification limits |
| [Architecture and decisions](docs/architecture.md) | Logical proposal, alternatives and decision status |
| [Roadmap and workflow](docs/roadmap.md) | Prioritized work packages, milestones and acceptance checks |
| [Codex instructions](AGENTS.md) | How to carry out one authorized work package |
| [Technical investigation](docs/technical-investigation.md) | Detailed source findings and hardware-reference leads |
| [Related projects](docs/related-projects.md) | Previous reuse investigation; candidates, not selected dependencies |
| [Documentation review](docs/reviews/2026-11-08-documentation.md) | Changes, preservation record and verification of this cleanup |

The pinned OpenSoar driver contains a write without an explicit binary length and locally assigns ACK after writing. These are source observations, not a successful radio test; see [F02–F04](docs/sources.md#rechecked-facts). No connector pinout, baud rate, bridge selection or aircraft installation is approved by this documentation.
