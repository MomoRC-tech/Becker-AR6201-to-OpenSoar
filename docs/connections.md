<a id="pinning-cabling-and-routing"></a>

## 🔌 Pinning, cabling and routing

> [!WARNING]
> Confirm cable pinouts, converter pads and ESP32 headers before connecting. UNKNOWN = unresolved.
>
> Wire the optional Zander and FLARM return paths directly where required. Leave instrument RX disconnected when the return path is unused.

[Module specs](#module-specs) · [Parts list](#parts-list) · [Sources](sources.md)

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

> [!NOTE]
> Dotted arrows show optional navigation returns. Becker commands are required for tuning.

---

<a id="hailege-board-pad-reference"></a>
<a id="converter-modules"></a>

### Module specs

<details>
<summary>Module specs, converter photos and pin diagrams</summary>

| Module | Power | Chip | Reported size |
| --- | --- | --- | --- |
| Hailege H-1-0290 (Zander and FLARM) | 3.3 V | MAX3232CSE (reported) | 9.4 × 15.9 mm |
| RS422 TO TTL (Becker) | 5 V per schematic | MAX490ESA (owner-confirmed) | 50 × 27 mm |
| AZ-Delivery ESP32 WROOM dev module | 5 V input, 3.3 V regulator | — | — |
| BEC (model TBD) | 2S–4S input reported, 5 V output | — | — |

[Power module sources (S16–S19)](sources.md#source-register)

#### Hailege H-1-0290 · RS-232

| Chip-side reference | Inferred pad view |
| --- | --- |
| <img src="images/hailege-h-1-0290-board.svg" width="240" alt="Hailege H-1-0290 board, chip side"> | <img src="images/pinout-hailege.svg" width="420" alt="Hailege chip-side pad map. L1 ground, L2 supply, L3 RS-232 input, L4 RS-232 output. R1 TTL input, R2 TTL output, R3 supply, R4 ground. Continuity unknown."> |

[S26–S27](sources.md#source-register)

#### RS422 TO TTL · RS-422

| Chip-side reference | Module pin diagram |
| --- | --- |
| <img src="images/rs422-to-ttl-supplied.png" width="160" alt="Supplied RS422 TO TTL board, chip side with the logic header above the green screw terminal block"> | <img src="images/pinout-rs422-to-ttl.svg" width="420" alt="Chip-side RS-422 module. Top left to right: TXD input, RXD output, GND, 5 V VCC. Bottom left to right: Y and Z outputs, B and A inputs."> |

[S28–S30](sources.md#source-register)

</details>

---

<a id="zander-zs1"></a>

### ✈️ Zander ZS1

![Proposed device to module to ESP32 path](images/wiring-zander.svg)

| ZS1 PC cable | → / ← | RS232 pads | TTL pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 2 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) | → | **L3** RS232 input | **R2** TTL output | → | GPIO32 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| *3 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO33 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">)* |
| 5 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">) | — | L1 GND† | R4 GND | — | GND contact |

> [!IMPORTANT]
> Use the ZS1 PC-cable DE9. A matched cable photo is pending. Optional RMC requires an enabled, unused RX input. † Confirm signal ground.

<details>
<summary>Zander PC-cable connector pin view</summary>

| Connector reference | Pin view |
| --- | --- |
| [ZS1 handbook D3029, pp.46/48](reference/ZS1_Deutsch.pdf#page=46) (S22) | <img src="images/pinout-zander-de9.svg" width="420" alt="ZS1 PC-cable DE-9 index. Pin 2 TX, pin 3 optional RX, pin 5 ground. Installed cable match and connector face unknown."> |

</details>

---

<a id="flarm"></a>

### 📡 FLARM

![Proposed device to module to ESP32 path](images/wiring-flarm.svg)

| Classic Power/Data reference† | → / ← | RS232 pads | TTL pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 5 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">)† | → | **L3** RS232 input | **R2** TTL output | → | GPIO26 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) |
| *6 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO27 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">)* |
| 7/8 (<img src="images/pins/gnd.svg" alt="GND" width="23" height="14">)† | — | L1 GND† | R4 GND | — | GND contact |

> [!WARNING]
> † Match the Classic Power/Data plug to the manual. IGC numbering runs in the opposite direction to common RJ45 numbering. Use a free data port and signal ground. Keep FLARM supply pins separate from converter VCC.

<details>
<summary>Classic FLARM Power/Data connector pin view</summary>

| Connector reference | Pin view |
| --- | --- |
| [Classic rear-panel and plug pictures, v221 p.6](https://www.flarm.com/wp-content/uploads/2024/04/FLARM_InstallationManual_D.pdf#page=6) (S23) | <img src="images/pinout-flarm-classic.svg" width="420" alt="Classic FLARM plug, latch down, IGC numbering. Pins 1 and 2 supply in, 3 auxiliary voltage out, 4, 7 and 8 ground, 5 TX, 6 RX. Installed match unknown."> |

</details>

---

<a id="becker-radio"></a>

### 📻 Becker radio

![Proposed device to module to ESP32 path](images/wiring-becker.svg)

| Becker J1 reference† | → / ← | RS-422 terminals | TTL header | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 2 TX2+ / 9 TX2− | → | **A / B** input | **RXD** output | → | GPIO16 (<img src="images/pins/rx.svg" alt="RX" width="20" height="14">) via level conversion |
| 3 RX2+ / 10 RX2− | ← | **Y / Z** output | **TXD** input | ← | GPIO17 (<img src="images/pins/tx.svg" alt="TX" width="20" height="14">) (interface TBD) |
| 21 D_GND | — | Bond **UNKNOWN** | — | — | — |
| — | — | — | GND | — | GND contact |

> [!WARNING]
> † Confirm the installed J1 revision, moulded pin numbers and pair polarity. Use separate twisted pairs. Convert the 5 V-domain RXD before GPIO16. Verify GPIO17 compatibility, series resistance, termination, shielding and reference bonding before connecting. [S28–S30](sources.md#source-register) · [Espressif GPIO limits](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/hardware-design.html#what-is-the-voltage-tolerance-of-gpios-of-esp-chips)

<details>
<summary>Becker rear connector and J1 pin view</summary>

| Radio rear reference | J1 mating-face reference |
| --- | --- |
| <img src="images/becker-reference-connectors.png" width="275" alt="Becker rear drawing, P1 above J1. Callout 4 identifies the J1 connector."> | <img src="images/pinout-becker-j1.svg" width="420" alt="Becker J1 DB-25 socket reference. Pins 2 and 9 TX pair, 3 and 10 RX pair, 21 digital ground. Installed match and ground bond unknown."> |

Callout 4 identifies J1. Functions from [Becker DV14307.03 Issue 06, pp.57/59](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=59) (S10/S20). Socket numbering follows the [Winford BRKDD25F-R-FT Rev. A mating-face drawing](https://www.winford.com/products/cad/brkdd25f-r-ft_rev_a_dwg.pdf).

</details>

Pin-view styling follows the [Pinout.xyz examples](https://github.com/pinout-xyz/Pinout.xyz/tree/main/graphics). Colours group functions.

---

<a id="power"></a>

### ⚡ Power

```mermaid
flowchart LR
    S["Nominal 12 V battery"] --> F1["F1 fuse (rating TBD)"]
    F1 --> P["Reverse protection: UNKNOWN"]
    P --> I[Input node] --> B["BEC: 5 V"]
    I --- T["Transient clamp to return: UNKNOWN"]
    B --> V["Output protection if required: UNKNOWN"] --> E["ESP32 5 V input: position UNKNOWN"]
    E --> R[Board 3.3 V regulator]
    R --> C["RS-232-mod1 / RS-232-mod2"]
    V --> Q["Becker module VCC: 5 V"]
```

| From | To | Status |
| --- | --- | --- |
| Protected BEC 5 V output | ESP32 5 V input contact **UNKNOWN** | Verify 5 V header position and BEC output |
| BEC output return | ESP32 GND contact | Common logic ground; source return design pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod1 R3 (`+`) | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod2 R3 (`+`) | Proposed; module check pending |
| Protected BEC 5 V output | RS-422 module VCC | 5 V per board schematic (S29) |
| ESP32 GND contact | RS-232-mod1 R4 (`−`) | Proposed; continuity pending |
| ESP32 GND contact | RS-232-mod2 R4 (`−`) | Proposed; continuity pending |
| ESP32 GND contact | RS-422 module GND contact | Proposed logic return |

> [!WARNING]
> Use one VCC/GND pair per H-1-0290. Disconnect BEC 5 V before USB powering. Preserve existing instrument power feeds.
>
> - Place F1 near the battery on the bridge's positive feed. Use a separate reverse-polarity stage only if the exact BEC lacks adequate reverse-input protection.
> - Coordinate any input transient clamp with the fuse and BEC limits. Evaluate output overvoltage cutoff against BEC faults and ESP32 input limits.
> - Verify BEC polarity, tolerance and ripple. Check regulator current and temperature with Wi-Fi and the 3.3 V loads active. Spare 3.3 V capacity, source voltage range, BEC model and protection ratings remain open. [S16–S19](sources.md#source-register)

#### F1 sizing inputs

| Needed | Check |
| --- | --- |
| BEC model and datasheet | Input range, any specified input fuse, and reverse-input protection |
| Bridge load on the BEC input | Worst-case running current and startup inrush magnitude and duration across the battery voltage range |
| Battery and wiring | Maximum source voltage and prospective fault current. Smallest wire gauge, run length, connector and holder ratings, and temperature/bundling |

> [!IMPORTANT]
> Select the lowest fuse rating that carries the running load with temperature margin and survives startup per its time-current and pulse data. Check that it clears faults before the protected wiring or connectors overheat. Verify DC voltage and interrupt ratings against the source. [S31](sources.md#source-register)

---

<a id="parts-list"></a>

### Parts list

Parts for one bridge.

| Ref / quantity | Part or specification | Status / use |
| --- | --- | --- |
| MCU / 1 | AZ-Delivery ESP32 WROOM dev module | [Power](#power) |
| RS-232-mod1, RS-232-mod2 / 2 | Hailege H-1-0290 | [Zander](#zander-zs1) and [FLARM](#flarm) |
| PS1 / 1 | 5 V BEC | [Power](#power) |
| RS-422 mod / 1 | Assembled RS422-to-TTL board | [Becker](#becker-radio) |
| R-Z, R-F / 2 | 10 kΩ, 1%, at least 0.125 W | Proposed idle pull-ups for the Zander and FLARM converter inputs |
| F1 / 1 | DC blade fuse with inline holder (bench candidate: 32 V DC ATO/ATOF) | [Current rating TBD](#f1-sizing-inputs) |
| Reverse-polarity stage / optional | Separate BEC input protection | Only if the BEC lacks adequate reverse-input protection |

> [!IMPORTANT]
> Keep the existing assembly and harness as backup. Hold F1 and holder purchase until the BEC, source and wiring limits are known.

---

<a id="open-items"></a>

### ❓ Open items

| Needed | Resolves |
| --- | --- |
| Device connector photos + exact model/revisions | Correct pin view; FLARM/Zander/Becker applicability |
| Both H-1-0290 sides, IC markings + continuity map | Confirm inferred L/R pad map; equipment cable mapping |
| ESP32 header/regulator checks and BEC/source limits | Header positions, current budget, protection |
| Return-traffic requirements + serial settings | Optional return wiring and port configuration |
| Confirm RS-422 interface levels, series resistance and effective termination | Becker level conversion, termination and reference |
