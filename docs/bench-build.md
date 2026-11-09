# Bench-build reference

**Unbuilt proposal.** [Wiring guide](connections.md) owns device connections. Existing assembly/harness stays unchanged as backup.

## Parts list

Quantities: one new bridge. Product links are references; no purchases.

| Ref / quantity | Part or specification | Status / use |
| --- | --- | --- |
| MCU / 1 | AZ-Delivery ESP32 WROOM-32 | Selected family; model/regulator/header TBD. S17 vendor board is a reference only |
| RS-232-mod1, RS-232-mod2 / 2 | HW-044 DB9-to-logic modules matching the supplied photo | Selected; apparent MAX3232. Verify both ICs, routing and 3.3 V operation |
| PS1 / 1 | BEC: reported 2S–4S input; 5 V output | Reported nominal 12 V battery; model, voltage range, polarity, tolerance, ripple/protection TBD. Old UBEC specs inapplicable |
| RS-422 mod / 1 | TI THVD1451D, SOIC-8 | Proposed full-duplex RS-422; always enabled; soldering required |
| Adapter / 1 | SOIC-8 to 2.54 mm breakout, e.g. Adafruit 1212 | Use its SOIC side; verify pin numbering after soldering. A pack may contain more than one board |
| C-R / 1 | 100 nF ceramic, at least 10 V | Close between RS-422 mod pins 1 and 4; HW-044 capacitors are already fitted but their values/condition need inspection |
| R-Z, R-F, R-R / 3 | 10 kΩ, 1%, at least 0.125 W | Proposed pull-ups from each converter's logic data input to 3.3 V; retain an idle input when its TX jumper is open |
| Zander TX link, FLARM TX link, Becker TX link / 3 | Removable 2-pin links, 2.54 mm, with labelled shunts | ESP32 TX-to-converter-input links; initially open. These are not power switches or driver-disable controls |
| Bench interconnect / 1 set | Carrier/prototyping board, insulated wire, headers, standoffs and labelled test points | Solder the transceiver/decoupling locally; keep differential test leads short. No production PCB required for this increment |
| DB9 access / 2 | Breakout connectors mating with the actual HW-044 sockets/plugs | Choose gender after inspection; access contacts by their moulded numbers, not a guessed front/rear view |
| Radio bench terminal / 1 | Labelled 5-way connector for A, B, Y, Z and logic reference | Lab use only; instrument connector and shielding remain TBD |
| F1, reverse-polarity stage, transient/OV protection | See power table below | Functions specified; ratings and exact components held for BEC/source/board data. This part of the BOM is not ready to order |
| Equipment harness, termination, enclosure | Model-specific connectors, cable and resistors | Hold for matched manuals, cable lengths and enclosure space. No fixed 120 Ω termination is assumed |

Tools required: multimeter, current-limited BEC-compatible supply, oscilloscope. Logic analyser: logic-level signals only. Availability unknown.

## HW-044 identification

**Unpowered continuity first.** Trace RXD/TXD labels; no assumed direction. MAX3232: 16-pin top view (S17); confirm IC marking/package and pin 1.

| Function | MAX3232 channel 1 / channel 2 pins | Connection after confirming the used channel |
| --- | --- | --- |
| Supply | 16 VCC; 15 GND | HW-044 VCC → board 3.3 V; GND → common logic GND; verify continuity |
| Logic output from instrument | 12 R1OUT / 9 R2OUT | Trace to actual RXD/TXD header contact; device connections in wiring guide |
| Logic input toward instrument | 11 T1IN / 10 T2IN | Trace to other header contact; 10k pull-up to 3.3 V on converter side of TX link |
| RS-232 input | 13 R1IN / 8 R2IN | Trace to DB9 contact and record its number; later receives instrument TX |
| RS-232 output | 14 T1OUT / 7 T2OUT | Trace to DB9 contact and record its number; initially leave off the instrument |
| Reference | 15 GND | Trace to DB9 reference contact; connector shell/shield bonding is a separate question |

Record header-to-IC-to-DB9 mapping, reference, power and tied contacts for each board. No assumed straight-through cable. Return path checked separately. Module supply: 3.3 V only for this design.

## RS-422 circuit

RS-422 mod: **THVD1451D, SOIC-8** (S18), not the unidentified blue module. Local 100 nF decoupling; common bench logic ground; no galvanic isolation.

| RS-422 mod IC pin | Function / local components |
| --- | --- |
| 1 VCC | 3.3 V; C-R 100 nF to pin 4 |
| 2 R | Logic receiver output |
| 3 D | Logic driver input; R-R 10k to 3.3 V, converter side of Becker TX link |
| 4 GND | Common bench logic ground |
| 5 Y | Non-inverting driver output, bench TX+ |
| 6 Z | Inverting driver output, bench TX− |
| 7 B | Inverting receiver input, bench RX− |
| 8 A | Non-inverting receiver input, bench RX+ |

Local loopback, instruments disconnected: Y→A, Z→B. Tests converter transport only. Termination/protection/reference require matched manual; no assumed 120 Ω resistor. A disabled transmitter requires a different device and pin table.

## Power protection and budget

Transient clamp: shunt to return. Return routing/isolation depends on BEC/protection parts. Protection circuit incomplete.

| Function | Proposed requirement | Information needed to select it |
| --- | --- | --- |
| Input fuse F1 | Near source; protects upstream wiring | Wire/source ratings, BEC input/inrush, fuse curve; not output rating alone |
| Reverse-polarity protection | BEC-input diode/MOSFET protection | Source range, BEC minimum input, current/drop budget |
| Input transient clamp | TVS/source-impedance/fuse coordination | Normal maximum, BEC absolute maximum, pulse energy, loaded clamp voltage |
| BEC output overvoltage | Cutoff/eFuse if sustained BEC fault exceeds board limits | BEC fault behavior/regulator limits; fuse or TVS alone insufficient |
| Filtering / decoupling | Short leads/return loops; BEC-specified capacitors; filtering if measured noise requires | Stability, startup/ripple, interference; filter values TBD |
| USB servicing | Disconnect BEC 5 V before USB powering | Manual procedure; no added switching/OR-ing/back-feed circuitry |

**3.3 V budget:** ESP32/Wi-Fi + board peripherals + 2 × HW-044/LEDs + loaded RS-422 driver. Espressif’s ≥500 mA supply guidance is not spare header capacity (S19). Verify regulator current/thermal limits under load. Linear loss ≈ `(5.0 − 3.3) × total current`; 300 mA → 0.51 W (estimate).

Source: reported nominal 12 V battery; chemistry, actual range/transients and BEC model unknown. Equipment power wiring remains separate from bridge power.

## ESP32 constraints

Proposed GPIO allocation: [wiring guide](connections.md). Exact board/header positions UNKNOWN.

All three UARTs allocated; no spare debug UART. Isolate application/framework/OTA output and verify boot/remapping before equipment connection. Upstream RX/TX defaults 21/1, 16/17, 15/4 are not this design (S08).

## Cable construction

| Cable rule | Requirement |
| --- | --- |
| Branches | Separate labels, connectors, strain relief; new harness; preserve backup |
| RS-422 | Separate TX/RX twisted pairs; impedance/termination/reference/shield TBD |
| Logic | Keep 3.3 V within bridge; matched equipment-level signals externally |
| Navigation | Preserve returns/displays; verify loading before fan-out |
| Power | Size wiring/protection from source and load |
| UART0 | GPIO remap plus diagnostic/startup isolation |

Classify proposed connections: **compatible / change needed / unresolved**, with evidence and affected function.

## Bench checks

| Step | Work and pass condition | Stop condition |
| --- | --- | --- |
| 1 Identify | Exact ESP board/regulator, BEC voltage/current ratings, both HW-044 markings and continuity map recorded; RS-422 mod adapter orientation checked | Unknown supply pin, module direction or package |
| 2 Power only | Test BEC without electronics first, then under a suitable dummy load; measure startup, steady output, ripple and polarity against the selected board's permitted input. Verify fuse/protection design before using the intended source | Wrong/unstable voltage, unexplained current or missing source/BEC limits |
| 3 Board and converter rails | Add ESP32, then converters individually. Check 5 V/3.3 V during Wi-Fi load, converter activity and warm operation; measure regulator temperature and compare with its datasheet | Rail excursions, reset, excessive temperature or unbudgeted load |
| 4 Logic and loops | With equipment disconnected, verify converter outputs stay within the ESP32 input limits. Later test each UART and converter loop with counted binary patterns including 00, A5, FF and fragmented traffic | Byte loss/change, debug bytes, back-power or incorrect levels |
| 5 Navigation | After WP1/WP2 matching, verify each feed and required return path, then both together against the recorded baseline | Changed device operation or loss of required messages |
| 6 Radio and coexistence | After protocol/software prerequisites, WP7 checks actual standby, unchanged active, local controls and navigation under reconnect/power loss | A successful write alone is insufficient; keep physical acceptance open |

Sequence unexecuted. Isolate UART0 diagnostics before instrument connection. Firmware/harness implementation pending.

## Existing instrument power

Preserve existing feeds; not supplied by the bridge. Zander/FLARM supply pins UNKNOWN. Becker reference only: DV14307.03 Issue 06 §2.5.3.1 p.58, P1 11/12 positive, 13/25 return; rear grounding stud p.57 (S10). Match installed variant before use; chassis/reference bonding UNKNOWN.

## Evidence

S09/S16–S19 in [sources](sources.md); Becker pin evidence: prior review. Existing component-pin review retained; no new electrical/device tests. [Documentation verification](reviews/2026-11-08-documentation.md).
