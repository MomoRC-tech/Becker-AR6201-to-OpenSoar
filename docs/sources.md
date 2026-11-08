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
| S16 | Selected hardware; private photo `1000062272.jpg`, inspected 2026-11-08 | AZ-Delivery WROOM-32; HW-044; 5 V BEC; board-derived 3.3 V. Reported source: nominal 12 V battery; BEC input 2S–4S. USB/BEC manually exclusive. Visible: HW-044, VCC/RXD/TXD/GND, apparent MAX3232. Exact variants/netlist/voltages unverified. [ADR-04](architecture.md#adr-04--selected-bench-hardware-and-power-path). |
| S17 | [AZ-Delivery Dev Kit C V4](https://www.az-delivery.de/en/products/esp-32-dev-kit-c-v4), Description, live page checked 2026-11-08; [ADI MAX3232](https://www.analog.com/en/products/max3232.html), linked datasheet Rev. 10, pp. 1–2; [TI MAX3232 Rev. O](https://www.ti.com/lit/ds/symlink/max3232.pdf), Pin Configuration and Functions | Board-family reference; MAX3232 16-pin supply/signal functions. ADI PDF download failed; TI pin table used. Exact board/HW-044 netlist and IC authenticity unverified. |
| S18 | [TI THVD1451 datasheet SLLSEY3E, Rev. E, May 2019](https://www.ti.com/lit/ds/symlink/thvd1451.pdf), device comparison p. 4, pin diagram/table p. 6 and layout guidance p. 29; [Adafruit 1212](https://www.adafruit.com/product/1212), checked 2026-11-08 | Full duplex, always enabled; SOIC-8 pins and decoupling. Page 6 rendered/visually checked. Breakout compatible; pull-ups/jumpers proposed. No electrical/radio tests. |
| S19 | [Espressif ESP32 schematic checklist](https://docs.espressif.com/projects/esp-hardware-design-guidelines/en/latest/esp32/schematic-checklist.html), Power Supply; [TI eFuse overview](https://www.ti.com/product-category/power-management/power-protection-switches-controllers/efuses/overview.html), checked 2026-11-08, live documents | ESP32 supply guidance/protection functions; no spare-header-current or BEC-protection evidence. Parts/ratings pending. |

S05/S06: prior findings; confirm model compatibility and file-level reuse permission.

## Rechecked facts

S09 — Hardware proposal references, checked 2026-11-08; source observations only, not installed-system evidence:

- [Espressif DevKitC V4 guide](https://documentation.espressif.com/esp-dev-kits/en/latest/esp32/esp32-devkitc/user_guide.html), Header Block and Power Supply Options: GPIO availability, WROOM/WROVER differences, J2 pin 19 (5V), J2 pin 14 (GND), and mutually exclusive supply paths. [ESP32 datasheet v5.3](https://documentation.espressif.com/esp32_datasheet_en.html), GPIO/boot configuration and UART sections; [ESP-IDF UART guide v6.1](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/uart.html), Introduction and Set Communication Pins: three UARTs and routing. Live documentation; no framework selection implied.
- Zander-authored ZS1 manual, D3029, 16.09.2004, pages 46 and 48, [third-party text mirror](https://manualzz.com/doc/4639871/zander-sdi-zs1-segelflugrechner--sdi-zs1r-analoganzeige-b...): PC-cable connector and NMEA behavior. Mirror text inspected; original installation manual ZS1_Inst_d.pdf and connector-view drawing not obtained. Treat as a conditional lead pending matching equipment/cable confirmation.
- [FLARM FTD-012 download](https://www.flarm.com/wp-content/uploads/2025/09/FTD-012-Data-Port-Interface-Control-Document-ICD-7.22.pdf), section 5, PDF page 13: RS-232, configurable rates/protocols, limited Classic extension port. Catalog dates download 2025-09-24; retrieved text headers say 2022-06-06/version 1.1, so filename 7.22 is not treated as confirmed internal revision. Model-specific connector pinning remains unknown.
- [TI MAX3232 Rev. O](https://www.ti.com/lit/ds/symlink/max3232.pdf), title/supply range; [TI THVD1451 product details](https://www.ti.com/product/THVD1451/part-details/THVD1451DRBR), full-duplex RS-422/485 family at 3.3–5 V: component candidates only, no module/circuit acceptance.
- [OpenSoar release index](https://opensoar.de/releases/) and [7.46.26 notes](https://opensoar.de/releases/v7.46.26/): latest listed release 2026-10-30, Android artifacts, b2e745cfdd prefix and Becker WIP. This retrieval succeeds and supersedes S04's earlier retrieval limitation; no deployed-build claim.

S10 — [Becker DV14307.03 Issue 06](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=57), rechecked 2026-11-08. Page 57 identifies the rear grounding stud. Section 2.5.3.1, page 58, lists P1 pins 11 and 12 as positive supply and 13 and 25 as supply ground. The installed AR6201 variant and its wiring remain unverified.

Current constraints: one ESP32; equipment information pending from Tom; navigation returns unknown. See baseline/architecture.

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
