# Related projects and reuse investigation

Prior investigation dated 2026-11-08, preserved from [baseline fbed731](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/tree/fbed731cf5fa7e0f3106886182d14e2282b72a6b). Public GitHub searches covered AR6201/AR620x, Becker issues in XCSoar/OpenSoar/LK8000, and ESP32 serial bridges. Candidate source/metadata findings below were not independently repeated during the documentation cleanup. No candidate was built or tested with hardware.

These are research leads, not selected dependencies or verified compatibility. [Sources](sources.md) defines the evidence labels; [ADR-02](architecture.md#adr-02--implementation-and-reuse-baseline) owns the evaluation order and [WP3](roadmap.md#work-queue-and-milestones) owns selection.

**Follow-up, 2026-11-08 (S11):** The [Becker driver comparison](technical-investigation.md#becker-driver-follow-up--2026-11-08) rechecks LK8000, eku60 and RadioDriver and examines Werner86's PR 449 implementation in detail. Werner supplies useful counted-write, integer-conversion and state-guard ideas plus a reported explanation for paired frequency writes, but the guard's initialization and stream parser remain defective. Use that follow-up for these candidates' current evidence and file-level license limits; none is a verified replacement.

## Prior review shortlist

### LK8000 — primary protocol reference

The [Becker driver](https://github.com/LK8000/LK8000/blob/8d78c63f599b894dbff217e007db44be06ee67ee/Common/Source/Devices/devAR620x.cpp) implements frequency changes, swap, volume, squelch, radio mode, and status parsing. It uses explicit-length writes and matching receive CRC. However, its parser also resets on every A5 byte, uses static receive state, and suppresses conversion during sending. Use it to compare commands and status behavior, with independent validation before porting.

Revision 8d78c63f599b894dbff217e007db44be06ee67ee, committed 2026-11-06. GitHub identifies GPL-2.0. Historical discussions include [PR 941](https://github.com/LK8000/LK8000/pull/941) and [PR 940](https://github.com/LK8000/LK8000/pull/940). Shared code patterns mean agreement with OpenSoar alone is insufficient protocol evidence.

### Yuri-rage ESP-Serial-Bridge — first small bridge candidate to evaluate

The [AlphaLima-derived fork](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) documents ESP32/ESP8266 support, Arduino framework 2.0 compatibility work, PlatformIO support, and TCP/UDP options.

Inspection of its main sketch confirms the TCP receive loop breaks at buffer capacity before forwarding, addressing the drain-and-drop behavior in the basic AlphaLima revision. UART-to-TCP writes carry explicit lengths. Bluetooth targets a configured serial port. Write results remain unchecked in the inspected paths. Debug/boot output, multiple writers, backpressure, and simultaneous navigation require further checks.

Revision 130f731416d43c89bce5626ef989b3e65918c9ce, committed 2024-08-21. MIT repository metadata. The original review inspected the tree, README and sketch through pinned API/raw endpoints. Build compatibility statements are upstream claims.

### rbp28668/RadioDriver — dedicated Becker controller examples

The [project](https://github.com/rbp28668/RadioDriver/tree/a794c4a994838b94d2b0bfa69ef76cda9a698957) includes an AR620x class, a Teensy-oriented controller with display/encoder/SD/GPS libraries, and a Windows Visual Studio serial program. The Windows entry point opens COM7, parses available input, and issues example frequency changes. It could inform host-side testing and controller structure. It is not an independently validated simulator.

Revision a794c4a994838b94d2b0bfa69ef76cda9a698957, committed 2024-08-02. README documentation is minimal. No repository-level license was identified. Clarify permission before copying code.

## Other useful leads

| Project | Evidence and possible use | Remaining checks |
| --- | --- | --- |
| [DroneBridge ESP32](https://github.com/DroneBridge/ESP32/tree/25b3d9c60ca4cb958ad7156a742436c6a4361475) | README documents bidirectional transparent mode, TCP port 5760, configurable flush timeout, and host regression checks. Apache-2.0 metadata. Revision dated 2026-10-26. | Dedicated-radio bridge alternative. Confirm transparent TCP behavior and independent multi-UART capability. Forwarding code not audited here. |
| [XCVario](https://github.com/iltis42/XCVario/tree/1c5ad83a9e0911b81bb0bd5a47243bea5746f6c2) | ESP32 gliding instrument with serial/Wi-Fi routing. Router.cpp contains message filtering and queue routing. GPL-2.0 metadata. Revision dated 2026-10-18. | Useful coexistence reference. Whole-firmware adoption expands scope, and Becker binary transparency is unverified. |
| [esp-link](https://github.com/jeelabs/esp-link) | Project describes an ESP8266 Wi-Fi/serial bridge. | Background reference with a different target from the installed ESP32. |
| [SD-Link AR620X](https://sdlink.de/wp-content/uploads/2026/02/SD-AR620X-E-NG_en_easyvfr_installation_v1.0.pdf) | Commercial BLE integration. Vendor documentation specifies serial-number and tandem-mode prerequisites for its adapter. | Compare prerequisites against the exact radio and manufacturer documentation before applying them here. |

## Historical XCSoar work

[PR 189](https://github.com/XCSoar/XCSoar/pull/189), [PR 190](https://github.com/XCSoar/XCSoar/pull/190), and [PR 449](https://github.com/XCSoar/XCSoar/pull/449) are closed and unmerged according to API metadata. PR 189 reports author testing with an AR6201 on Windows and UNIX, including tuning and reconnects. This is historical testimony for that code, not verification of current OpenSoar.

[Issue 1092](https://github.com/XCSoar/XCSoar/issues/1092) links an eku60 Becker branch and includes a successful-test report. Maintainer discussion explains that the public protocol-documentation policy prevented acceptance. This records the historical obstacle, not a determination about current protocol availability.

No merged repair or independently validated end-to-end replacement was established in this search. The search is not exhaustive.

## Recommendation recorded by the prior review

1. Identify the installed ESP32 firmware before selecting a replacement.
2. Compare OpenSoar with LK8000 and the historical Becker branches for explicit-length transmission, initialization, and status handling.
3. Evaluate the Yuri-rage fork before adapting basic AlphaLima. Keep DroneBridge as a dedicated-link alternative.
4. Establish real radio response rules and independent vectors before implementing ACK logic or trusting a simulator.
5. Exercise embedded zero/A5, malformed frames, startup state, oversized bursts, partial writes, slow clients, reconnects, and navigation coexistence.

This superseded the earlier preference for starting with basic AlphaLima. It is a candidate-evaluation recommendation; installed firmware replacement remains undecided. Use ADR-02 for the current decision status.

## Original verification and limitations

Inspected pinned source for LK8000, the Yuri-rage forwarding paths, RadioDriver entry points/tree, and XCVario routing. Read DroneBridge documentation and checked repository metadata and XCSoar issue/PR status. License labels need file-level review before reuse. No application build, hardware test, or independent reproduction of historical claims was performed.
