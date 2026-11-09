<a id="pinning-cabling-and-routing"></a>

## 🔌 Pinning, cabling and routing

**Proposed, untested wiring. UNKNOWN = unresolved; do not connect.** GPIO assignments proposed; physical ESP32 header positions UNKNOWN. Existing setup stays as backup.

**RS-232-mod1** = Zander Hailege H-1-0290 · **RS-232-mod2** = FLARM Hailege H-1-0290 · **RS-422 mod** = proposed Becker THVD1451D circuit; no finished module selected.

[BOM and bench checks](bench-build.md). Tables show equipment-side and ESP32-side connections. *Italic* rows and dotted arrows = optional device-RX paths (ESP32 → device). TX links start open; their contact numbers are UNKNOWN.

Signals: <img src="images/pins/rx.svg" alt="RX" width="28" height="20"> receive · <img src="images/pins/tx.svg" alt="TX" width="28" height="20"> transmit · <img src="images/pins/gnd.svg" alt="GND" width="33" height="20"> ground. Labels refer to each endpoint; colours are not wire colours.

<a id="logical-connections"></a>

### 🧭 Logical connections

```mermaid
flowchart LR
    Z[Zander ZS1] --> NZ["RS-232-mod1: H-1-0290 RS-232"]
    F["Classic FLARM: reported, match pending"] --> NF["RS-232-mod2: H-1-0290 RS-232"]
    NZ --> E[ESP32]
    NF --> E
    NZ -.-> Z
    E -.-> NZ
    NF -.-> F
    E -.-> NF
    B["AR6201-022: reported, match pending"] --> R["RS-422 mod: proposed RS-422"]
    R --> E
    E -.-> R
    R -.-> B
    E -->|Wi-Fi: separate device paths| O[OpenSoar]
    O --> E
```

Navigation returns: optional, listed below. Serial settings unverified; existing TCP endpoints [reported in baseline](project-baseline.md#pending-configuration), not selected/verified new-build settings. Radio bytes separate from navigation/diagnostics.

<a id="zander-zs1"></a>

### ✈️ Zander ZS1

![Proposed device to module to ESP32 path](images/wiring-zander.svg)

| ZS1 PC cable | → / ← | RS-232-mod1 · RS-232 pads | RS-232-mod1 · logic pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 2 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">) | → | Input **UNKNOWN** | Output **UNKNOWN** | → | GPIO32 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">) |
| *3 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">)* (optional) | *←* | *Output **UNKNOWN*** | *Input **UNKNOWN*** | *←* | *GPIO33 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">) via Zander TX link* |
| 5 (<img src="images/pins/gnd.svg" alt="GND" width="33" height="20">) | — | Reference **UNKNOWN** | GND | — | GND contact **UNKNOWN** |

Conditional PC-cable DE9 reference; actual cable match pending. Device RX optional (RMC only); equipment-reference bonding unresolved.

<details>
<summary>🖼️ Connector / module / ESP32 pictures and references</summary>

| ZS1 connector | RS-232-mod1 | ESP32 |
| --- | --- | --- |
| Matched view missing; private S25 crop unverified | H-1-0290 pad mapping UNKNOWN | Exact board/header positions UNKNOWN |

**Unmatched drawing crop (S25):** connector **B** labels: 2 GND, 3 RX-PC, 4 TX-PC. Red-circled **A** uses RX-SR/TX-SR and other functions. Revision/view unverified; complete installation drawing and actual cable mapping still needed. This crop does not authorize wiring.

Source: [S22 original handbook](reference/ZS1_Deutsch.pdf), D3029, 16.09.2004, pp.46/48; conditional **PC-cable DE9** reference, not rear-panel B numbering. Cable/interface match pending. The 3037 supplement specifies 4,800 baud with the ZS1 FLARM function off, 19,200 with it on; installed version/setting unverified. RJ45/DE9 shape does not establish voltage or compatible pinning.

</details>

<a id="flarm"></a>

### 📡 FLARM

![Proposed device to module to ESP32 path](images/wiring-flarm.svg)

| FLARM data port | → / ← | RS-232-mod2 · RS-232 pads | RS-232-mod2 · logic pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| <img src="images/pins/tx.svg" alt="TX" width="28" height="20"> **UNKNOWN** | → | Input **UNKNOWN** | Output **UNKNOWN** | → | GPIO26 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">) |
| *<img src="images/pins/rx.svg" alt="RX" width="28" height="20"> **UNKNOWN*** (optional) | *←* | *Output **UNKNOWN*** | *Input **UNKNOWN*** | *←* | *GPIO27 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">) via FLARM TX link* |
| <img src="images/pins/gnd.svg" alt="GND" width="33" height="20"> **UNKNOWN** | — | Reference **UNKNOWN** | GND | — | GND contact **UNKNOWN** |

Classic FLARM reported; actual port/view unverified. Device RX optional; equipment-reference bonding unresolved. Preserve displays; no assumed splitter.

<details>
<summary>🖼️ Connector / module / ESP32 pictures and references</summary>

| FLARM connector | RS-232-mod2 | ESP32 |
| --- | --- | --- |
| Classic plug view: manual link below; actual cable unverified | H-1-0290 pad mapping UNKNOWN | Exact board/header positions UNKNOWN |

<img src="images/powerflarm-core-reference-connectors.png" width="260" alt="PowerFLARM Core manual reference: connector locations">
<img src="images/powerflarm-core-reference-de9.png" width="390" alt="PowerFLARM Core DE9 reference: device socket left, cable plug right, mating faces">

**PowerFLARM Core examples only; not the reported Classic FLARM.** DE9: device socket left, cable plug right, mating faces. [FTD-033 v1.1, pp.11/14](https://www.flarm.com/wp-content/uploads/2024/04/FTD-033-PowerFLARM-Core-Installation-Manual.pdf#page=11), © FLARM Technology AG. Retained as reference, not installed pinning.

**Conditional Classic reference (S23):** [F6/F7/F8/F9 manual v221, p.6](https://www.flarm.com/wp-content/uploads/2024/04/FLARM_InstallationManual_D.pdf#page=6): RJ45 Power/Data, TX 5, RX 6, ground 7/8 (also 4); supply 1/2. IGC numbering is reversed relative to common numbering: match the manual's plug view and trace the actual cable. Extension is a separate RJ12 socket. Installed pins remain UNKNOWN pending match.

Source: S23, FTD-012 §5 and Classic installation manual pp.6/7/9. RS-232 is not ESP32 GPIO signalling. Confirm baud/output; TCP port 2000 does not establish serial settings. Preserve displays/returns; the Classic manual permits one attached device per data port, so no assumed passive splitter.

</details>

Both H-1-0290 boards: pad mapping pending (S26); no onboard DB9. Signal direction requires [unpowered continuity](bench-build.md#h-1-0290-identification). For receive-only use, disconnect the RS-232 output at the instrument; an open TX link does not disable it.

<a id="becker-radio"></a>

### 📻 Becker radio

![Proposed device to module to ESP32 path](images/wiring-becker.svg)

| | Becker J1* | → / ← | RS-422 mod · line IC pin | RS-422 mod · logic / TX link | → / ← | ESP32 / TX link |
| :-: | --- | :-: | --- | --- | :-: | --- |
| 🟦 | 2 (<img src="images/pins/tx2-plus.svg" alt="TX2+" width="40" height="20">) | → | 8 (<img src="images/pins/a.svg" alt="A" width="28" height="20">) | — | — | — |
| 🟦 | 9 (<img src="images/pins/tx2-minus.svg" alt="TX2−" width="40" height="20">) | → | 7 (<img src="images/pins/b.svg" alt="B" width="28" height="20">) | — | — | — |
| 🟦 | — | — | — | 2 (<img src="images/pins/r.svg" alt="R" width="28" height="20">) | → | GPIO16 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">) |
| 🟧 | *3 (<img src="images/pins/rx2-plus.svg" alt="RX2+" width="40" height="20">)* | *←* | *5 (<img src="images/pins/y.svg" alt="Y" width="28" height="20">)* | — | — | — |
| 🟧 | *10 (<img src="images/pins/rx2-minus.svg" alt="RX2−" width="40" height="20">)* | *←* | *6 (<img src="images/pins/z.svg" alt="Z" width="28" height="20">)* | — | — | — |
| 🟧 | — | — | — | *3 (<img src="images/pins/d.svg" alt="D" width="28" height="20">)* | *←* | *TX link: module contact **UNKNOWN*** |
| 🟧 | — | — | — | *TX link: ESP32 contact **UNKNOWN*** | *←* | *GPIO17 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">)* |
| ⬜ | 21 (<img src="images/pins/d_gnd.svg" alt="D_GND" width="47" height="20">) | — | Reference **UNKNOWN** | — | — | — |
| ⬜ | — | — | — | 4 (<img src="images/pins/gnd.svg" alt="GND" width="33" height="20">) | — | GND contact **UNKNOWN** |

*Conditional J1 reference; installed revision unverified. 🟦 Status · 🟧 *Commands (optional)* · ⬜ Reference/ground. Each row shows one connection; — = unused cell. **Becker device RX is required for standby tuning; optional only for receive-only monitoring.** THVD1451D **IC numbers**, not module-header numbers. Breakout contacts, reference bonding and termination UNKNOWN. Keep radio unplugged pending checks; TX link does not disable the transceiver. No galvanic isolation.

<details>
<summary>🖼️ Connector / module / ESP32 pictures and references</summary>

| Becker connector | RS-422 mod | ESP32 |
| --- | --- | --- |
| Reference rear view below; contacts not numbered | Proposed circuit; finished-module picture unavailable | Exact board/header positions UNKNOWN |

<img src="images/becker-reference-connectors.png" width="325" alt="Becker manual reference: AR62X1/RT62X1 rear connector layout, P1 above J1">

**Reference only — AR62X1/RT62X1 rear view; AR6201-(022) reported, revision UNKNOWN.** Callout 4 identifies J1, not contact 4. Contact numbering is not shown. [DV14307.03 Issue 06, p.57, Fig.18](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=57), © Becker Avionics GmbH.

Sources: S10, DV14307.03 Issue 06 §2.5.3.2 p.59; S18 THVD1451D. [Circuit details](bench-build.md#rs-422-circuit).

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
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod1 VCC pad | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod2 VCC pad | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-422 mod IC pin 1, VCC | Proposed |
| ESP32 GND contact **UNKNOWN** | RS-232-mod1 GND pad | Proposed; continuity pending |
| ESP32 GND contact **UNKNOWN** | RS-232-mod2 GND pad | Proposed; continuity pending |
| ESP32 GND contact **UNKNOWN** | RS-422 mod IC pin 4, GND | Proposed |

Use one VCC/GND pair per H-1-0290; the listing reports shared power pads on both sides (S26), continuity pending. Disconnect BEC 5 V before USB powering. [Protection/current budget](bench-build.md#power-protection-and-budget) pending. Keep existing instrument supplies unchanged.

<a id="open-items"></a>

### ❓ Open items

| Needed | Resolves |
| --- | --- |
| Device connector photos + exact model/revisions | Correct pin view; FLARM/Zander/Becker applicability |
| Both H-1-0290 sides, IC markings + continuity map | RS-232 ↔ logic pads; equipment cable mapping |
| Exact ESP32 board/BEC + source limits | Header positions, current budget, protection |
| Return-traffic requirements + serial settings | Optional links and port configuration |
| Matched radio interface/reference/termination | Final radio circuit/harness |

Manual crops: [S20](sources.md#connector-reference-drawings). Installed connector matches unverified; no electrical acceptance.
