<a id="pinning-cabling-and-routing"></a>

## 🔌 Pinning, cabling and routing

> [!WARNING]
> Confirm cable pinouts, converter pads and ESP32 headers before connecting. UNKNOWN = unresolved.
>
> Zander and FLARM TX links start open. Leave their RX inputs disconnected for receive-only use.

[Bench checks and parts](bench-build.md) · [Sources](sources.md)

---

<a id="logical-connections"></a>

### 🧭 Logical connections

```mermaid
flowchart LR
    Z[Zander ZS1] --> NZ["RS-232-mod1: H-1-0290"]
    F["Classic FLARM"] --> NF["RS-232-mod2: H-1-0290"]
    B["Becker AR6201-022"] --> R["RS-422 mod"]
    NZ --> E[ESP32]
    NF --> E
    R --> E
    E -->|Wi-Fi: separate paths| O[OpenSoar]
    %% Keep edge declarations left to right. Return paths draw only the left arrowhead.
    Z <-.-> NZ
    F <-.-> NF
    B <--> R
    NZ <-.-> E
    NF <-.-> E
    R <--> E
    E <--> O
    linkStyle 7,8,9,10,11,12,13 marker-end:none
```

Dotted arrows show optional navigation returns. Becker commands are required for tuning.

---

<a id="hailege-board-pad-reference"></a>
<details>
<summary>Hailege board pad reference</summary>

| Chip-side reference | Inferred pad view |
| --- | --- |
| <img src="images/hailege-h-1-0290-board.svg" width="240" alt="Hailege H-1-0290 board, chip side"> | <img src="images/pinout-hailege.svg" width="420" alt="Hailege chip-side pad map. L1 ground, L2 supply, L3 RS-232 input, L4 RS-232 output. R1 TTL input, R2 TTL output, R3 supply, R4 ground. Continuity unknown."> |

Orient the chip side as pictured. Confirm continuity before power. [S26–S27](sources.md#source-register)

</details>

---

<a id="zander-zs1"></a>

### ✈️ Zander ZS1

![Proposed device to module to ESP32 path](images/wiring-zander.svg)

| ZS1 PC cable | → / ← | RS232 pads | TTL pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 2 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) | → | **L3** RS232 input | **R2** TTL output | → | GPIO32 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| *3 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO33 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) via TX link* |
| 5 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">) | — | L1 GND† | R4 GND | — | GND contact **UNKNOWN** |

Use the ZS1 PC-cable DE9. Optional RMC requires an enabled, unused RX input. † Confirm signal ground.

<details>
<summary>Zander PC-cable connector pin view</summary>

| Connector reference | Pin view |
| --- | --- |
| Matched cable photo pending. [ZS1 handbook D3029, pp.46/48](reference/ZS1_Deutsch.pdf#page=46) (S22) | <img src="images/pinout-zander-de9.svg" width="420" alt="ZS1 PC-cable DE-9 index. Pin 2 TX, pin 3 optional RX, pin 5 ground. Installed cable match and connector face unknown."> |

</details>

---

<a id="flarm"></a>

### 📡 FLARM

![Proposed device to module to ESP32 path](images/wiring-flarm.svg)

| Classic Power/Data reference† | → / ← | RS232 pads | TTL pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 5 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">)† | → | **L3** RS232 input | **R2** TTL output | → | GPIO26 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| *6 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO27 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) via TX link* |
| 7/8 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">)† | — | L1 GND† | R4 GND | — | GND contact **UNKNOWN** |

† Confirm the Classic Power/Data plug view and IGC numbering. Use a free data port and signal ground. Keep FLARM supply pins separate from converter VCC.

<details>
<summary>Classic FLARM Power/Data connector pin view</summary>

| Connector reference | Pin view |
| --- | --- |
| [Classic rear-panel and plug pictures, v221 p.6](https://www.flarm.com/wp-content/uploads/2024/04/FLARM_InstallationManual_D.pdf#page=6) (S23) | <img src="images/pinout-flarm-classic.svg" width="420" alt="Classic FLARM plug, latch down, IGC numbering. Pins 1 and 2 supply in, 3 auxiliary voltage out, 4, 7 and 8 ground, 5 TX, 6 RX. Installed match unknown."> |

IGC numbering runs in the opposite direction to common RJ45 numbering. Match the actual plug to the manual.

</details>

---

<a id="becker-radio"></a>

### 📻 Becker radio

![Proposed device to module to ESP32 path](images/wiring-becker.svg)

| | Becker J1* | → / ← | RS422 chip pin | Logic pin | → / ← | ESP32 |
| :-: | --- | :-: | --- | --- | :-: | --- |
| 🟦 | 2 (<img src="images/pins/tx2-plus.svg" alt="TX2+" width="28" height="14">) | → | 8 (<img src="images/pins/a.svg" alt="A" width="20" height="14">) | — | — | — |
| 🟦 | 9 (<img src="images/pins/tx2-minus.svg" alt="TX2−" width="28" height="14">) | → | 7 (<img src="images/pins/b.svg" alt="B" width="20" height="14">) | — | — | — |
| 🟦 | — | — | — | 2 (<img src="images/pins/r.svg" alt="R" width="20" height="14">) | → | GPIO16 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| 🟧 | 3 (<img src="images/pins/rx2-plus.svg" alt="RX2+" width="28" height="14">) | ← | 5 (<img src="images/pins/y.svg" alt="Y" width="20" height="14">) | — | — | — |
| 🟧 | 10 (<img src="images/pins/rx2-minus.svg" alt="RX2−" width="28" height="14">) | ← | 6 (<img src="images/pins/z.svg" alt="Z" width="20" height="14">) | — | — | — |
| 🟧 | — | — | — | 3 (<img src="images/pins/d.svg" alt="D" width="20" height="14">) | ← | GPIO17 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) |
| ⬜ | 21 (<img src="images/pins/d_gnd.svg" alt="D_GND" width="33" height="14">) | — | Reference **UNKNOWN** | — | — | — |
| ⬜ | — | — | — | 4 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">) | — | GND contact **UNKNOWN** |

Confirm J1 revision and signal ground. 🟦 Status · 🟧 Commands · ⬜ Ground. The table uses THVD1451D chip pins. GPIO17 connects directly to D for standby tuning. ESP32 header positions and breakout contacts remain **UNKNOWN**.

<details>
<summary>Becker rear connector and J1 pin view</summary>

| Radio rear reference | J1 mating-face reference |
| --- | --- |
| <img src="images/becker-reference-connectors.png" width="275" alt="Becker rear drawing, P1 above J1. Callout 4 identifies the J1 connector."> | <img src="images/pinout-becker-j1.svg" width="420" alt="Becker J1 DB-25 socket reference. Pins 2 and 9 TX pair, 3 and 10 RX pair, 21 digital ground. Installed match and ground bond unknown."> |

Callout 4 identifies J1. Functions from [Becker DV14307.03 Issue 06, pp.57/59](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=59) (S10/S20). Socket numbering follows the [Winford BRKDD25F-R-FT Rev. A mating-face drawing](https://www.winford.com/products/cad/brkdd25f-r-ft_rev_a_dwg.pdf). Confirm the moulded numbers on the installed connector.

</details>

Pin-view styling follows the [Pinout.xyz examples](https://github.com/pinout-xyz/Pinout.xyz/tree/main/graphics). Colours group functions.

---

<a id="power"></a>

### ⚡ Power

```mermaid
flowchart LR
    S["Nominal 12 V battery"] --> P["Fuse / reverse protection: UNKNOWN"]
    P --> I[Input node] --> B["BEC: 5 V"]
    I --- T["Transient clamp to return: UNKNOWN"]
    B --> V["Output protection if required: UNKNOWN"] --> E["ESP32 5 V input: position UNKNOWN"]
    E --> R[Board 3.3 V regulator]
    R --> C["RS-232-mod1 / RS-232-mod2 / RS-422 mod"]
```

| From | To | Status |
| --- | --- | --- |
| Protected BEC 5 V output | ESP32 5 V input contact **UNKNOWN** | Board/BEC identification pending |
| BEC output return | ESP32 GND contact **UNKNOWN** | Common logic ground; source return design pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod1 R3 (`+`) | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod2 R3 (`+`) | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-422 mod IC pin 1, VCC | Proposed |
| ESP32 GND contact **UNKNOWN** | RS-232-mod1 R4 (`−`) | Proposed; continuity pending |
| ESP32 GND contact **UNKNOWN** | RS-232-mod2 R4 (`−`) | Proposed; continuity pending |
| ESP32 GND contact **UNKNOWN** | RS-422 mod IC pin 4, GND | Proposed |

Use one VCC/GND pair per H-1-0290. Disconnect BEC 5 V before USB powering. [Protection and current budget](bench-build.md#power-protection-and-budget).

---

<a id="open-items"></a>

### ❓ Open items

| Needed | Resolves |
| --- | --- |
| Device connector photos + exact model/revisions | Correct pin view; FLARM/Zander/Becker applicability |
| Both H-1-0290 sides, IC markings + continuity map | Confirm inferred L/R pad map; equipment cable mapping |
| Exact ESP32 board/BEC + source limits | Header positions, current budget, protection |
| Return-traffic requirements + serial settings | Optional links and port configuration |
| Matched radio interface/reference/termination | Final radio circuit/harness |
