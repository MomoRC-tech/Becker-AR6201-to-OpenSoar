# Becker bridge · EasyEDA Pro

Open [the editable EasyEDA Pro project](Becker-bridge.epro2) to inspect the schematic and the routed PCB. The [four-layer copper drawing](routing.svg) comes from the native Gerber and drill export. Each layer is shown from above, with the front panel at the bottom.

| Layout | Result |
| --- | --- |
| Board outline | 58.50 × 45.83 mm, down from 58.50 × 46.50 mm |
| Mechanical references | J1 (DA-15), J2 (XT30) and both mounting-hole centres retained. The ESP32 antenna projects 6.20 mm beyond the rear board edge. |
| Routing | 50 routed nets, 1,020 track segments and 128 vias. Top, inner 2 and bottom carry signals or power. Inner 1 is the ground plane. |
| Front-panel return | Continuous 2.10 mm bottom copper connects J1 pins 3, 6 and 7 to J2 pin 1. |
| Fitted parts | 55 per board across 27 order codes, including the approved 100 nF C27 at D3. J3, J5 and R16 are unpopulated options. Routing introduced no component substitutions or additional placements. |

## Manufacturer layout guidance

| Device | Guidance applied |
| --- | --- |
| TI TPS62933P buck (U4) | Placed the input bypass C13 and bootstrap C10 at their pin pairs. The switch node reaches L1 directly. The output capacitors sit beside the inductor. Review feedback pickup and switching noise on the bench. [TI layout examples, §12](https://www.ti.com/lit/ds/symlink/tps62933p.pdf#page=40) |
| ST USBLC6-2SC6 (D3) | Routed the USB signals from J4 through the protection device toward U8. C27 bypasses D3 pin 5 to ground beside the clamp. [ST layout drawing, Figure 18](https://atta.szlcsc.com/upload/public/pdf/source/20140724/1457707160627.pdf#page=9) |
| WCH CH9102F USB UART (U8) | Faced the USB pins toward D3, routed the protected pair together and placed local supply bypass nearby. Verify the USB geometry against the selected fabrication stack-up. [WCH application circuit and guidance, §8.1](https://atta.szlcsc.com/upload/public/pdf/source/20251203/8244E9DADC2897AE99C54F027EA73027.pdf#page=8) |
| MaxLinear SP3232E (U2) | Grouped the charge-pump capacitors around the flying and reservoir pins and placed C7 at the supply pins. [MaxLinear application circuit, Figure 7](https://www.maxlinear.com/ds/sp3222e_sp3232e.pdf#page=7) |
| Espressif ESP32-WROOM-32E (U1) | Placed the antenna beyond the carrier edge, clear of carrier copper. The enclosure and installed wiring still need an RF check. [Espressif module placement guidance](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32/pcb-layout-design.html) |
| Maxinmicro MX5050S6030A1 (U6/U7) | Connected both OUT exposed pads to SYS_VIN. The manufacturer supplies the pinout and application circuit. [Maxinmicro pin table and application circuit](https://atta.szlcsc.com/upload/public/pdf/source/20260416/F783EFBB1E9B019FBA93660DE65BE074.pdf#page=5) |

## Checks and assembly review

| Check | Result |
| --- | --- |
| Reopened EasyEDA PCB | Zero airwires. The native design-rule check reported zero copper-clearance or open-connection findings. |
| Pin mapping | All 263 source pin assignments, including C27, matched the live EasyEDA schematic and all 287 numbered PCB pad occurrences. |
| Schematic synchronization | EasyEDA reports one netlist error. Its import dialog proposes removing H1 and H2. These are PCB-only mechanical mounting holes. Retain both when importing future schematic changes. |
| Local fine-pitch clearance | General clearance is 0.25 mm. Top-layer rule regions at U4/U8/J4 allow 0.22 mm pad-to-track. Pad-to-pad is 0.20 mm at U4/J4 and 0.225 mm at U8. U4/U8 allow 0.24 mm track-to-track. |
| Before assembly | Resolve the J1 exact device match, J2 XT30 drawing revision and J4 shell-slot/land differences. Check connector and antenna fit against the enclosure, the USB stack-up and the JLCPCB assembly preview. Bench checks include power, temperature, USB and radio operation. |

The fitted component cost is the same as the approved C27 design. Fabrication and assembly pricing for this outline have not been quoted.
