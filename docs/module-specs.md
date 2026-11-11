# Module specs

[Wiring guide](connections.md)

| Module | Power | Chip | Reported size |
| --- | --- | --- | --- |
| Hailege H-1-0290 (Zander and FLARM) | 3.3 V | MAX3232CSE (reported) | 9.4 × 15.9 mm |
| [Becker RS-422 board](#becker-max490-board) | 5 V | MAX490 (seller-reported) | 50 x 27 mm (reported) |
| AZ-Delivery ESP32 WROOM dev module | 5 V input, 3.3 V regulator | — | — |
| Hobbywing UBEC 3A (2–6S), 86010010 | 5.5–26 V input, jumper-selectable 5 V or 6 V at 3 A. Selected setting: 5 V | — | 43 × 17 × 7 mm (catalog) |

<a id="hobbywing-ubec"></a>

## Hobbywing UBEC 3A (2–6S)

<img src="images/hobbywing-ubec-3a-product.png" alt="Hobbywing UBEC with voltage jumper, ferrite ring and leads" width="320">
<img src="images/hobbywing-ubec-3a-dimensions.png" alt="Supplied UBEC image annotated 35 × 12 × 5 mm" width="320">

Verify clearance on the delivered unit including the jumper, leads, ferrite ring and connector. The owner-supplied image marks 35 × 12 × 5 mm around the pictured body. The dimension boundaries are unverified.

[Power module sources (S14, S16, S17, S19)](sources.md#source-register)

<a id="becker-max490-board"></a>

## Becker MAX490 board

**Selected for bench design.** The supplied photos show a blue board marked RS422 TO TTL, a header marked TXD/RXD/GND/VCC, and four screw terminals marked Y/Z/B/A. The seller reports a MAX490, 5 V supply and UART logic, power and activity LEDs, 2.5 Mbit/s, up to 1000 m, 15 kV ESD protection, 10 Ω series resistance, protection diodes and 120 Ω termination. Treat rate, distance and protection figures as separate claims. Verify chip identity, header directions, resistor placement, effective termination and protection ratings. [Evidence (S34)](sources.md#source-register)

**UART receive interface.** ESP32-WROOM-32 specifies VDD + 0.3 V as the upper input limit (3.6 V at nominal 3.3 V). A series resistor alone limits current only if there is a current path. The reviewed Espressif documents give no allowable continuous positive injection current for GPIO16 that would support sizing a resistor for a 5 V UART idle level. The seller's 10 Ω claim gives no basis for protecting this input.

**Proposed interface.** Feed the module's receiver output into a SN74LVC1G17 powered from the ESP32 3.3 V rail, with a common UART ground. The buffer accepts up to 5.5 V and specifies partial-power-down behavior. The ESP32 TX can drive the MAX490 DI directly if the module header routes to DI without incompatible circuitry. The MAX490 DI high threshold is 2.0 V. Verify header routing and startup/shutdown behavior before connecting to the radio. [Evidence (S34)](sources.md#source-register)

<a id="esp32-pinout"></a>

## ESP32 DevKitC V4 pinout

**Candidate for the AZ-Delivery WROOM-32 board.** View from the component side with the antenna at the top and Micro-USB at the bottom. J2 is on the left and J3 on the right. Count both headers from the antenna end. Header labels below follow Espressif's reference. RX and TX are from the ESP32's perspective.

<details>
<summary>AZ-Delivery pinout diagram</summary>

![AZ-Delivery ESP32 Dev Kit C V4 pinout, component side with Micro-USB at the bottom](images/esp32-devkitc-v4-az-delivery-pinout.png)

[AZ-Delivery pinout PDF, page 1](https://cdn.shopify.com/s/files/1/1509/1638/files/ESP32_DevKit_C_V4_Pinout.pdf?v=1615364529)

</details>

| Position | J2 label | J2 bridge use | J3 label | J3 bridge use |
| ---: | --- | --- | --- | --- |
| 1 | `3V3` | Navigation converters' 3.3 V supply | `GND` | Logic ground |
| 2 | `EN` | — | `IO23` | — |
| 3 | `VP` | — | `IO22` | — |
| 4 | `VN` | — | `TX` | UART0 TX (USB serial) |
| 5 | `IO34` | — | `RX` | UART0 RX (USB serial) |
| 6 | `IO35` | — | `IO21` | — |
| 7 | `IO32` | Zander RX | `GND` | Logic ground |
| 8 | `IO33` | Zander TX (optional) | `IO19` | — |
| 9 | `IO25` | — | `IO18` | — |
| 10 | `IO26` | FLARM RX | `IO5` | — |
| 11 | `IO27` | FLARM TX (optional) | `IO17` | Becker TX |
| 12 | `IO14` | — | `IO16` | Becker RX |
| 13 | `IO12` | — | `IO4` | — |
| 14 | `GND` | Logic ground | `IO0` | — |
| 15 | `IO13` | — | `IO2` | — |
| 16 | `D2` | — | `IO15` | — |
| 17 | `D3` | — | `D1` | — |
| 18 | `CMD` | — | `D0` | — |
| 19 | `5V` | 5 V BEC input | `CLK` | — |

> [!IMPORTANT]
> Before wiring, match the board's module marking, two 19-pin headers, silkscreen and power contacts to the reference. The proposed Becker pins GPIO16/17 are available on WROOM boards and reserved internally on WROVER boards. Leave D0–D3, CMD and CLK free for flash. Verify regulator capacity for the navigation converters.

[Espressif header tables and pin layout](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html#header-block) · [Evidence (S32)](sources.md#source-register)

<a id="hailege-board-pad-reference"></a>

## Hailege H-1-0290 · RS-232

| Chip-side reference | Inferred pad view |
| --- | --- |
| <img src="images/hailege-h-1-0290-board.svg" width="240" alt="Hailege H-1-0290 board, chip side"> | <img src="images/pinout-hailege.svg" width="420" alt="Hailege chip-side pad map. L1 ground, L2 supply, L3 RS-232 input, L4 RS-232 output. R1 TTL input, R2 TTL output, R3 supply, R4 ground. Continuity unknown."> |

[S26–S27](sources.md#source-register)
