<a id="top"></a>

# 🧱 Modules and parts

🏠 [Overview](../../README.md) › 🔧 Hardware › **Modules and parts**

The modules the bridge is built from, how to orient them, and the shopping list for one bridge.

> [!NOTE]
> **Status:** all modules selected. Electrical specs come from seller listings or datasheets; physical sizes are recorded in [housing](housing.md#contents). Open checks: [bench hardware](../project/open-items.md#bench-hardware).

📑 **On this page:** <kbd>[1 · 📋 Module overview](#module-overview)</kbd> <kbd>[2 · 🧠 ESP32 board](#esp32-pinout)</kbd> <kbd>[3 · 🔁 RS-232 modules (Zander, FLARM)](#rs232-module)</kbd> <kbd>[4 · 📻 RS-422 board (Becker)](#rs422-board)</kbd> <kbd>[5 · 🔋 Power module (UBEC)](#ubec)</kbd> <kbd>[6 · 🛒 Parts list](#parts-list)</kbd>

Related: 🔌 [Signal wiring](wiring.md) · ⚡ [Power and grounding](power.md) · 📦 [Housing](housing.md) · 📚 [Sources](../research/sources.md)

---

<a id="module-overview"></a>

## 1. 📋 Module overview

| Module | Use | Supply | Chip | Size |
| --- | --- | --- | --- | --- |
| [AZ-Delivery ESP32 WROOM dev module](#esp32-pinout) | Bridge controller | 5 V in, 3.3 V on board | ESP32-WROOM-32 | [Measured PCB size](housing.md#contents) |
| [Hailege H-1-0290](#rs232-module) (2×) | RS-232 for Zander and FLARM | 3.3 V | MAX3232CSE | [Measured size](housing.md#contents) |
| [RS-422 board](#rs422-board) | Becker radio | 5 V | MAX490 | 50 × 27 mm |
| [Hobbywing UBEC 3A (2–6S)](#ubec), part 86010010 | 12 V → 5 V | 5.5–26 V in; 5 V or 6 V out (jumper) at 3 A | — | 43 × 17 × 7 mm |

---

<a id="esp32-pinout"></a>

## 2. 🧠 ESP32 board

The AZ-Delivery board follows Espressif's DevKitC V4 layout (S32). View from the component side, antenna at the top, Micro-USB at the bottom: J2 is the left header, J3 the right; both are counted from the antenna end. RX and TX are from the ESP32's side.

| Position | J2 label | J2 bridge use | J3 label | J3 bridge use |
| ---: | --- | --- | --- | --- |
| 1 | `3V3` | **3.3 V to RS-232-mod2 R3, then mod1** | `GND` | **Ground: RS-232-mod2 R4, then mod1** |
| 2 | `EN` | — | `IO23` | — |
| 3 | `VP` | — | `IO22` | — |
| 4 | `VN` | — | `TX` | USB serial (boot only) |
| 5 | `IO34` | **Supply voltage sense** via R-V1, R-V2 and C-V to J2-14 ([circuit](power.md#voltage-sense)) | `RX` | USB serial (boot only) |
| 6 | `IO35` | — | `IO21` | — |
| 7 | `IO32` | **Zander RX** (UART0) | `GND` | **Ground: R-B2** |
| 8 | `IO33` | Not connected | `IO19` | — |
| 9 | `IO25` | — | `IO18` | — |
| 10 | `IO26` | **FLARM RX** (UART1) | `IO5` | — |
| 11 | `IO27` | Not connected | `IO17` | **Becker TX** (UART2), direct to MAX490 TXD |
| 12 | `IO14` | — | `IO16` | **Becker RX** via R-B1 (UART2), R-B2 to J3-7 |
| 13 | `IO12` | — | `IO4` | — |
| 14 | `GND` | **Ground: central GND-J** | `IO0` | — |
| 15 | `IO13` | — | `IO2` | — |
| 16 | `D2` | — | `IO15` | — |
| 17 | `D3` | — | `D1` | — |
| 18 | `CMD` | — | `D0` | — |
| 19 | `5V` | **5 V from central 5V-J** | `CLK` | — |

GPIO16/17 are free only on WROOM boards (WROVER boards use them for PSRAM). D0–D3, CMD and CLK belong to the flash; leave them free. The pin plan avoids the boot strapping pins (GPIO0, 2, 5, 12, 15). [Espressif header tables](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html#header-block)

<details>
<summary><b>🖼️ Show the AZ-Delivery pinout diagram</b> (click to expand)</summary>

![AZ-Delivery ESP32 Dev Kit C V4 pinout, component side with Micro-USB at the bottom](../images/esp32-devkitc-v4-az-delivery-pinout.png)

[AZ-Delivery pinout PDF, page 1](https://cdn.shopify.com/s/files/1/1509/1638/files/ESP32_DevKit_C_V4_Pinout.pdf?v=1615364529)

</details>

---

<a id="rs232-module"></a>

## 3. 🔁 RS-232 modules (Zander, FLARM)

Hailege H-1-0290: a MAX3232 RS-232 ↔ 3.3 V converter with edge pads. The pad map follows the board marks: +/− for supply and ground, arrows for signal direction; the TTL output is the pad whose arrow points off the board (S26). Verify it during [bench bring-up](wiring.md#bring-up).

| Chip side | Pad map |
| --- | --- |
| <img src="../images/hailege-h-1-0290-board.svg" width="240" alt="Hailege H-1-0290 board, chip side"> | <img src="../images/pinout-hailege.svg" width="420" alt="Hailege chip-side pad map. L1 ground, L2 supply, L3 RS-232 input, L4 RS-232 output. R1 TTL output, R2 TTL input, R3 supply, R4 ground."> |

> [!IMPORTANT]
> The RS-232 and TTL sides share one 3.3 V net (L2 ↔ R3) and one ground net (L1 ↔ R4). Use R3 and R4 on each board for the [ESP32-fed daisy chain](power.md#supply). Leave both L1 and both L2 pads unwired.

---

<a id="rs422-board"></a>

## 4. 📻 RS-422 board (Becker)

A blue "RS422 TO TTL" board with a 4-pin header (TXD/RXD/GND/VCC) and four screw terminals (Y/Z/B/A), S34; PCB marked QYF-998, V1165 (S145). The seller lists a MAX490, 5 V supply, 120 Ω termination and ESD protection.

View from the component side, with the TTL header on the right and the screw terminals on the left.

| Component side | Pin map |
| --- | --- |
| <img src="../images/rs422-to-ttl-board.svg" width="320" alt="Selected blue RS422 TO TTL board, component side; TTL header at right and screw terminals at left."> | <img src="../images/pinout-rs422-to-ttl.svg" width="420" alt="RS-422 component-side pin map: right header top to bottom TXD, RXD, GND, VCC; left terminals top to bottom Y output positive, Z output negative, B input negative, A input positive. TXD is DI; RXD is RO (S144)."> |

| Terminal | MAX490 function |
| --- | --- |
| A / B | Receiver input + / − (from the radio's TX2) |
| Y / Z | Driver output + / − (to the radio's RX2) |
| Header RXD / TXD | RXD = receiver output (RO); TXD = driver input (DI), S144. Each pin has its own 1 kΩ and LED to VCC (S137, S145); on DI this pull-up holds 3.1 V while GPIO17 is released |
| Line parts | One TVS diode and one 100 Ω series resistor per line. Each pair has a 120 Ω resistor that ends on an isolated pad, so neither pair is terminated as delivered (S145) |
| Header GND / VCC | Bridge ground / 5 V supply |

> [!TIP]
> **Selected:** keep the 5 V board; its receiver output reaches the ESP32 only through a 2.2 kΩ / 3.3 kΩ divider. Details: [Becker wiring](wiring.md#becker).

<a id="rs422-check"></a>

### 4.1 Check a new board

"RS422 TO TTL" boards from other sellers or batches differ in header labels, DI pull-up and termination. The values above are for the QYF-998 board only. Check every new or replacement board loose, before wiring it, and record the results in [sources](../research/sources.md).

MAX490 in SO-8, top view ([datasheet Fig. 2](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf#page=8), S144): pin 1 is at the dot or notch, and the pins count anticlockwise from there.

| Chip pin | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Signal | VCC | **RO** | **DI** | GND | Y | Z | B | A |

- [ ] Read the IC marking and find pin 1.
- [ ] Power off, meter on beep: each header pin to chip pins 2 and 3. Identify **3 (DI)** and **2 (RO)**. RO goes through the divider to GPIO16. Keep GPIO17 disconnected until the DI voltage check below.
- [ ] Beep the screw terminals to chip pins 5–8 to confirm Y, Z, B, A. A series resistor (about 100 Ω) in a line is normal.
- [ ] Ω across A–B: about 120 Ω means a termination is fitted; tens of kΩ means it is open. The selected setup has it [open](wiring.md#becker).
- [ ] With the replacement board powered at 5 V and the ESP32 disconnected, measure DI to board GND. DI needs at least 2.0 V for idle high (S34). The upper limit at GPIO17 is its I/O supply + 0.3 V (3.6 V at a nominal 3.3 V, S121). A 5 V pull-up exceeds that limit. If the bias circuit is unknown, DI floats, or the voltage is outside these limits, leave GPIO17 disconnected for interface review. For a compatible board, switch power off before wiring and repeat [bench step 5](wiring.md#bring-up) with the ESP32 held in reset.

---

<a id="ubec"></a>

## 5. 🔋 Power module (UBEC)

<table>
  <tr>
    <td><img src="../images/hobbywing-ubec-3a-product.png" alt="Hobbywing UBEC with voltage jumper, ferrite ring and leads" width="320"></td>
    <td><img src="../images/hobbywing-ubec-3a-dimensions.png" alt="Supplied UBEC image annotated 35 × 12 × 5 mm" width="320"></td>
  </tr>
</table>

> [!WARNING]
> Set the jumper to 5 V before connecting anything. At 6 V the MAX490 board runs above its 5.25 V supply range (S34), and the RX divider puts 3.6 V on GPIO16, the ESP32's absolute limit.

The manual describes reverse-input protection (S14). Allow space for the jumper, leads and ferrite ring; the body alone is about 35 × 12 × 5 mm in the supplied image, 43 × 17 × 7 mm per the catalog.

---

<a id="parts-list"></a>

## 6. 🛒 Parts list

For one bridge, bridge end only — the device-side connectors and harness already exist. The existing bridge stays as a backup.

| # | Ref | Qty | Part | Use |
| :-: | --- | :-: | --- | --- |
| 1 | MCU | 1 | AZ-Delivery ESP32 WROOM dev module | [Pinout](#esp32-pinout) |
| 2 | RS-232-mod1, mod2 | 2 | Hailege H-1-0290 | [Zander](wiring.md#zander), [FLARM](wiring.md#flarm) |
| 3 | RS-422 mod | 1 | MAX490 RS-422 board, 5 V | [Becker](wiring.md#becker) |
| 4 | PS1 | 1 | Hobbywing UBEC 3A (2–6S), 86010010, set to 5 V | [Power](power.md#supply) |
| 5 | R-B1, R-B2 | 1 each | 2.2 kΩ and 3.3 kΩ resistors, 0.125 W, DIN0204 axial (body about Ø1.6 × 3.6 mm) | Divider: R-B1 in the RO wire at J3-12, R-B2 across J3-12 and J3-7 |
| 6 | R-V1, R-V2 | 1 each | 100 kΩ and 10 kΩ resistors, 1 %, 0.125 W, DIN0204 axial | Supply voltage divider: R-V1 at the 12 V tap, R-V2 across J2-5 and J2-14 ([circuit](power.md#voltage-sense)) |
| 7 | C-V | 1 | 100 nF ceramic disc capacitor marked 104, leaded (owner's part); any rating from 6.3 V up is enough, the pin sees at most 3.6 V | ADC filter across J2-5 and J2-14 |
| 8 | F1 | 1 | Supply fuse in an inline holder at the supply end of the 12 V harness; specification tracked in [open items](../project/open-items.md#bench-hardware) | [Power](power.md#supply) |
| 9 | — | existing | Becker harness, reused from its loose bridge-side ends | Check against [Becker wiring](wiring.md#becker); [inventory](../project/existing-installation.md#equipment) |
| 10 | — | 1 pair | Standard XT30 male housing connector and mating female harness connector | [Power](power.md#supply), [mounting](housing.md#connectors) |
| 11 | — | 1 pair | DE-9 male housing connector and female cable connector, solder cups and screw locking, with cable backshell and strain relief | [Combined instrument connector](wiring.md#overview) |
| 12 | — | as needed | Hookup wire 26 AWG, stranded, silver-plated, PTFE-insulated (typical OD 0.9–1.0 mm) in violet, blue and black | Signals, RS-232 3.3 V/ground chain, resistor leads ([colours](housing.md#internal-routing)) |
| 13 | — | as needed | Hookup wire 22 AWG, stranded, silver-plated, PTFE-insulated (typical OD about 1.3 mm) in orange and black | Power branches from the junctions and the instrument-ground wire; the UBEC's own input and output leads are used where they reach |

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
