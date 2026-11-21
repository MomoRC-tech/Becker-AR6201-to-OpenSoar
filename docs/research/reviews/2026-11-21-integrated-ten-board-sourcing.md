# 🔬 Ten-board sourcing check

The [original BOM audit](2026-11-20-integrated-jlcpcb-bom.md) records the initial selections. This follow-up checks the [front-panel schematic](../../../electronics/integrated/front-panel/compact-bridge.kicad_sch) at 6c6d4fe against JLCPCB’s public catalogue on November 21, 2026.

**37 production lines, 66 placements per board, 660 for ten.** J3 is normally unpopulated. TP1–TP17 are bare PCB pads.

## Assembly cost

| Service | Feeder loading per distinct SMT part on the order | Impact at ten boards |
| --- | --- | --- |
| Economic | Basic and Preferred Extended $0, ordinary Extended $3.07 | $0.307 per board for each ordinary Extended feeder |
| Standard | JLC lists $1.53 per Basic or Extended feeder | $0.153 per board per feeder. The selected ESP32-WROOM-32E-H4 requires Standard assembly. |

On Standard, using the same approved SKU in several positions reduces feeder count. Switching one Extended SKU to one Basic SKU can lower its unit price while retaining the same published feeder charge. J2 is listed for wave soldering. Quote the THT operation separately, along with J1 fitting, stencil, excess components, U1/U4 paste and via treatment. [JLCPCB assembly price](https://jlcpcb.com/help/article/pcb-assembly-price) · [Preferred Extended policy](https://jlcpcb.com/help/article/pcb-assembly-faqs)

The original audit identifies 19 exact SKUs. Eighteen are SMT and contribute $27.54 in Standard feeder charges for the entire run. J2 is the wave-soldered exception. This is a subtotal before the unassigned parts. [JLCPCB fee explanation](https://jlcpcb.com/blog/reduce-pcb-assembly-cost-prototype)

## Production BOM

**Need** is the fitted quantity for ten. **Stock** is the exact SKU’s JLCPCB catalogue stock field (overseasStockCount) when retrieved. **USD/pc** uses JLC’s published quantity tier at that fitted quantity. Purchasing minimums, assembly attrition and order allocation can change the invoiced quantity and price. A listed price at zero stock is indicative only.

| Ref. | Need | Catalogue SKU and MPN | Tier | Stock | USD/pc | Match or sourcing action |
| --- | ---: | --- | --- | ---: | ---: | --- |
| C1 | 10 | [C15849](https://jlcpcb.com/partdetail/C15849) · CL10A105KB8NNNC | Basic | 11,747,043 | 0.0092 | Basic candidate, 1 µF / 50 V X5R, 0603. EN timing. |
| C2 | 10 | [C15850](https://jlcpcb.com/partdetail/C15850) · CL21A106KAYNNNE | Basic | 8,930,785 | 0.0311 | Basic candidate, 10 µF / 25 V X5R, 0805. ESP32 rail. |
| C3–C9, C18, C20, C22, C23 | 110 | [C14663](https://jlcpcb.com/partdetail/C14663) · CC0603KRX7R9BB104 | Basic | 53,728,435 | 0.0056 | Basic candidate, 100 nF / 50 V X7R, 0603. |
| C10 | 10 | [C2844757](https://jlcpcb.com/partdetail/C2844757) · C1608X7R1C104K | Extended | 0 | 0.0007 | TDK catalogue omits the full suffix. Zero stock. See bootstrap alternative below. |
| C11 | 10 | [C2167399](https://jlcpcb.com/partdetail/C2167399) · C1608X7R1C105K080AC | Extended | 4 | 0.0311 | Selected 1 µF / 16 V X7R. Six short before losses. |
| C12,C19 | 20 | [C49296968](https://jlcpcb.com/partdetail/C49296968) · C3225X7R2A106KT000E | Extended | 7,843 | 0.4652 | Selected, exact catalogue match. |
| C13 | 10 | [C445721](https://jlcpcb.com/partdetail/C445721) · C1608X7R1H224KT000N | Extended | 7,891 | 0.0458 | Selected, exact catalogue match. |
| C14-C17 | 40 | [C2175584](https://jlcpcb.com/partdetail/C2175584) · C3225X7R1E226M250AB | Extended | 0 | 0.8765 | Selected 22 µF / 25 V X7R. Forty short. |
| C21 | 10 | [C19666](https://jlcpcb.com/partdetail/C19666) · CL10A475KO8NNNC | Basic | 3,178,292 | 0.0144 | Basic candidate, 4.7 µF / 16 V X5R, 0603. CP2102N rail. |
| D1 | 10 | [C19077410](https://jlcpcb.com/partdetail/C19077410) · BZT52C12 | Extended | 314,629 | 0.0169 | Selected, exact catalogue match. |
| D2 | 10 | [C151257](https://jlcpcb.com/partdetail/C151257) · SMBJ20A | Extended | 5,659 | 0.1848 | Selected, exact catalogue match. |
| D3 | 10 | [C7519](https://jlcpcb.com/partdetail/C7519) · USBLC6-2SC6 | Extended | 44,183 | 0.1558 | Selected, exact catalogue match. |
| D4 | 10 | [C125113](https://jlcpcb.com/partdetail/C125113) · LTST-S270KGKT | Extended | 2,185 | 0.0288 | Exact MPN, green side-emitting LED. |
| D5 | 10 | [C125114](https://jlcpcb.com/partdetail/C125114) · LTST-S270KRKT | Extended | 34,090 | 0.0319 | Exact MPN, red side-emitting LED. |
| J1 | 10 | D15P33E4GV00LF | — | — | — | Exact JLC assembly SKU unresolved. See below. |
| J2 | 10 | [C431092](https://jlcpcb.com/partdetail/C431092) · XT30PW-M30.G.Y | Extended | 95,221 | 0.3346 | Selected, wave soldering. |
| J4 | 10 | [C5291559](https://jlcpcb.com/partdetail/C5291559) · USB4145-03-0230-C | Extended | 422 | 1.1311 | Exact upright GCT MPN. JLC flags high assembly difficulty. |
| L1 | 10 | [C3911557](https://jlcpcb.com/partdetail/C3911557) · XGL6030-682MEC | Extended | 67 | 5.1397 | Selected, 6.8 µH. |
| Q1 | 10 | [C154901](https://jlcpcb.com/partdetail/C154901) · DMP6023LE-13 | Extended | 2,715 | 0.7835 | Selected, exact catalogue match. |
| Q2,Q3 | 20 | [C20526](https://jlcpcb.com/partdetail/C20526) · MMBT3904(RANGE:100-300) | Basic | 4,182,481 | 0.0103 | Basic JSCJ candidate. B/E/C on pins 1/2/3. |
| R1,R2,R5,R11,R19,R20 | 60 | [C25804](https://jlcpcb.com/partdetail/C25804) · 0603WAF1002T5E | Basic | 43,064,603 | 0.0017 | Candidate, 10k in 0603. |
| R3 | 10 | [C23162](https://jlcpcb.com/partdetail/C23162) · 0603WAF4701T5E | Basic | 19,437,905 | 0.0028 | Candidate, 4.7k in 0603. |
| R4 | 10 | [C21189](https://jlcpcb.com/partdetail/C21189) · 0603WAF0000T5E | Basic | 15,728,188 | 0.0021 | Candidate, 0R in 0603. |
| R6,R8,R10,R12 | 40 | [C25803](https://jlcpcb.com/partdetail/C25803) · 0603WAF1003T5E | Basic | 13,133,239 | 0.0029 | Candidate, 100k in 0603. |
| R7 | 10 | 43.3k | — | — | — | Exact JLC assembly SKU unresolved. See below. |
| R9 | 10 | [C1854558](https://jlcpcb.com/partdetail/C1854558) · TNPW060311K1BEEA | Extended | 6,439 | 0.0763 | Candidate, 11.1 kΩ ±0.1%. |
| R13,R14 | 20 | [C23186](https://jlcpcb.com/partdetail/C23186) · 0603WAF5101T5E | Basic | 18,478,003 | 0.0015 | Candidate, 5.1k in 0603. |
| R15 | 10 | [C25961](https://jlcpcb.com/partdetail/C25961) · 0603WAF2212T5E | Extended | 183,236 | 0.0019 | Candidate, 22.1 kΩ ±1%. |
| R16 | 10 | [C23061](https://jlcpcb.com/partdetail/C23061) · 0603WAF4752T5E | Extended | 91,566 | 0.0019 | Candidate, 47.5 kΩ ±1%. |
| R17,R18,R21-R23 | 50 | [C21190](https://jlcpcb.com/partdetail/C21190) · 0603WAF1001T5E | Basic | 40,934,449 | 0.0016 | Candidate, 1k in 0603. |
| U1 | 10 | [C3013935](https://jlcpcb.com/partdetail/C3013935) · ESP32-WROOM-32E-H4 | Extended | 2,963 | 3.8993 | Selected H4. Standard PCBA only. |
| U2 | 10 | [C2652868](https://jlcpcb.com/partdetail/C2652868) · TRS3232EIPWR | Extended | 11,304 | 0.6268 | Selected, exact catalogue match. |
| U3 | 10 | [C2865785](https://jlcpcb.com/partdetail/C2865785) · SN65HVD1470DR | Extended | 12 | 1.5016 | Only two above fitted demand. Confirm attrition before booking. |
| U4 | 10 | [C1852147](https://jlcpcb.com/partdetail/C1852147) · LMR33610ADDAR | Extended | 90 | 1.5783 | Selected, exact catalogue match. |
| U5 | 10 | [C2673443](https://jlcpcb.com/partdetail/C2673443) · TMUX1574PWR | Extended | 18,730 | 0.7068 | Selected, exact catalogue match. |
| U6,U7 | 20 | [C54800033](https://jlcpcb.com/partdetail/C54800033) · MX5050S6030A1 | Extended | 2,774 | 0.4032 | Selected, exact catalogue match. |
| U8 | 10 | [C969151](https://jlcpcb.com/partdetail/C969151) · CP2102N-A02-GQFN24R | Extended | 21,065 | 1.7187 | Same A02 QFN24 device in tape and reel. Candidate ordering suffix. |

## Proposed alternatives

| Position | Alternative | Tier | JLC stock | USD/pc | Engineering check before assignment |
| --- | --- | --- | ---: | ---: | --- |
| C10 | [C14663](https://jlcpcb.com/partdetail/C14663) · CC0603KRX7R9BB104 | Basic | 53,728,435 | 0.0056 | Share the eleven 100 nF sites’ Yageo 50 V X7R, ±10%, 0603. Preserves the BOOT_SW-to-SW connection. Total 120 if shared. |
| C11 | [C106858](https://jlcpcb.com/partdetail/C106858) · CC0603KRX7R8BB105 | Extended | 244,550 | 0.0180 | 1 µF, 25 V, X7R, ±10%, 0603. Retains the VCC bias capacitor dielectric. Using this part also at C1 would combine two 1 µF feeders. |
| C14–C17 | [C49118556](https://jlcpcb.com/partdetail/C49118556) · TCC1210X7R226K250MT | Extended | 60,010 | 0.1728 | 22 µF, 25 V, X7R, ±10%, 1210. Compare effective capacitance at 3.3 V across tolerance, temperature and aging against U4 stability and transient requirements. Check assembled height. |
| C21 | [C71633](https://jlcpcb.com/partdetail/C71633) · GRM188R61A475KE15D | Extended | 302,752 | 0.0311 | 4.7 µF, 10 V X5R, 0603. Extended fallback to the Basic Samsung above. Check capacitance at 3.3 V. |
| R7 | [C23053](https://jlcpcb.com/partdetail/C23053) · UNI-ROYAL 0603WAF4322T5E | Extended | 31,242 | 0.0018 | Value-change proposal. 43.2 kΩ ±1%, 0603. With R6 at 100 kΩ and nominal 1 V feedback, the ideal output moves from 3.3095 to 3.3148 V (+5.35 mV). TI Table 9-2 lists this pair. Check the tolerance budget before approval. |
| Q2/Q3 | [C3200105](https://jlcpcb.com/partdetail/C3200105) · MMBT3904 | Extended | 0 | 0.0319 | Extended onsemi listing has zero stock. Prefer the Basic JSCJ candidate above for the reset pair. |

- **J1.** Keep the Amphenol D15P33E4GV00LF and quote hand fitting or consignment of ten. Amphenol lists $2.997 each at ten, $29.97 before shipping. Its distributor table reported 2,835 at DigiKey on November 16. [DigiKey Germany](https://www.digikey.de/en/products/detail/amphenol-icc-fci/D15P33E4GV00LF/1001806) No JLC assembly stock was confirmed for this MPN. [Amphenol connector and distributor list](https://www.amphenol-cs.com/product/d15p33e4gv00lf.html)
- **Q2/Q3 and U8.** The JSCJ SOT-23 transistor places base on 1, emitter on 2 and collector on 3. Silicon Labs defines the CP2102N ordering suffix R as tape and reel. Both options fit the existing design. [JSCJ pinout](https://www.jscj-elec.com/gallery/file/MMBT3904%20SOT-23V2.pdf) · [Silicon Labs ordering guide](https://www.silabs.com/documents/public/data-sheets/cp2102n-datasheet.pdf)
- **MLCCs.** Verify effective capacitance for each rail’s DC bias and temperature, particularly the buck output bank and the ESP32/CP2102N supplies. [TI LMR33610](https://www.ti.com/lit/ds/symlink/lmr33610.pdf) · [Samsung C21 data](https://product.samsungsem.com/mlcc/CL10A475KO8NNN.do)

**What the proposed parts would cost for ten.** Use the primary table with C106858 at both C1 and C11, C14663 at all twelve 100 nF sites, C49118556 for the four output capacitors and C23053 for R7. At fitted quantities the JLC components total $206.01, including J2. Ten externally sourced J1 connectors at the manufacturer’s ten-piece price add $29.97. The 33 distinct SMT feeders add $50.49 on Standard. Combined subtotal **$286.47 for ten ($28.65 per board)** before PCB fabrication, assembly setup, stencil, soldering operations, attrition, shipping and tax. The U3 stock margin and output-capacitor check still apply. No SKU in this example carries the Preferred Extended designation.

All alternatives are proposals for the owner’s component decision. The KiCad part fields retain the original selections.
