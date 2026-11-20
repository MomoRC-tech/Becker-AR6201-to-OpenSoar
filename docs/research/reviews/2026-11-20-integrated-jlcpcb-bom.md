<a id="top"></a>
# 🔬 Integrated PCB — JLCPCB BOM audit

🏠 [Overview](../../../README.md) › 🔬 [Research](../sources.md) › **JLCPCB BOM**

Production BOM category, assembly eligibility, stock snapshots, and unassigned passive candidates for the integrated PCB.

> [!NOTE]
> **Status:** Frozen catalog review of the native KiCad export created 2026-11-20 23:42 (Eeschema 10.0.8); JLCPCB pages checked 2026-11-21. Candidate SKUs are research only. Stock counts are snapshots, not order-time guarantees.

📑 **On this page:** <kbd>[1 · 🔎 Scope](#scope)</kbd> <kbd>[2 · 🏭 JLC tiers and service](#tiers)</kbd> <kbd>[3 · 📦 Exact catalog matches](#matches)</kbd> <kbd>[4 · 🧾 Passive candidates](#passives)</kbd> <kbd>[5 · 🖐️ Fit and open sourcing](#fit)</kbd> <kbd>[6 · 🔗 Sources](#sources)</kbd>

---
<a id="scope"></a>
## 1. 🔎 Scope and count

- Source: recursive BOM from [`compact-bridge.kicad_sch`](../../../electronics/integrated/compact-bridge.kicad_sch) exported with KiCad 10.0.8.
- The export has **84 schematic components**. **66 references across 37 grouped BOM lines** are active production items. J3 is excluded/DNP; TP1–TP17 are test pads and excluded from the BOM/placement list.
- 16 grouped lines already carry a JLC/LCSC code, covering **21 reference instances**. **45 active references have no LCSC code**; 14 grouped lines have no MPN (all 0603 resistor and capacitor groups).
- Exact JLC catalog pages matched all 16 coded lines. Exact-MPN lookups found three more rows (D4, D5 and J4) whose LCSC fields are blank. Those 19 exact catalog SKUs cover 24 references.
- Passive SKUs below are purchasing candidates against the values and footprints currently stated in KiCad. Added candidate ratings are catalog specs, not requirements inferred for the circuit. None were written into the BOM.

---
<a id="tiers"></a>
## 2. 🏭 JLC tiers and PCBA service

| JLC category or service | Published treatment | Audit result |
| --- | --- | --- |
| Basic | Preloaded parts; no Economic feeder fee. | No exact coded production BOM item was confirmed Basic. The common 0603 resistor candidates below are Basic. |
| Preferred Extended | Manually loaded Extended parts; exempt from the Economic feeder fee. | No exact coded production BOM item or candidate below was confirmed Preferred. |
| Extended | Economic PCBA lists a $3.07 fee per unique Extended part. | All 16 coded production items and the three exact-MPN-only matches are Extended. |
| Standard PCBA | Separate assembly service. JLCPCB's price guide lists $1.53 feeder loading for Basic/Extended parts; the Standard-only flag is a service restriction, not a part tier. | U1's exact row is **Standard Only**, so JLCPCB would need Standard PCBA to place U1 from its library. |
| Through-hole / wave solder | JLCPCB states THT wave soldering is available with both Economic and Standard PCBA; a specific part must also be listed for wave soldering. | J2 is listed as Wave Soldering and both PCBA types. J1 has no exact library row. |

JLCPCB says Preferred Extended parts have no feeder fee for **Economic** PCBA. Its price guide separately lists feeder loading for Basic/Extended parts under **Standard** PCBA. This keeps the part tier distinct from the selected PCBA service. [JLCPCB assembly FAQ](https://jlcpcb.com/help/article/pcb-assembly-faqs) · [JLCPCB assembly price guide](https://jlcpcb.com/help/article/pcb-assembly-price) · [JLCPCB capabilities](https://jlcpcb.com/capabilities/pcb-assembly-capabilities)

---
<a id="matches"></a>
## 3. 📦 Exact JLC catalog matches

The inventory column reports only quantities exposed by JLC's public page/search snapshot. A blank means no numeric JLC stock was surfaced in this check. Recheck every line in the live BOM quote before ordering.

| Ref. | BOM MPN / JLC code | JLC category and assembly | Stock snapshot / note |
| --- | --- | --- | --- |
| C11 | TDK C1608X7R1C105K080AC · [C2167399](https://jlcpcb.com/partdetail/TDK-C1608X7R1C105K080AC/C2167399) | Extended · SMT · Economic/Standard | Not surfaced |
| C12, C19 | TDK C3225X7R2A106KT000E · [C49296968](https://jlcpcb.com/partdetail/TDK-C3225X7R2A106KT000E/C49296968) | Extended · SMT · Economic/Standard | Not surfaced |
| C13 | TDK C1608X7R1H224KT000N · [C445721](https://jlcpcb.com/partdetail/C445721) | Extended · SMT · Economic/Standard | Not surfaced |
| C14–C17 | TDK C3225X7R1E226M250AB · [C2175584](https://jlcpcb.com/partdetail/C2175584) | Extended · SMT · Economic/Standard | Not surfaced |
| D1 | BZT52C12 · [C19077410](https://jlcpcb.com/partdetail/C19077410) | Extended · SMT · Economic/Standard | Not surfaced |
| D2 | SMBJ20A · [C151257](https://jlcpcb.com/partdetail/C151257) | Extended · SMT · Economic/Standard | 16,935 shown in a JLC snapshot crawled about a month earlier |
| D3 | ST USBLC6-2SC6 · [C7519](https://jlcpcb.com/partdetail/C7519) | Extended · SMT · Economic/Standard | Indexed snapshots disagreed (35,356 and 35,604); current count unknown |
| J2 | XT30PW-M30.G.Y · [C431092](https://jlcpcb.com/partdetail/XT30PW-M30.G.Y/C431092) | Extended · Wave Soldering · Economic/Standard | 69,996 shown in a JLC snapshot crawled about a month earlier |
| L1 | Coilcraft XGL6030-682MEC · [C3911557](https://jlcpcb.com/partdetail/C3911557) | Extended · SMT · Economic/Standard | Not surfaced |
| Q1 | DMP6023LE-13 · [C154901](https://jlcpcb.com/partdetail/C154901) | Extended · SMT · Economic/Standard | Not surfaced |
| U1 | ESP32-WROOM-32E-H4 · [C3013935](https://jlcpcb.com/partdetail/EspressifSystems-ESP32_WROOM_32EH4/C3013935) | Extended · SMT · **Standard Only** | 2,340 shown in a JLC snapshot crawled about a month earlier |
| U2 | TI TRS3232EIPWR · [C2652868](https://jlcpcb.com/partdetail/C2652868) | Extended · SMT · Economic/Standard | Not surfaced |
| U3 | TI SN65HVD1470DR · [C2865785](https://jlcpcb.com/partdetail/C2865785) | Extended · SMT · Economic/Standard | Not surfaced |
| U4 | TI LMR33610ADDAR · [C1852147](https://jlcpcb.com/partdetail/C1852147) | Extended · SMT · Economic/Standard | Not surfaced |
| U5 | TI TMUX1574PWR · [C2673443](https://jlcpcb.com/partdetail/C2673443) | Extended · SMT · Economic/Standard | 7,623 shown in a JLC snapshot crawled about a month earlier |
| U6, U7 | MX5050S6030A1 · [C54800033](https://jlcpcb.com/partdetail/C54800033) | Extended · SMT · Economic/Standard | No JLC count surfaced. A separate LCSC lookup reported 2,774; that is not JLC assembly inventory. |
| D4 | Lite-On LTST-S270KGKT · [C125113](https://jlcpcb.com/partdetail/LiteOn-LTSTS270KGKT/C125113) | Extended · SMT · Economic/Standard | 3,270 shown in a JLC snapshot crawled about a month earlier |
| D5 | Lite-On LTST-S270KRKT · [C125114](https://jlcpcb.com/partdetail/LiteOn-LTSTS270KRKT/C125114) | Extended · SMT · Economic/Standard | 24,852 shown in a JLC snapshot crawled about a week earlier |
| J4 | GCT USB4145-03-0230-C · [C5291559](https://jlcpcb.com/partdetail/6035042-USB4145_03_0230C/C5291559) | Extended · SMT · Economic/Standard · “Assembly Difficulty High” | No JLC count surfaced. A separate LCSC lookup reported 0; this is not JLC inventory. |

### Near matches and unassigned catalog rows

| Ref. | Finding | Stock / assembly implication |
| --- | --- | --- |
| C10 | BOM MPN is `C1608X7R1C104K080AA`. JLC lists TDK `C1608X7R1C104K`, code [C2844757](https://jlcpcb.com/partdetail/TDK-C1608X7R1C104K/C2844757), with 100 nF, 16 V, X7R, 0603 specs. Its displayed MPN omits the BOM suffix, so the exact code mapping is not verified. | The JLC page showed 0 in stock. Do not treat it as an exact MPN match or an available part. |
| U8 | No exact row for `CP2102N-A02-GQFN24` was found. JLC lists the suffix variant `CP2102N-A02-GQFN24R`, [C969151](https://jlcpcb.com/partdetail/C969151), Extended, QFN-24-EP 4 × 4 mm, Economic/Standard. | The index showed 27,321 for the suffix variant in a snapshot crawled about a week earlier. Suffix difference is unresolved; it is not assigned to U8. |
| Q2, Q3 | Several manufacturer-specific MMBT3904 SOT-23 rows are Extended. One research candidate is onsemi MMBT3904, [C3200105](https://jlcpcb.com/partdetail/onsemi-MMBT3904/C3200105). The onsemi MMBT3904LT1G datasheet gives pin 1 Base, pin 2 Emitter, pin 3 Collector, matching the required B=1/E=2/C=3 map; the exact catalog ordering suffix still needs to be matched to the row. | No manufacturer or JLC code is assigned. The candidate's current JLC stock count was not surfaced. [onsemi datasheet](https://www.onsemi.com/pdf/datasheet/mmbt3904lt1-d.pdf) |
| J1 | No exact JLC library result was found for Amphenol D15P33E4GV00LF. | No JLC stock or assembly part match is verified. Manual/owner-supplied fitting is a fallback to resolve; do not assume JLC will assemble it from its stock library. |

---
<a id="passives"></a>
## 4. 🧾 Unassigned 0603 passive candidates

These SKUs match the stated resistance/capacitance and package only. Ratings shown are the candidate's catalog ratings; the source BOM does not specify resistor tolerance/power or most capacitor voltage/dielectric requirements. Candidate status does not authorize replacing or assigning the schematic MPN/LCSC fields.

### 0603 resistors

| BOM refs. / value | JLC candidate | Category | Catalog extras | Stock snapshot |
| --- | --- | --- | --- | --- |
| R17, R18, R21–R23 · 1 kΩ | UNI-ROYAL `0603WAF1001T5E` · [C21190](https://jlcpcb.com/partdetail/C21190) | Basic | 100 mW, 75 V, ±1%, ±100 ppm/°C | 26,683,889 shown in a snapshot crawled about two months earlier |
| R1, R2, R5, R11, R19, R20 · 10 kΩ | UNI-ROYAL `0603WAF1002T5E` · [C25804](https://jlcpcb.com/partdetail/C25804) | Basic | 100 mW, 75 V, ±1%, ±100 ppm/°C | Not surfaced |
| R13, R14 · 5.1 kΩ | UNI-ROYAL `0603WAF5101T5E` · [C23186](https://jlcpcb.com/partdetail/23913-0603WAF5101T5E/C23186) | Basic | 100 mW, 75 V, ±1%, ±100 ppm/°C | 23,801,693 shown in a snapshot crawled about two months earlier |
| R3 · 4.7 kΩ | UNI-ROYAL `0603WAF4701T5E` · [C23162](https://jlcpcb.com/partdetail/23889-0603WAF4701T5E/C23162) | Basic | 100 mW, 75 V, ±1%, ±100 ppm/°C | Not surfaced |
| R4 · 0 Ω | UNI-ROYAL `0603WAF0000T5E` · [C21189](https://jlcpcb.com/partdetail/C21189) | Basic | 0 Ω, 0603 | Not surfaced |
| R6, R8, R10, R12 · 100 kΩ | UNI-ROYAL `0603WAF1003T5E` · [C25803](https://jlcpcb.com/partdetail/C25803) | Basic | 100 mW, 75 V, ±1%, ±100 ppm/°C | Not surfaced |
| R9 · 11.1 kΩ | Vishay `TNPW060311K1BEEA` · [C1854558](https://jlcpcb.com/partdetail/VishayIntertech-TNPW060311K1BEEA/C1854558) | Extended | 125 mW, 75 V, ±0.1%, ±25 ppm/°C | 6,812 shown in a snapshot crawled about a month earlier |
| R15 · 22.1 kΩ | UNI-ROYAL `0603WAF2212T5E` · [C25961](https://jlcpcb.com/partdetail/26704-0603WAF2212T5E/C25961) | Extended | 100 mW, 75 V, ±1%, ±100 ppm/°C | Not surfaced |
| R16 · 47.5 kΩ | UNI-ROYAL `0603WAF4752T5E` · [C23061](https://jlcpcb.com/partdetail/23788-0603WAF4752T5E/C23061) | Extended | 100 mW, 75 V, ±1%, ±100 ppm/°C | Not surfaced |
| R7 · 43.3 kΩ | No exact 0603 JLC row found in the catalog search. | Unknown | Do not round to 43 kΩ or 43.2 kΩ. | Unknown |

The six Basic SKUs above cover 19 resistor references. Three Extended candidates cover R9, R15 and R16. R7 remains unmatched.

### Capacitors

| BOM refs. / value | JLC candidate | Category | Catalog specs / stock |
| --- | --- | --- | --- |
| C1 · 1 µF 0603 | TDK `C1608X7R1C105K080AC` · [C2167399](https://jlcpcb.com/partdetail/TDK-C1608X7R1C105K080AC/C2167399) | Extended | 16 V, X7R, ±10%; same catalog MPN already present at C11. Stock not surfaced. |
| C2 · 10 µF 0805 | TDK `C2012X5R1C106K125AC` · [C20507531](https://jlcpcb.com/partdetail/TDK-C2012X5R1C106K125AC/C20507531) | Extended | 16 V, X5R, ±10%. Stock not surfaced. |
| C3–C9, C18, C20, C22, C23 · 100 nF 0603 | TDK `C1608X7R1H104K080AE` · [C2167477](https://jlcpcb.com/partdetail/TDK-C1608X7R1H104K080AE/C2167477) | Extended | 50 V, X7R, ±10%. Stock not surfaced. |
| C21 · 4.7 µF 0603 | Murata `GRM188R61E475KE15D` · [C408137](https://jlcpcb.com/partdetail/MurataElectronics-GRM188R61E475KE15D/C408137) | Extended | 25 V, X5R, ±10%; 1,093 shown in a snapshot crawled about a month earlier. Meets the stated ≥10 V constraint. |
| C21 · 4.7 µF 0603 | Murata `GRM188R61A475KE15D` · [C71633](https://jlcpcb.com/partdetail/MurataElectronics-GRM188R61A475KE15D/C71633) | Extended | 10 V, X5R, ±10%; stock not surfaced. Also meets the stated constraint. |

---
<a id="fit"></a>
## 5. 🖐️ Fit, DNP and unresolved sourcing

| Ref. | Current finding |
| --- | --- |
| J1 | Exact D15P33E4GV00LF is not in the JLC match set. Assembly/hand-fit route is unverified. |
| J2 | Exact JLC row is listed for Wave Soldering under Economic and Standard; no physical mating/fit acceptance is implied by the catalog row. |
| J3 | Excluded/DNP in the production BOM export. |
| J4 | Exact MPN is listed for SMT under Economic and Standard, but JLC marks assembly difficulty high and no stock count was exposed. |
| Q2/Q3 | MMBT3904 manufacturer and LCSC code are not assigned. Preserve required pin map B=1/E=2/C=3 and verify the final manufacturer-specific datasheet and BOM row. |
| U8 | Exact CP2102N-A02-GQFN24 row is not verified; the `...GQFN24R` listing is a near match only. |
| TP1–TP17 | Test points are excluded from the production BOM/placement list. |
| R7, C10 | R7 has no exact 43.3 kΩ JLC candidate in this lookup; C10's JLC page uses a shorter MPN than the BOM field. |

Order-time checks needed: refresh every inventory count, use the live JLC BOM matcher to confirm the exact manufacturer/MFR code and package, confirm the chosen PCBA service against U1, and settle the J1/Q2/Q3/U8 rows before finalizing a BOM/CPL. No order, substitution, or part-field change was made in this review.

---
<a id="sources"></a>
## 6. 🔗 Sources

- [JLCPCB parts library search](https://jlcpcb.com/parts/componentSearch) and the linked part pages in Sections 3–4; exact component categories/specifications are from these pages.
- [JLCPCB assembly FAQs](https://jlcpcb.com/help/article/pcb-assembly-faqs), [price guide](https://jlcpcb.com/help/article/pcb-assembly-price), and [assembly capabilities](https://jlcpcb.com/capabilities/pcb-assembly-capabilities): Basic/Preferred/Extended fees, Standard-only distinction, and THT support.
- [onsemi MMBT3904LT1G datasheet](https://www.onsemi.com/pdf/datasheet/mmbt3904lt1-d.pdf): manufacturer pin numbering for the stated Q2/Q3 candidate family.

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../../README.md)</kbd>
