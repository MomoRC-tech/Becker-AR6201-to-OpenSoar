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

### Hailege board pad reference

<img src="images/hailege-h-1-0290-supplied.png" width="330" alt="Owner-supplied Hailege H-1-0290 listing image, chip side: four pads on each end">

S26 supplied product image, unchanged; third-party artwork, no repository-license claim. Use **9.4 × 15.9 mm** from the supplied text; the image's 17 × 10 mm annotation is not the selected dimension.

**Inferred pinout, not measured:** S27 supplier photos/underside labels and schematic match the pictured layout. No Hailege-specific schematic or sample continuity result obtained. Confirm both boards before power. **Chip side up, oriented exactly as above:** left top is `−`, right bottom is `−`. L1–L4/R1–R4 are our pad references, not printed pin numbers. Turning the board over mirrors the view.

```text
        RS232 / instrument             TTL / ESP32
                +---------------------+
  L1  GND   −   |                     |   ←  TX input   R1
  L2  3.3 V +   |      MAX3232CSE      |   →  RX output  R2
  L3  RX in →   |                     |   +  3.3 V      R3
  L4  TX out←   |                     |   −  GND        R4
                +---------------------+
```

Arrows show flow **at the board**, not device RX/TX names. Instrument TX → L3 → R2 → ESP32 RX; ESP32 TX → R1 → L4 → instrument RX. Supply either L2/L1 or R3/R4; no separate supply on the other side.

| Pad, top to bottom | Mark / inferred function | Expected IC continuity in S27 comparator | Connection in this proposal |
| --- | --- | --- | --- |
| L1 | `−` / GND | 15; also R4 | Equipment signal reference after cable/grounding match |
| L2 | `+` / VCC | 16; also R3 | Leave unconnected when powered at R3 |
| L3 | `→` / RS232 input | 8 R2IN | Zander or FLARM TX |
| L4 | `←` / RS232 output | 7 T2OUT | Optional Zander or FLARM RX |
| R1 | `←` / TTL input | 10 T2IN | ESP32 TX via initially open link; proposed 10 kΩ pull-up to 3.3 V at R1 |
| R2 | `→` / TTL output | 9 R2OUT | ESP32 RX |
| R3 | `+` / VCC | 16; also L2 | ESP32 board-derived 3.3 V |
| R4 | `−` / GND | 15; also L1 | ESP32 GND |

The reference board exposes one TX/RX pair, using IC channel 2; two boards remain selected. No direct L3–R2 or R1–L4 continuity is expected through the active IC. [Unpowered checks](bench-build.md#h-1-0290-identification) validate pads to IC pins, not through the converter. This is not galvanic isolation; installed signal-reference/grounding compatibility remains open.

Sources: [S27 supplier reference](https://www.sunrom.com/p/rs232-ttl-module-max3232), schematic Rev. 1 (2016-02-29), board/underside photos; S26 supplied photo. [Evidence limits](sources.md).

<a id="zander-zs1"></a>

### ✈️ Zander ZS1

![Proposed device to module to ESP32 path](images/wiring-zander.svg)

| ZS1 PC cable | → / ← | RS-232-mod1 · RS-232 pads | RS-232-mod1 · logic pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 2 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">) | → | **L3** RS232 input | **R2** TTL output | → | GPIO32 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">) |
| *3 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO33 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">) via Zander TX link* |
| 5 (<img src="images/pins/gnd.svg" alt="GND" width="33" height="20">) | — | L1 GND† | R4 GND | — | GND contact **UNKNOWN** |

Board pads: inferred map above, continuity pending. Conditional PC-cable DE9 reference; actual cable match pending. Device RX optional (RMC only; NMEA-input option required); do not drive an RX already fed by another GPS. † L1 signal-reference connection requires cable/grounding match.

<details>
<summary>🖼️ Connector / module / ESP32 pictures and references</summary>

| ZS1 connector | RS-232-mod1 | ESP32 |
| --- | --- | --- |
| Matched view missing; private S25 crop unverified | [H-1-0290 image / inferred pad map](#hailege-board-pad-reference) | Exact board/header positions UNKNOWN |

**Unmatched drawing crop (S25):** connector **B** labels: 2 GND, 3 RX-PC, 4 TX-PC. Red-circled **A** uses RX-SR/TX-SR and other functions. Revision/view unverified; complete installation drawing and actual cable mapping still needed. This crop does not authorize wiring.

Source: [S22 original handbook](reference/ZS1_Deutsch.pdf), D3029, 16.09.2004, pp.46/48; conditional **PC-cable DE9** reference, not rear-panel B numbering. Cable/interface match pending. The 3037 supplement specifies 4,800 baud with the ZS1 FLARM function off, 19,200 with it on; installed version/setting unverified. RJ45/DE9 shape does not establish voltage or compatible pinning.

</details>

<a id="flarm"></a>

### 📡 FLARM

![Proposed device to module to ESP32 path](images/wiring-flarm.svg)

| Classic Power/Data reference† | → / ← | RS-232-mod2 · RS-232 pads | RS-232-mod2 · logic pads | → / ← | ESP32 |
| --- | :-: | --- | --- | :-: | --- |
| 5 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">)† | → | **L3** RS232 input | **R2** TTL output | → | GPIO26 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">) |
| *6 (<img src="images/pins/rx.svg" alt="RX" width="28" height="20">)* (optional) | *←* | *L4 RS232 output* | *R1 TTL input* | *←* | *GPIO27 (<img src="images/pins/tx.svg" alt="TX" width="28" height="20">) via FLARM TX link* |
| 7/8 (<img src="images/pins/gnd.svg" alt="GND" width="33" height="20">)† | — | L1 GND† | R4 GND | — | GND contact **UNKNOWN** |

† S23 Classic Power/Data reference only; installed port/cable view unverified. Use the manual's IGC numbering, not generic RJ45 numbering. Board map inferred; L1 ground connection conditional on reference/grounding match. Preserve displays/returns; no assumed splitter. FLARM supply pins 1/2 and 3 do not connect to the converter VCC; instrument power stays separate.

<details>
<summary>🖼️ Connector / module / ESP32 pictures and references</summary>

| FLARM connector | RS-232-mod2 | ESP32 |
| --- | --- | --- |
| Classic plug view: manual link below; actual cable unverified | [H-1-0290 image / inferred pad map](#hailege-board-pad-reference) | Exact board/header positions UNKNOWN |

<img src="images/powerflarm-core-reference-connectors.png" width="260" alt="PowerFLARM Core manual reference: connector locations">
<img src="images/powerflarm-core-reference-de9.png" width="390" alt="PowerFLARM Core DE9 reference: device socket left, cable plug right, mating faces">

**PowerFLARM Core examples only; not the reported Classic FLARM.** DE9: device socket left, cable plug right, mating faces. [FTD-033 v1.1, pp.11/14](https://www.flarm.com/wp-content/uploads/2024/04/FTD-033-PowerFLARM-Core-Installation-Manual.pdf#page=11), © FLARM Technology AG. Retained as reference, not installed pinning.

**Conditional Classic reference (S23):** [F6/F7/F8/F9 manual v221, p.6](https://www.flarm.com/wp-content/uploads/2024/04/FLARM_InstallationManual_D.pdf#page=6): RJ45 Power/Data, TX 5, RX 6, ground 7/8 (also 4); supply 1/2. IGC numbering is reversed relative to common numbering: match the manual's plug view and trace the actual cable. Extension is a separate RJ12 socket. Installed pins remain UNKNOWN pending match.

Source: S23, FTD-012 §5 and Classic installation manual pp.6/7/9. RS-232 is not ESP32 GPIO signalling. Confirm baud/output; TCP port 2000 does not establish serial settings. Preserve displays/returns; the Classic manual permits one attached device per data port, so no assumed passive splitter.

</details>

Both H-1-0290 boards: inferred pad map (S26/S27), sample continuity pending; no onboard DB9. Confirm signal direction with [unpowered continuity](bench-build.md#h-1-0290-identification). For receive-only use, disconnect the RS-232 output at the instrument; an open TX link does not disable it.

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
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod1 R3 (`+`) | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-232-mod2 R3 (`+`) | Proposed; module check pending |
| ESP32 3.3 V contact **UNKNOWN** | RS-422 mod IC pin 1, VCC | Proposed |
| ESP32 GND contact **UNKNOWN** | RS-232-mod1 R4 (`−`) | Proposed; continuity pending |
| ESP32 GND contact **UNKNOWN** | RS-232-mod2 R4 (`−`) | Proposed; continuity pending |
| ESP32 GND contact **UNKNOWN** | RS-422 mod IC pin 4, GND | Proposed |

Use one VCC/GND pair per H-1-0290; the listing reports shared power pads on both sides (S26), continuity pending. Disconnect BEC 5 V before USB powering. [Protection/current budget](bench-build.md#power-protection-and-budget) pending. Keep existing instrument supplies unchanged.

<a id="open-items"></a>

### ❓ Open items

| Needed | Resolves |
| --- | --- |
| Device connector photos + exact model/revisions | Correct pin view; FLARM/Zander/Becker applicability |
| Both H-1-0290 sides, IC markings + continuity map | Confirm inferred L/R pad map; equipment cable mapping |
| Exact ESP32 board/BEC + source limits | Header positions, current budget, protection |
| Return-traffic requirements + serial settings | Optional links and port configuration |
| Matched radio interface/reference/termination | Final radio circuit/harness |

Manual crops: [S20](sources.md#connector-reference-drawings). Installed connector matches unverified; no electrical acceptance.
