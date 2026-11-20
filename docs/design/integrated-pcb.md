<a id="top"></a>

# 🧩 Integrated bridge PCB

🏠 [Overview](../../README.md) › 🧩 Design › **Integrated bridge PCB**

Use this plan to take the module-based bench bridge through a smaller four-layer design, first articles and a repeatable JLCPCB order.

> [!NOTE]
> **Status:** Development schematic and unrouted four-layer placement for ten production units. See the [approved requirements](requirements.md#requirements), [KiCad project](../../electronics/integrated/compact-bridge.kicad_pro) and [evidence register S170–S177](../research/sources.md).

📑 **On this page:** <kbd>[1 · 🔌 Circuits](#circuits)</kbd> <kbd>[2 · 📐 Housing and layout](#layout)</kbd> <kbd>[3 · 🌡️ Heat and verification](#heat)</kbd> <kbd>[4 · 💶 Production cost](#cost)</kbd> <kbd>[5 · 📦 Design to order](#order)</kbd>

---

<a id="circuits"></a>

## 1. 🔌 Circuits

| Function | Captured circuit | Verification before release |
| --- | --- | --- |
| Controller | ESP32-WROOM-32E-H4 with antenna and shield, no PSRAM. Local decoupling and six top-side 3.3 V logic programming pads. Fixture senses 3.3 V, board powers itself. | Check boot and OTA recovery, unpowered UART injection and instrument-port startup bytes. H4 stock and assembly profile per S170/S175. |
| Navigation | One TRS3232EIPWR receives Zander and FLARM independently on GPIO32 and GPIO26. | Two simultaneous streams, polarity, loading and powered-off input. |
| Becker | SN65HVD1470DR full duplex at 3.3 V. Populate R4 to enable TX; R3 biases DI high, R5 defines DE if link removed. Receiver enabled, no receive termination fitted. SN65HVD35DR alternate subject to qualification. | Verify separate pairs and idle levels through reset and brownout. Compare loaded cable waveforms and bytes in both directions. |
| Supply | External F1, DMP6023LE reverse-polarity MOSFET, 100 kΩ gate network, 12 V gate clamp, provisional SMBJ20A, LMR33610ADDAR synchronous 400 kHz buck to 3.3 V. Draft power stage uses 6.8 µH, two 10 µF/100 V inputs, four 22 µF/25 V outputs. | Scope actual hot clamp and ringing against every input limit. Verify inrush, MOSFET gate, minimum effective capacitance after DC bias and thermal limits. See S172. |
| Battery measurement | R8/R9 divider from protected input to ground, C18 on the divider. TMUX1574PWR connects GPIO34 to ground while PG is low, then to the divider. Output bleed preserves nominal ×11 scaling with the selected resistor network. | Calibrate against XT30 at 7–20 V. Scope GPIO34 and measure injection into unpowered 3.3 V at hot/cold start, hard collapse, reverse input and maximum clamped input. |
| Harness | Right-angle male Amphenol LD09P13A4GV00LF and AMASS XT30PW-M30.G.Y with housing support. Retain [DE-9 pinout](../hardware/wiring.md#de9-pinout), separate instrument returns and existing shield contract. | Dimension and check mounting loads, contact numbering, drills, actual cable exits, board thickness and shell handling. See S173. |

<a id="layout"></a>

## 2. 📐 Compact placement

One 50 × 55.5 mm four-layer PCB, populated from the top with through-hole connectors. The housing study targets **72 × 60 × 25 mm** versus 92 × 70 × 25 mm, about 33% less exterior volume. The native board contains the proposed walls and an XT30 clearance model. These are placement dimensions, pending mated fit and installed clearance. The original housing stays the reference.

- Keep the ESP32 antenna past the PCB edge, with all-layer clearance to the host board, nearby metal and the mounted harness. Test Wi-Fi and Bluetooth inside the closed housing.
- Start with top signals and power, an uninterrupted inner ground plane, an inner power/ground layer and bottom signals/ground. Keep buck input and switching loops short, provide thermal copper for its exposed pad and keep feedback and ADC away from the switch node.
- Carry the DE-9 instrument fault return directly to XT30 minus. For scale, 25 mm of 2 mm wide, 35 µm copper at 85 °C is roughly 7.6 mΩ, 24 mV and 0.078 W at 3.2 A. Include vias, spokes, pad necks and contacts in the finished check. The bridge-positive fuse cannot break an external return current. [Fault paths](../hardware/power.md#wire-breaks)
- **Placement checkpoint:** KiCad 10.0.8 reports zero schematic ERC errors or warnings, zero board/schematic parity issues, 124 unconnected items and twelve 0.20 mm U1 ground-pad holes below the default 0.30 mm rule. Confirm the drill and paste process with the assembler. Prioritize the buck loops and return path before signal routing.

---

<a id="heat"></a>

## 3. 🌡️ Thermal, climate and installation checks

| Check | Procedure |
| --- | --- |
| Energy and thermal load | At the XT30 log input voltage, current peaks and integrated Wh with three loaded Wi-Fi ports, with Bluetooth off, with an additional Bluetooth link and in fallback. Repeat across 7–20 V and under hot closed-box conditions. Probe the buck, inductor, protection device, module, connectors and loaded housing supports. Use the same traffic and phone for any controller comparison. |
| Outdoor storage | Log site temperature and RH, cold surfaces on warm-up and hot parked-canopy conditions. Cycle the unpowered assembly through representative condensation, dry before powering, and check insulation, corrosion, retention, ADC, rail stability and all three streams. Test dry cold starts and sustained hot operation at the measured limits. |
| Limits | XT30 −20…+120 °C operating, TRS3232E −40…+85 °C ambient, H4 and DE-9 up to +105 °C, HVD1470 up to +125 °C ambient. Buck limit is junction temperature. Loaded PETG features may govern earlier. Check the matching plugs, capacitors and remaining BOM as well. See S170–S174 and the [housing climate study](../hardware/housing.md#climate). |
| Fabrication | Evaluate JLC04161H-7628 1.6 mm FR-4, green solder mask, 1 oz outer copper. Confirm actual laminate, finished thickness, finish and thermal-pad process at quote. Compare lead-free HASL and ENIG. If acrylic coating is needed, mask antenna, connectors, shell bonds and probes. Inspect coverage and re-test RF and leakage. Require a cleaner compatible with the module and an assembly sequence respecting its single reflow. |
| Installed use | Ask the responsible certifying staff to assess the actual aircraft, connection to the Becker and any standby-frequency command. Supply wiring, fuse and isolation, failure effects, installed radio/compass interference checks, mounting and continued-maintenance information. S177 records the regulatory context. |

---

<a id="cost"></a>

## 4. 💶 Ten-board cost comparison

Supplier snapshot on 20 November 2026, USD per part. Six major lines only, before passives, protection, ADC switch, fabrication, assembly, test, freight and tax. S175 records the listings.

| Part | One | Ten, each | Ten, line total |
| --- | ---: | ---: | ---: |
| ESP32-WROOM-32E-H4 | 4.3546 | 3.8993 | 38.99 |
| LMR33610ADDAR | 1.7660 | 1.5783 | 15.78 |
| SN65HVD1470DR | 1.5408 | 1.5016 | 15.02 |
| Amphenol DE-9 | 1.6518 | 1.3727 | 13.73 |
| TRS3232EIPWR | 0.7770 | 0.6268 | 6.27 |
| AMASS XT30 | 0.3836 | 0.3346 | 3.35 |
| **Subtotal, calculated before rounding** | **10.4738** | **9.3133** | **93.13** |

- The approved HVD1470 saves $3.0424 per board versus the HVD35 at $4.5440. A SIT3491EESD at $0.4293 needs a separate edge-rate and ESD qualification. A 3PEAK TPT3232E-TS3R at $0.4636 changes receiver limits and charge-pump sizing. A $0.8520 LMR51420YDDCR needs a new regulator layout and voltage-valid circuit for the ADC switch.
- Reverse protection costs about $0.78 more per board using the MOSFET network than a series Schottky. At 0.10 A input over 10 h, a 0.4 V diode costs 0.40 Wh versus 0.0035 Wh in a 35 mΩ MOSFET channel, plus gate bias. Verify actual flight draw.
- Quote ten on Standard PCBA with one SMT side, panel rails/fiducials, connector soldering and attrition. The published one-side setup and stencil example totals $33.77 per order. Compare charges for ENIG, optional coating, connector hand fitting, shipping and tax separately. Report cost per accepted unit. S168–S169.

---

<a id="order"></a>

## 5. 📦 From design to JLCPCB order

| Gate | Work product | Ready to proceed when |
| --- | --- | --- |
| 1 · Freeze interfaces | One-page interface and environmental limits sheet, mated connector drawings and compact fit study | Owner selects hardware choices individually. Record instrument variants, electrical limits, allowed TX states, ground-fault case and thermal test limits. Track missing inputs in [open items](../project/open-items.md#pcb-study). |
| 2 · Capture the circuit | Separate KiCad PCB project. Sheets for power/protection, ESP32/service, navigation and radio. Exact MPN, supplier code, rating, land pattern and fitted/DNP state on each line | Rev A schematic owns its board nets. Compare the netlist against the existing harness contract and firmware pin map. Review worst-case power, fault and logic calculations, ERC and every remaining warning. Check footprints against manufacturer drawings. |
| 3 · Place and route | Four-layer board and STEP export, mated compact housing study, placement and thermal review | Fit with connectors and service fixture, continuous return paths, fault-current capacity, antenna clearance, thermal pad and test access. Check JLC stackup, copper, annular rings, drills, mask/paste, routed slots, board-edge distances and panel rails/fiducials. DRC and schematic/PCB parity pass. |
| 4 · Obtain ten-board quote | One revision-stamped ZIP with Gerbers for every copper, mask, silk, paste and outline layer, plus Excellon plated/non-plated drills. Separate JLC BOM and CPL, dimensioned fabrication and assembly drawings, fitted/DNP and hand-fit list, editable KiCad source and STEP | Reopen plots/drills in an independent viewer. Match fitted designators exactly between BOM and CPL. BOM carries value, footprint, designator and JLC/LCSC code. CPL carries designator, X/Y in mm, rotation and side using one origin. Inspect polarity, pin 1, module antenna direction, connector face and rotations in JLC's placement preview. S169. |
| 5 · Owner order | Dated quote or reproducible ordering pack with stackup, board thickness/copper/finish, mask, assembly side, service tier, quantities, accepted parts, stock snapshot, lead time, shipping and price breakdown | Every fitted part is matched or explicitly listed for local fitting. Record any substitutions, quote validity shown by vendor and assembly-review corrections. Owner reviews the final amount and places the order. Arrange first-article inspection and acceptance before committing the remaining assembly where available. |
| 6 · Accept and repeat | Serialized first-article results against section 3, defects and corrected revision where needed. Bundle flash/recovery instructions and fixture pinout with the release | Run the closed-box thermal, electrical, mechanical and radio tests on actual hardware. Use the accepted revision and a fresh stock/price check for the production order. |

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
