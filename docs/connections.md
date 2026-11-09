<a id="pinning-cabling-and-routing"></a>

## 🔌 Pinning, cabling and routing

**Proposed wiring. Confirm cable pinouts, converter pads and ESP32 headers before connecting. UNKNOWN = unresolved.**

TX links start open. Leave instrument RX disconnected for receive-only use.

[Bench checks and parts](bench-build.md) · [Sources](sources.md)

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

<a id="hailege-board-pad-reference"></a>
<details>
<summary>Hailege board pad reference</summary>

<img src="images/hailege-h-1-0290-board.svg" width="240" alt="Hailege H-1-0290 board, chip side">

**Inferred pad map.** Orient the chip side as pictured. Confirm continuity before power.

```text
        RS232 / instrument             TTL / ESP32
                +---------------------+
  L1  GND   −   |                     |   ←  TX input   R1
  L2  3.3 V +   |      MAX3232CSE      |   →  RX output  R2
  L3  RX in →   |                     |   +  3.3 V      R3
  L4  TX out←   |                     |   −  GND        R4
                +---------------------+
```

</details>

<a id="zander-zs1"></a>

### ✈️ Zander ZS1

![Proposed device to module to ESP32 path](images/wiring-zander.svg)

| ZS1 PC cable | → / ← | RS232 pads | TTL pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 2 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) | → | **L3** RS232 input | **R2** TTL output | → | GPIO32 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| *3 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO33 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) via TX link* |
| 5 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">) | — | L1 GND† | R4 GND | — | GND contact **UNKNOWN** |

Use the ZS1 PC-cable DE9. Optional RMC requires an enabled, unused RX input. † Confirm signal ground.

<a id="flarm"></a>

### 📡 FLARM

![Proposed device to module to ESP32 path](images/wiring-flarm.svg)

| Classic Power/Data reference† | → / ← | RS232 pads | TTL pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 5 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">)† | → | **L3** RS232 input | **R2** TTL output | → | GPIO26 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| *6 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO27 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) via TX link* |
| 7/8 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">)† | — | L1 GND† | R4 GND | — | GND contact **UNKNOWN** |

† Confirm the Classic Power/Data plug view and IGC numbering. Use a free data port and signal ground. Keep FLARM supply pins separate from converter VCC.

<a id="becker-radio"></a>

### 📻 Becker radio

![Proposed device to module to ESP32 path](images/wiring-becker.svg)

| | Becker J1* | → / ← | RS422 chip pin | Logic / TX link | → / ← | ESP32 / TX link |
| :-: | --- | :-: | --- | --- | :-: | --- |
| 🟦 | 2 (<img src="images/pins/tx2-plus.svg" alt="TX2+" width="28" height="14">) | → | 8 (<img src="images/pins/a.svg" alt="A" width="20" height="14">) | — | — | — |
| 🟦 | 9 (<img src="images/pins/tx2-minus.svg" alt="TX2−" width="28" height="14">) | → | 7 (<img src="images/pins/b.svg" alt="B" width="20" height="14">) | — | — | — |
| 🟦 | — | — | — | 2 (<img src="images/pins/r.svg" alt="R" width="20" height="14">) | → | GPIO16 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| 🟧 | *3 (<img src="images/pins/rx2-plus.svg" alt="RX2+" width="28" height="14">)* | *←* | *5 (<img src="images/pins/y.svg" alt="Y" width="20" height="14">)* | — | — | — |
| 🟧 | *10 (<img src="images/pins/rx2-minus.svg" alt="RX2−" width="28" height="14">)* | *←* | *6 (<img src="images/pins/z.svg" alt="Z" width="20" height="14">)* | — | — | — |
| 🟧 | — | — | — | *3 (<img src="images/pins/d.svg" alt="D" width="20" height="14">)* | *←* | *TX link: module contact **UNKNOWN*** |
| 🟧 | — | — | — | *TX link: ESP32 contact **UNKNOWN*** | *←* | *GPIO17 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">)* |
| ⬜ | 21 (<img src="images/pins/d_gnd.svg" alt="D_GND" width="33" height="14">) | — | Reference **UNKNOWN** | — | — | — |
| ⬜ | — | — | — | 4 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">) | — | GND contact **UNKNOWN** |

Confirm J1 revision and signal ground. 🟦 Status · 🟧 Commands · ⬜ Ground. The table uses THVD1451D chip pins. Standby tuning requires the command path.

<details>
<summary>Becker rear connector view</summary>

<img src="images/becker-reference-connectors.png" width="325" alt="Becker rear view with P1 above J1">

</details>

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

<a id="open-items"></a>

### ❓ Open items

| Needed | Resolves |
| --- | --- |
| Device connector photos + exact model/revisions | Correct pin view; FLARM/Zander/Becker applicability |
| Both H-1-0290 sides, IC markings + continuity map | Confirm inferred L/R pad map; equipment cable mapping |
| Exact ESP32 board/BEC + source limits | Header positions, current budget, protection |
| Return-traffic requirements + serial settings | Optional links and port configuration |
| Matched radio interface/reference/termination | Final radio circuit/harness |
