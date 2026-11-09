# Bench-build reference

**Unbuilt bench design.** Keep the existing assembly and harness as backup. [Device wiring](connections.md).

## Parts list

Parts for one bridge.

| Ref / quantity | Part or specification | Status / use |
| --- | --- | --- |
| MCU / 1 | AZ-Delivery ESP32 WROOM-32 | Selected family; model/regulator/header TBD. S17 vendor board is a reference only |
| RS-232-mod1, RS-232-mod2 / 2 | Hailege H-1-0290 RS-232/logic boards; 9.4 × 15.9 mm each | Selected (S26); reported MAX3232CSE. Verify both ICs, pad routing and 3.3 V operation |
| PS1 / 1 | BEC: reported 2S–4S input; 5 V output | Reported nominal 12 V battery; model, voltage range, polarity, tolerance, ripple/protection TBD. Old UBEC specs inapplicable |
| RS-422 mod / 1 | TI THVD1451D, SOIC-8 | Proposed full-duplex RS-422; always enabled; soldering required |
| Adapter / 1 | SOIC-8 to 2.54 mm breakout, e.g. Adafruit 1212 | Use the SOIC side. Check pins after soldering |
| C-R / 1 | 100 nF ceramic, at least 10 V | Close between RS-422 mod pins 1 and 4; H-1-0290 fitted capacitors need value/condition checks |
| R-Z, R-F, R-R / 3 | 10 kΩ, 1%, at least 0.125 W | Proposed pull-ups to 3.3 V for idle bias when the ESP32 TX output is high impedance |
| Bench interconnect / 1 set | Carrier/prototyping board, insulated wire, headers, standoffs and labelled test points | Solder transceiver and decoupling together. Keep differential test leads short |
| Navigation interconnect / 2 sets | Soldered leads/headers for H-1-0290 pads; equipment-side connectors TBD | Pad pitch, connector type/gender and actual cable mapping require inspection; no onboard DB9 assumed |
| Radio bench terminal / 1 | Labelled 5-way connector for A, B, Y, Z and logic reference | Lab use only; instrument connector and shielding remain TBD |
| F1, reverse-polarity stage, transient/OV protection | See power table below | Hold purchase until BEC, source and board limits are known |
| Equipment harness, termination, enclosure | Model-specific connectors, cable and resistors | Hold for matched manuals, cable lengths and enclosure space. No fixed 120 Ω termination is assumed |

Tools: multimeter, current-limited supply suitable for the BEC, oscilloscope. Use a logic analyser on logic-level signals only.

---

## H-1-0290 identification

Use one H-1-0290 per navigation device. Specifications are reported in S26.

| Property | Supplied specification / design use |
| --- | --- |
| IC | MAX3232CSE; actual marking/manufacturer unverified |
| Board dimensions | **9.4 × 15.9 mm** per supplied text. Height, pad pitch and clearance unknown |
| Supply | 3–5 V reported; **3.3 V** selected for this design |
| Rate | Listing recommends ≤120 kbps. Throughput and installed baud unverified |
| Power pads | Both sides reportedly connected; connect one VCC/GND pair per board after continuity checks |
| Signals | Independent channels reported. Arrows show flow. RS232/TTL mark levels. [Inferred pad map](connections.md#hailege-board-pad-reference) |

> [!WARNING]
> Before power, match the [chip-side photo and pad map](connections.md#hailege-board-pad-reference). Verify IC marking, package, pin 1 and datasheet. Trace all eight pads on both boards to IC pins. Verify common supply and ground pads and check for shorts. Stop and remap any mismatch.

S27 comparator, channel 2: L3→8, L4→7, R1→10, R2→9, both `+`→16 and both `−`→15. The generic 16-pin reference below is from S17.

| Function | MAX3232 channel 1 / channel 2 pins | Connection after confirming the used channel |
| --- | --- | --- |
| Supply | 16 VCC; 15 GND | H-1-0290 VCC → board 3.3 V; GND → common logic GND; verify continuity |
| Logic output from instrument | 12 R1OUT / 9 R2OUT | Trace to actual logic output pad; device connections in wiring guide |
| Logic input toward instrument | 11 T1IN / 10 T2IN | Trace to actual logic input pad. Wire directly to ESP32 TX where a return is required. Proposed 10 kΩ pull-up to 3.3 V |
| RS-232 input | 13 R1IN / 8 R2IN | Trace to RS-232-side pad and record its position; later receives instrument TX |
| RS-232 output | 14 T1OUT / 7 T2OUT | Trace to RS-232-side pad and record its position; initially leave off the instrument |
| Reference | 15 GND | Trace ground pads on both sides; equipment-reference/shield bonding is a separate question |

Record pad maps, shared contacts and deviations from S27. Trace equipment cables and check return paths separately.

---

## RS-422 circuit

**THVD1451D, SOIC-8** (S18). Common bench ground and local 100 nF decoupling. No galvanic isolation.

| RS-422 mod IC pin | Function / local components |
| --- | --- |
| 1 VCC | 3.3 V; C-R 100 nF to pin 4 |
| 2 R | Logic receiver output |
| 3 D | Logic driver input, wired directly to GPIO17 TX. R-R 10 kΩ pull-up to 3.3 V |
| 4 GND | Common bench logic ground |
| 5 Y | Non-inverting driver output, bench TX+ |
| 6 Z | Inverting driver output, bench TX− |
| 7 B | Inverting receiver input, bench RX− |
| 8 A | Non-inverting receiver input, bench RX+ |

With instruments unplugged, loop Y→A and Z→B to check converter transport. Match termination, protection and reference to the radio manual. Driver disable requires a different transceiver.

---

## Power protection and budget

Shunt clamp to return. Select BEC and protection parts before finalizing return routing and isolation.

| Function | Proposed requirement | Information needed to select it |
| --- | --- | --- |
| Input fuse F1 | Near source; protects upstream wiring | Wire/source ratings, BEC input/inrush, fuse curve; not output rating alone |
| Reverse-polarity protection | BEC-input diode/MOSFET protection | Source range, BEC minimum input, current/drop budget |
| Input transient clamp | TVS/source-impedance/fuse coordination | Normal maximum, BEC absolute maximum, pulse energy, loaded clamp voltage |
| BEC output overvoltage | Cutoff/eFuse if sustained BEC fault exceeds board limits | BEC fault behavior/regulator limits; fuse or TVS alone insufficient |
| Filtering / decoupling | Short leads/return loops; BEC-specified capacitors; filtering if measured noise requires | Stability, startup/ripple, interference; filter values TBD |
| USB servicing | Disconnect BEC 5 V before USB powering | Manual procedure; no added switching/OR-ing/back-feed circuitry |

**3.3 V budget** covers ESP32/Wi-Fi, peripherals, both H-1-0290 boards and the loaded RS-422 driver. Check regulator current and temperature under load. S19 gives ≥500 mA supply guidance. Verify spare header capacity separately. Estimated linear loss at 300 mA is 0.51 W.

Nominal 12 V source reported. Battery chemistry, voltage range, transients and BEC model are open.

---

## ESP32 constraints

Proposed GPIO allocation: [wiring guide](connections.md). Exact board/header positions UNKNOWN.

All three UARTs are allocated. Before connecting instruments, verify boot and GPIO remapping. Isolate application, framework and OTA output. S08 upstream RX/TX defaults are 21/1, 16/17 and 15/4.

---

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

---

## Bench checks

| Step | Work and pass condition | Stop condition |
| --- | --- | --- |
| 1 Identify | Exact ESP board/regulator, BEC voltage/current ratings, both H-1-0290 markings and continuity map recorded; RS-422 mod adapter orientation checked | Unknown supply pin, module direction or package |
| 2 Power only | Test BEC alone, then with a dummy load. Check startup, voltage, ripple and polarity against board limits. Verify protection before using the intended source | Wrong/unstable voltage, unexplained current or missing source/BEC limits |
| 3 Board and converter rails | Add ESP32, then converters one at a time. Check 5 V/3.3 V under Wi-Fi and converter load. Compare warm regulator temperature with its datasheet | Rail excursions, reset, excessive temperature or unbudgeted load |
| 4 Logic and loops | Unplug instruments. Verify ESP32 input levels. Test each UART and converter loop with counted bytes, including 00, A5, FF and fragmented traffic | Byte loss/change, debug bytes, back-power or incorrect levels |
| 5 Navigation | After WP1/WP2 matching, verify each feed and required return path, then both together against the recorded baseline | Changed device operation or loss of required messages |
| 6 Radio and coexistence | After protocol and software checks, verify standby, unchanged active channel, local controls and navigation through reconnect and power loss (WP7) | A successful write alone is insufficient; keep physical acceptance open |

Bench sequence pending.

---

## Existing instrument power

Preserve existing feeds; not supplied by the bridge. Zander/FLARM supply pins UNKNOWN. Becker reference only: DV14307.03 Issue 06 §2.5.3.1 p.58, P1 11/12 positive, 13/25 return; rear grounding stud p.57 (S10). Match installed variant before use; chassis/reference bonding UNKNOWN.

---

## Evidence

[Sources](sources.md) S09, S16–S19, S26–S27. [Documentation checks](reviews/2026-11-08-documentation.md).
