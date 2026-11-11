<a id="top"></a>

# 🛩️ Existing installation

🏠 [Overview](../../README.md) › 📋 Project › **Existing installation**

What is already in the glider (a DG200): Zander and FLARM → an ESP32 Wi-Fi bridge → OpenSoar. The Becker radio is not connected to it. This setup stays untouched as a backup ([ADR-03](../design/architecture.md#adr-03)); no parts are reused for the new bridge.

> [!NOTE]
> **Status:** based on Tom's report (S21) and four photos (S13); nothing measured. Missing facts: [installed equipment](open-items.md#installed-equipment). Full assessment: [2026-11-09 review](../research/reviews/2026-11-09-installation-claims.md).

📑 **On this page:** <kbd>[1 · 🧰 Equipment](#equipment)</kbd> <kbd>[2 · 🌐 OpenSoar configuration](#configuration)</kbd> <kbd>[3 · 📷 Photos](#photos)</kbd>

> [!WARNING]
> Keep logs, device identifiers, credentials and configuration backups out of this public repository.

---

<a id="equipment"></a>

## 1. 🧰 Equipment

| Item | What is known | Source |
| --- | --- | --- |
| 📻 Radio | Becker AR6201-(022), ETSO-2C37e/2C38e; matches the Issue 06 manual | S21, S10 |
| 📈 Zander | ZS1; its FLARM input is unused | S21 |
| 📡 FLARM | Classic FLARM, IGC series 6 | S21, S23 |
| 🧠 ESP32 | WROOM-family board with micro-USB and BOOT/RST buttons | S13 |
| 🔁 Converter | Blue "RS422 TO TTL" board with Y/Z/B/A terminals (not connected to the radio) | S13 |
| 🔋 Power | 12 V → Hobbywing UBEC 3A → ESP32; FLARM and Zander have their own supplies | S21 |
| 🔌 Wiring | Partly visible in the photos; two unmatched wiring sketches (S25). Whether there is an RS-232 converter is not known | S13, S25 |
| 📶 Navigation | Zander and FLARM data reach OpenSoar | S21 |

---

<a id="configuration"></a>

## 2. 🌐 OpenSoar configuration

As reported by Tom (S21):

| Device | TCP endpoint | OpenSoar driver |
| --- | --- | --- |
| 📈 Zander | `10.10.10.10:10110` | `Zander / SDI` |
| 📡 FLARM | `10.10.10.10:2000` | `FLARM` |
| 📻 Becker | `10.10.10.10:4352` | `AR62xx` — entry only, the radio is not connected |

The installed OpenSoar build and the role of the OpenVario are not known. The review baseline is OpenSoar 7.46.26 (S09); which Zander fields it uses is in S24.

---

<a id="photos"></a>

## 3. 📷 Photos

Four private photos from Tom (S13). They show what is visible only — no wiring can be traced end to end.

| Photo | What is visible | What it tells us |
| --- | --- | --- |
| P1 | Blue "RS422 TO TTL" board, screw terminals Y/Z/B/A, VCC/GND at the header; chip marking unreadable | A full-duplex RS-422 module; how it is wired to the radio or ESP32 cannot be seen |
| P2 | HOBBYWING UBEC with "3A" marking, blue jumper, stripboard, soldered wires | Probably the bridge's regulator; jumper setting and output voltage unknown |
| P3 | ESP32 board, module marked ESP-WROOM-32, micro-USB, BOOT/RST, pin labels, wiring | A classic ESP32-WROOM board; exact board type and pin use unknown |
| P4 | Wider view: board, green enclosure, partly covered cable assembly | Assembly context only; covered joints hide the wiring |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
