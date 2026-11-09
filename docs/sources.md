# Sources and evidence

Source/test dates identify evidence scope; source inspection ≠ hardware validation.

## Evidence labels

| Label | Meaning |
| --- | --- |
| Rechecked source | Inspected revision/artifact; inferences separate. Photos establish appearance only. |
| Prior review | Retained finding; not rechecked. |
| Reported | Statement without matching measurement. |
| Assumption / proposed | Unverified hypothesis/design. |
| Measured | Recorded setup/versions/input/result. Host evidence only; no radio/bridge measurements. |

---

## Source register

| ID | Source / baseline | Checked scope and limits |
| --- | --- | --- |
| S00 | [Repository before cleanup, fbed731](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/tree/fbed731cf5fa7e0f3106886182d14e2282b72a6b) | Prior reviews/context; three Markdown files, no code/tests/license. Chat is not hardware evidence. |
| S01 | [OpenSoaring AR62xx.cpp, b2e745c](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp) | Rechecked Send, constructor/state, framing, receive/parser, registration. Review baseline; installed build unknown. |
| S02 | [Port.hpp, same revision](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Port/Port.hpp) | Rechecked binary-span/string_view FullWrite overloads; F02. |
| S03 | [Becker manual catalog](https://becker-avionics.com/manual-instructions-downloads/) | Rechecked 2026-11-08: separate hardware-generation manuals; installed applicability unknown. |
| S04 | [OpenSoar 7.46.26 release notes](https://opensoar.de/releases/v7.46.26/) | Initial retrieval failed; release/WIP rechecked under S09. |
| S05 | [Technical investigation](technical-investigation.md); AlphaLima [basic](https://github.com/AlphaLima/ESP32-Serial-Bridge) / [Pro](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro) | Prior pinned driver/bridge/manual reviews; exact revisions and pages in linked investigation. |
| S06 | [Related projects](related-projects.md) | Prior candidate reviews; file-level licenses/build compatibility require confirmation. |
| S07 | [OpenAI workflow guidance](https://developers.openai.com/cookbook/examples/codex/iterating-development-workflows-with-codex) | Read 2026-11-08; repository workflow conventions. |
| S11 | [Becker driver follow-up](technical-investigation.md#becker-driver-follow-up--2026-11-08), checked 2026-11-08 | Pinned OpenSoar, Werner86, LK8000, eku60 and RadioDriver comparisons; functions/revisions in follow-up. PR statements reported. No independent protocol/capture/runtime validation. |
| S13 | Four owner-supplied photos attributed to Tom, received/inspected 2026-11-08: P1 `1000062273.jpg`, P2 `1000062275.jpg`, P3 `1000062274.jpg`, P4 `1000062276.jpg` | Visible labels/assembly only; revisions and capture date unknown. [Findings](project-baseline.md#toms-photo-evidence--2026-11-08). Private originals; no electrical/settings/runtime evidence. |
| S14 | [Hobbywing UBEC 3A (2-6S), part 86010010](https://www.hobbywing.com/en/products/ubec-3a-2-6s99), Overview / Specifications, checked 2026-11-08; live page has no stated revision | Manufacturer: switching regulator, jumper-selected 5/6 V, 3 A. Family lead only; pictured unit/output unconfirmed. |
| S15 | Current build constraint; owner-reported | New, separate hardware; existing assembly/firmware/harness preserved as backup. [ADR-03](architecture.md#adr-03--new-hardware-with-the-existing-setup-as-backup). No hardware validation. |
| S16 | Earlier hardware selection; private photo `1000062272.jpg`, inspected 2026-11-08 | AZ-Delivery WROOM-32; HW-044; 5 V BEC; board-derived 3.3 V. Reported source: nominal 12 V battery; BEC input 2S–4S. USB/BEC manually exclusive. Visible: HW-044, VCC/RXD/TXD/GND, apparent MAX3232. Exact variants/netlist/voltages unverified. HW-044 selection superseded by S26; other selections retained. [ADR-04](architecture.md#adr-04--selected-bench-hardware-and-power-path). |
| S17 | [AZ-Delivery Dev Kit C V4](https://www.az-delivery.de/en/products/esp-32-dev-kit-c-v4), Description, live page checked 2026-11-08; [ADI MAX3232](https://www.analog.com/en/products/max3232.html), linked datasheet Rev. 10, pp. 1–2; [TI MAX3232 Rev. O](https://www.ti.com/lit/ds/symlink/max3232.pdf), Pin Configuration and Functions | Board-family reference; MAX3232 16-pin supply/signal functions. ADI PDF download failed; TI pin table used. Exact board/HW-044 netlist and IC authenticity unverified. |
| S18 | [TI THVD1451 datasheet SLLSEY3E, Rev. E, May 2019](https://www.ti.com/lit/ds/symlink/thvd1451.pdf), device comparison p. 4, pin diagram/table p. 6 and layout guidance p. 29; [Adafruit 1212](https://www.adafruit.com/product/1212), checked 2026-11-08 | Full duplex, always enabled; SOIC-8 pins and decoupling. Page 6 rendered/visually checked. Prior bare-chip proposal with breakout and local components, superseded by S28. No electrical/radio tests. |
| S19 | [Espressif ESP32 schematic checklist](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32/schematic-checklist.html), Power Supply; [TI eFuse overview](https://www.ti.com/product-category/power-management/power-protection-switches-controllers/efuses/overview.html), checked 2026-11-08, live documents | ESP32 supply guidance/protection functions; no spare-header-current or BEC-protection evidence. Parts/ratings pending. |
| S21 | Tom's equipment/configuration report supplied 2026-11-09; [assessment](reviews/2026-11-09-installation-claims.md) | Reported: AR6201-(022), Classic FLARM IGC series 6, ZS1, TCP endpoints/drivers, 12 V and UBEC 3A 2–6S; separate instrument supplies; no FLARM-to-Zander connection. Existing bridge excludes Becker. No installed settings/measurements verified; private correspondence not published. |
| S22 | Supplied [ZS1 handbook](reference/ZS1_Deutsch.pdf), D3029, 2004-09-16, pp.2/46/48/50; [NMEA supplement](reference/ZS1_NMEA_3037.pdf), program 3037, 2010-10-14, pp.1–2 | Rechecked 2026-11-09: cable reference, fields, changed PZAN3 and conditional baud rates. Original PDFs retained with hashes in assessment. Installed version and latest manufacturer revision unknown; manufacturer site retrieval timed out. Supersedes S09's mirror-only evidence for the handbook. |
| S23 | [FLARM download catalog](https://www.flarm.com/de/support/downloads/); [F6/F7/F8/F9 installation manual](https://www.flarm.com/wp-content/uploads/2024/04/FLARM_InstallationManual_D.pdf), v221, 2016-12-18, pp.1/6/7/9; [FTD-012 ICD](https://www.flarm.com/wp-content/uploads/2025/09/FTD-012-Data-Port-Interface-Control-Document-ICD-7.22.pdf), history 7.22, 2025-09-24, pp.2/13/22–27/55–56; supplied FTD-116 v1.02, 2026-09-07, pp.1/4/7 and [listing](https://www.flarm.com/en/integration/flarm-famp-public-protocol/) | Rechecked 2026-11-09. Classic connector/serial scope; traffic fields and limits; FAMP is over-air. ICD history resolves revision despite stale headers. Documents do not establish installed firmware, port configuration or cable orientation. |
| S24 | OpenSoar 7.46.26 source `b2e745cfddfe0150f6ed7db467ac18c321b38d01`: [Zander.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/Zander.cpp), [Parser.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Parser.cpp), [Descriptor.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Descriptor.cpp), [FLARM/Register.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/FLARM/Register.cpp), [FLARM/StaticParser.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/FLARM/StaticParser.cpp), [TrafficDatabases.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/FLARM/TrafficDatabases.cpp) | Rechecked 2026-11-09: PZAN1–5 / ZanderDevice::ParseNMEA, NMEAParser::ParseLine/RMC, DeviceDescriptor::ParseNMEA, flarm_driver, ParsePFLAA, TrafficDatabases::FindNameById. [Field assessment](reviews/2026-11-09-installation-claims.md#zander-output-versus-opensoar-consumption). Source inspection only; no code copied or runtime tests. |
| S25 | Two diagrams in supplied correspondence, inspected 2026-11-09; private originals `foqSakC0W5neyf3V.png`, `Bl0kTZs9nJOPvdtL.png` | Zander crop: B2 GND, B3 RX-PC, B4 TX-PC; circled A has RX-SR/TX-SR and other functions. Revision/page/view not established. FLARM sketch: 5 TX, 6 RX, 7/8 GND, consistent with S23 numbering only; not proof of complete installed wiring or 1:1 cable compatibility. |
| S26 | Owner-selected Hailege H-1-0290; supplied product text and [image](images/hailege-h-1-0290-supplied.png), 2026-11-09; listing revision/URL not supplied | Reported: MAX3232CSE, 3–5 V, rates above 120 kbps not recommended, 9.4 × 15.9 mm; shared power on both sides, one supply connection; arrows indicate signal flow, RS232/TTL labels identify levels. Rechecked source: image shows edge pads and 17 × 10 mm annotation; use text dimensions as instructed. Replaces S16 HW-044 for both navigation modules. IC identity, pad routing, physical dimensions and electrical operation unmeasured. S27 adds an inferred map; original image retained unchanged at owner request. |
| S27 | [Sunrom 4691](https://www.sunrom.com/p/rs232-ttl-module-max3232), [schematic](https://www.sunrom.com/media/content/670/max3232-schematic.gif) Rev. 1, 2016-02-29; [PCB layout](https://www.sunrom.com/media/content/670/max3232-max232-board.jpg), [chip-side photo](https://www.sunrom.com/media/content/670/max3232-level.jpg), [underside](https://www.sunrom.com/media/content/670/max3232-level-conversion.jpg); rechecked 2026-11-09, photos unversioned | Rechecked source: chip-side pad marks, underside RS232/TTL ends, shared power, channel-2 schematic (8 R2IN, 7 T2OUT, 10 T2IN, 9 R2OUT). Inference: S26 appearance matches this layout; [L/R map](connections.md#hailege-board-pad-reference) is conditional, not Hailege-certified or measured. Exact Hailege schematic unavailable; Amazon listing retrieval failed. Comparator DB9 wiring is not equipment pinning evidence. |
| S28 | Owner-selected assembled RS422-to-TTL board, supplied photo reviewed 2026-11-09 | Rechecked image: `RS422 TO TTL`, header `TXD RXD GND VCC`, screw terminals `Y Z B A`. Exact model and chip marking unconfirmed. Use S29 for supply and signal directions. Logic levels, polarity, termination and isolation remain open. [ADI MAX490 product overview](https://www.analog.com/en/products/max490.html) is a 5 V comparison only. This photo does not establish MAX490 identity or ESP32 compatibility. |
| S29 | Supplied RS422-to-TTL schematic, reviewed 2026-11-09. Owner confirmed it is for the S28 board. Exact model/revision unverified | Rechecked drawing: header contacts 1 TXD→DI, 2 RXD←RO, 3 GND, 4 +5 V. A/B feed the receiver and Y/Z leave the driver. Supply capacitors and +5 V activity LED paths are shown. 120 Ω resistor symbols have open ends. IC label appears to read MAX400ESA and pin numbers repeat. Header and terminal labels are consistent with S28. S30 reports MAX490ESA and onboard termination. Fitted chip, logic levels and actual termination remain unverified. [ADI MAX490 family datasheet 19-0122 Rev. 10, pp.1–2](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf) is a comparison only. |
| S30 | Supplied RS-422 module specifications, reported 2026-11-09. Listing URL/revision not supplied | Reported: MAX490ESA, full-duplex A/B/Y/Z, up to 2.5 Mbit/s and up to 1000 m, 15 kV ESD, 100 Ω current-limiting resistor, TVS diodes, 120 Ω onboard termination, power/TX/RX LEDs and 50 × 27 mm. Rate and distance are separate claims. No ESD test standard supplied. S29 shows 10 Ω series labels and open-ended 120 Ω symbols. Actual series resistance and termination wiring remain open. Rechecked [ADI MAX490 datasheet 19-0122 Rev. 10, pp.1–2](https://www.analog.com/media/en/technical-documentation/data-sheets/MAX1487-MAX491.pdf): 5 V, full duplex, 2.5 Mbit/s. The receiver high-voltage specification gives a minimum, with no ESP32-safe output ceiling. Chip specifications do not verify the assembled module. |

S05/S06: prior findings; confirm model compatibility and file-level reuse permission.

S22/S23 pinout follow-up, 2026-11-09: supplied ZS1 D3029 pp.46/48 visually rechecked (PC-cable DE9 2 TX, 3 optional RMC input, 5 GND); Classic FLARM v221 pp.6/7 rechecked (Power/Data 5 TX, 6 RX, 7/8 GND; reversed IGC numbering, one attached device per port). Installed cable/view matching remains open.

---

## Rechecked facts

S09 — Hardware proposal references, checked 2026-11-08; source observations only, not installed-system evidence:

- [Espressif DevKitC V4 guide](https://documentation.espressif.com/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html), Header Block and Power Supply Options: GPIO availability, WROOM/WROVER differences, J2 pin 19 (5V), J2 pin 14 (GND), and mutually exclusive supply paths. [ESP32 datasheet v5.3](https://documentation.espressif.com/esp32_datasheet_en.html), GPIO/boot configuration and UART sections; [ESP-IDF UART guide v6.1](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html), Introduction and Set Communication Pins: three UARTs and routing. Live documentation; no framework selection implied.
- Zander-authored ZS1 manual, D3029, 16.09.2004, pages 46 and 48, [third-party text mirror](https://manualzz.com/doc/4639871/zander-sdi-zs1-segelflugrechner--sdi-zs1r-analoganzeige-b...): prior PC-cable/NMEA reference. S22 now rechecks the supplied original handbook; S25 adds an unmatched diagram crop. Full installation manual and installed cable match remain missing.
- [FLARM FTD-012 download](https://www.flarm.com/wp-content/uploads/2025/09/FTD-012-Data-Port-Interface-Control-Document-ICD-7.22.pdf), section 5, PDF page 13: RS-232, configurable rates/protocols, limited Classic extension port. Earlier revision uncertainty superseded by S23: p.2 history confirms 7.22/2025-09-24 despite stale headers. S23 supplies a Classic connector reference; installed match remains pending.
- [TI MAX3232 Rev. O](https://www.ti.com/lit/ds/symlink/max3232.pdf), title/supply range; [TI THVD1451 product details](https://www.ti.com/product/THVD1451/part-details/THVD1451DRBR), full-duplex RS-422/485 family at 3.3–5 V: component candidates only, no module/circuit acceptance.
- [OpenSoar release index](https://opensoar.de/releases/) and [7.46.26 notes](https://opensoar.de/releases/v7.46.26/): latest listed release 2026-10-30, Android artifacts, b2e745cfdd prefix and Becker WIP. This retrieval succeeds and supersedes S04's earlier retrieval limitation; no deployed-build claim.

S10 — [Becker DV14307.03 Issue 06](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=57), rechecked 2026-11-08. Page 57 identifies the rear grounding stud. Section 2.5.3.1, page 58, lists P1 pins 11 and 12 as positive supply and 13 and 25 as supply ground. The installed AR6201 variant and its wiring remain unverified.

S10 follow-up, rechecked 2026-11-09: same Issue 06 pp.16/36 lists AR6201-(022), article 0636.339-910 and reported ETSO designations; pp.1/46 establish software/type-plate applicability; J1 table p.59 rechecked. S21 narrows the candidate manual family; installed hardware/software match remains open.

S14 follow-up, rechecked 2026-11-09: same manufacturer Specifications lists 5.5–26 V input and jumper-selected 5/6 V output at 3 A. S21 identifies the existing unit as this family; output unmeasured. Keep separate from S16's new-build BEC selection.

S19 follow-up, rechecked 2026-11-09: [Espressif hardware FAQ](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/hardware-design.html#what-is-the-voltage-tolerance-of-gpios-of-esp-chips), GPIO voltage-tolerance section: 3.6 V maximum. Inference with S23: connector-only adaptation cannot establish a compatible RS-232-to-ESP32 input.

[Reported installation and open checks](project-baseline.md#existing-installation--wp1-record).

S08 — [Yuri/basic AlphaLima source evaluation](reviews/2026-11-08-yuri-bridge.md), rechecked 2026-11-08: pinned sketches, Yuri configuration/license/tree and default-branch histories. Yuri 130f731 has later functional fixes; AlphaLima bb42dc3 has newer documentation only since their shared 2020 revision. See the review for exact references, proposed increments and verification limits; no runtime evidence.

| ID | Observation | Evidence and limit |
| --- | --- | --- |
| F01 | S00 contains README and two docs, with no application code/tests. | Repository tree inspection; no claim about private or external firmware. |
| F02 | Send ignores msg_size and passes a char pointer to FullWrite; Port offers a string_view overload. | S01 lines 247–265; S02 lines 87–93. Measured host consequence in S12: the actual Send body passes 4/10 or 6/10 bytes for synthetic zero-containing frames. Full deployed-build and physical acceptance remain open. |
| F03 | Send assigns ACK locally after writing; success follows that value. No receive-side assignment/notification implements actual acknowledgment in this driver. | S01 Send and receive methods, especially lines 264–278. Establishes a source defect, not the radio's real acknowledgment format. |
| F04 | The command reads both stored frequencies; the constructor does not initialize them. Reception can update private state. | S01 Radio/radio_para, constructor, SetAR620xStation and AR620xConvertAnswer. Unknown/stale active state is a design risk; physical consequences remain untested. |
| F05 | Parser resets on A5, keeps static state, allows zero received CRC and suppresses conversion while sending. Frequency decoding lacks command-specific length checks. | S01 lines 459–558. Implementation observations; whether any exception is protocol-valid requires independent evidence. |
| F06 | Manufacturer supplies distinct manuals for different hardware generations. | S03. Supports identifying the installed device before choosing connector/settings guidance. |

New evidence: revision/page, date, scope and label. Related drivers are not independent protocol validation.

S12 — [Historical Becker driver comparison and send reproduction](reviews/2026-11-08-becker-send.md), rechecked/measured 2026-11-08. PR #189-associated cd818c90 uses counted writes. The current char-pointer call appears in eku60 commit 7a575346 (2024-03-05) and OpenSoar 7.46.26 b2e745cf. A compiled host extraction reproduces truncation and verifies the single-call byte-span correction. See the review for synthetic inputs, method, exact revisions and limits.

---

## Connector reference drawings

S20 — Rechecked source, 2026-11-09. Cropped PDF renders; labels unchanged. Reference examples only; installed model/revision and wiring remain unverified.

| Asset | Manufacturer source | Scope |
| --- | --- | --- |
| `images/becker-reference-connectors.png` | [Becker DV14307.03 Issue 06, February 2021, p.57 Fig.18](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=57) | AR62X1/RT62X1 rear connector layout; callouts identify components, not contacts. J1 functions rechecked on p.59 |
| `images/powerflarm-core-reference-connectors.png` | [FLARM FTD-033 v1.1, 2022-06-06, p.11 §5.3.1](https://www.flarm.com/wp-content/uploads/2024/04/FTD-033-PowerFLARM-Core-Installation-Manual.pdf#page=11) | PowerFLARM Core connector locations |
| `images/powerflarm-core-reference-de9.png` | Same manual, p.14 §5.3.7 | Device socket left / cable plug right; mating-face numbering |

Drawing copyrights: Becker Avionics GmbH / FLARM Technology AG; no repository-license claim. S25 subsequently adds a private, unmatched Zander diagram crop; complete installation drawing still missing. PowerFLARM Core drawings are not applicable pinning evidence for the Classic FLARM reported in S21.
