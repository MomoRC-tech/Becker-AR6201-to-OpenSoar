# Technical investigation

Reviewed 2026-11-08. This is a source review and an engineering recommendation. No application build, simulator, ESP32 execution, radio bench test, or aircraft test was performed.

## Reproducible source baseline

| Project | Reviewed revision | Latest commit date at review | Repository state |
| --- | --- | --- | --- |
| OpenSoar | [4a3794a](https://github.com/August2111/OpenSoar/tree/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41) | 2026-10-21 | Not archived |
| ESP32 Serial Bridge | [bb42dc3](https://github.com/AlphaLima/ESP32-Serial-Bridge/tree/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf) | 2026-07-28 | Not archived |
| ESP32 Serial Bridge Pro | [bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/tree/bf263c1880d7630cadef5264dac9aa9654e4096e) | 2026-07-30 | Not archived |

Dates and archive flags came from GitHub repository/commit metadata. Recent commits alone do not establish ongoing support or reliability. The installed firmware and Android build have not been identified.

## OpenSoar driver findings

Confirmed in the [pinned AR62xx driver](https://github.com/August2111/OpenSoar/blob/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41/src/Device/Driver/AR62xx.cpp).

| Area | Source observation and consequence |
| --- | --- |
| Transmission | `Send()` ignores `msg_size` and passes a character pointer. The `string_view` overload determines length at the first zero byte, truncating binary commands. |
| Acknowledgment | `Send()` assigns ACK after writing. No receive-side response assignment or condition notification exists in this driver. Success does not establish radio acceptance. |
| Initialization | `radio_para` has uninitialized doubles. Both frequencies are read when constructing a command, before any guaranteed status reception. |
| Framing | Every `0xA5` resets reception, including inside payload or CRC. |
| Receive state | Static parser buffers and CRC deduplication state are shared across instances and survive device recreation. |
| Validation | Zero received CRC bypasses verification. Command-specific minimum lengths are unchecked before frequency fields are read. |
| Readback | Conversion is suppressed during sending. Frequencies update private state, while `NMEAInfo` only receives an alive update. |

The write overload is confirmed in [Port.hpp](https://github.com/August2111/OpenSoar/blob/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41/src/Device/Port/Port.hpp).

### Protocol details to validate independently

The implementation constructs a ten-byte frequency frame with header A5, protocol 14, length 05, command 16 hexadecimal, two two-byte frequency values, and two CRC bytes. CRC calculation starts at zero and uses polynomial 8005 hexadecimal. Byte ordering uses union byte indices and assumes the target representation. These are implementation observations, awaiting comparison with authoritative protocol documentation and independent vectors.

The actual acknowledgment format, zero-CRC allowance, startup status request, channel mapping, firmware support, and application display behavior still need verification. Source defects above have not been reproduced on a running OpenSoar build.

## ESP32 bridge comparison

### Basic bridge

The [sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) implements separate TCP servers and UART arrays. TCP-to-UART and UART-to-TCP use explicit byte counts. There is no NMEA parser in those paths.

However, startup prints go to all three UARTs unconditionally. Disabling debug alone does not remove them. Receive loops continue draining after buffer indices saturate, so bursts exceeding the 1,023-byte forwarded capacity lose bytes. Write results are ignored, multiple clients may write to the same UART, and optional Bluetooth input broadcasts to every UART.

The [configuration](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/config.h) defaults to three UARTs, TCP ports 8880–8882, 19200 baud, 8N1, AP mode, debug, and OTA. These defaults describe the project, not the Becker requirements or installed board.

The repository includes an [MIT license](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/LICENSE). It is the smaller adaptation candidate, subject to the forwarding fixes and hardware checks.

### Pro bridge

The [Pro sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/blob/bf263c1880d7630cadef5264dac9aa9654e4096e/ESP32-Serial-Bridge-pro/ESP32_Serial_Bridge_pro.ino) adds persistent configuration, a web interface, filtering, Bluetooth, and configurable cross-port forwarding.

With both NMEA filter counts zero, UART reception uses counted binary chunks. Enabling filters introduces dollar-sign resets, text matching, and newline insertion. Radio filters must therefore be disabled. Cross-port routes must exclude radio traffic.

TCP reception still drains beyond its 126-byte forwarded capacity. Input is also copied to the debug UART, and write counts are ignored. The serial receive path does stop at its chunk limit.

No standalone license file appeared in the reviewed tree, despite the README referring to one. Reuse permission needs clarification. The web stack and SPIFFS assets also require a reproducible dependency/build audit before choosing this version.

### Existing installed bridge

Its successful navigation service is user-reported. Firmware source, board variant, serial routing, and bidirectional behavior are unknown. Identify and back up its configuration before deciding whether to adapt it. Neither upstream candidate has been shown to match the installed firmware.

## Recommended minimum architecture

Follow-up research identifies the Yuri-rage fork as the first small bridge candidate to evaluate. See [Related projects](related-projects.md) for the updated comparison.

Retain the proposed separate radio TCP endpoint and byte-transparent UART path. Prefer adapting the installed firmware if inspection establishes that it can meet the requirements. Otherwise, start from the basic MIT bridge, with bounded buffering, checked writes, isolated diagnostics, and one command-writing client for the radio endpoint. Pro adds configuration convenience but also filtering/routing and licensing work.

Before radio bench testing, address the OpenSoar transmission, initialization, framing, and acknowledgment defects together. Establish the real radio response rules before implementing ACK handling. Keep frequency validity explicit, require known active state before standby changes, and refresh state after reconnect or local radio changes.

These are proposed changes. This investigation implements documentation only.

## Open checks and next work package

| Check | Evidence needed |
| --- | --- |
| Exact radio | Model label, hardware revision, firmware version, matching installation manual |
| Electrical interface | Verified connector/pinout, serial settings, remote-control configuration, transceiver requirements |
| ESP32 resources | Board/SoC, UART ownership, boot and USB/debug behavior, existing level converters and wiring |
| Installed software | Firmware source/configuration and deployed OpenSoar version |
| Application integration | AR62xx availability over TCP, simultaneous navigation connections, status presentation |
| Protocol | Authoritative framing, ACK/NAK, CRC rules, status requests, and channel conversion vectors |
| Bridge reliability | Bidirectional all-byte tests, oversized bursts, slow clients, partial writes, reconnects, and concurrent navigation |
| Driver regression | Embedded zero/A5, short frames, invalid CRC, unknown initial state, two instances, absent/late responses |
| Physical acceptance | Frequency change and status readback on the real radio while preserving local controls and navigation |

Use the [Becker manual catalog](https://becker-avionics.com/manual-instructions-downloads/) to select the manual after identifying the device revision. No pinout or baud rate is approved by this review.

The next executable deliverable should be a host-side protocol harness with independently checked vectors and explicit tests for the source findings. A simulator based only on the same driver would reproduce its assumptions, so agreement with it alone is insufficient.

## Follow-up source verification (2026-11-08)

- The official release index records the Becker driver in OpenSoar 7.28.05 on 2023-03-24 and lists 7.46.26, dated 2026-10-30, at the top. The 7.46.26 notes still mark Becker support WIP. The tag is in the OpenSoaring organization repository, distinct from the August2111 fork used for the initial review. [Release index](https://opensoar.de/releases/) · [7.46.26 notes](https://opensoar.de/releases/v7.46.26/)
- The official tag resolves to b2e745cfddfe0150f6ed7db467ac18c321b38d01. Its driver still uses A5/14 framing, CRC polynomial 8005 hexadecimal, channel conversion, and a command containing both frequency values. It still passes a character pointer without the frame length and assigns ACK locally after the write. Received frequency fields update private state. This confirms the source findings at the release tag. The Android build and actual radio reply behavior still need testing. [Tagged AR62xx source](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp)
- Becker DV14307.03 Issue 06, section 2.5.3.2 (page 59), confirms J1 pins 2/9 as TX2+/−, 3/10 as RX2+/−, and 21 as D_GND (discrete-line ground). Pins 22 and 23 also carry D_GND in that table. Becker lists separate first hardware design manuals. The older tables label pin 2 TX2+ with conflicting direction text (IN in the direction column and OUT+ in the function column). Issue 06 marks it OUT. Pin functions also vary by revision, including J1-5. Match the installed part number, software version, and connector view before relying on a diagram. [Issue 06, page 59](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=59) · [First hardware design manual](https://becker-avionics.com/wp-content/uploads/2017/08/AR6201_IO_SW3050149.pdf#page=22) · [Manual catalog](https://becker-avionics.com/manual-instructions-downloads/)
- LXNAV rev. 21 v2.10, pages 10–11, shows separate AR6201 single-seat, AR6201 twin-seat, and RT6201 remote-head layouts. The AR6201 diagrams use interface 2 for the LXNAV adapter and pin 21 for its ground connection. The RT6201 example uses interface 1 for the adapter and interface 2 for the RCU. These diagrams include a separate RS232-to-RS422 Becker adapter on the LXNAV bridge path. The reseller listing identifies a J1 harness for that product. [LXNAV diagrams](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10) · [Navboys harness listing](https://www.navboys.com/lxnav-to-becker-ar6201-harness-for-radio-bridge-j1-plug.html)
- LXNAV describes enabling remote control and recommends checking Tandem. It gives minimum versions for its integration: bridge 1.16, Becker SCI1050S305 3.05 and SCI1051S305 1.49. SD-Link specifies a free J1, enabled Tandem mode, and AR6201 serial number 3001 or higher for its BLE adapter. These are product-specific compatibility statements. Neither establishes a universal cutoff, an ESP32 baud rate, or a working OpenSoar TCP setup. [LXNAV, page 10](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10) · [SD-Link prerequisites](https://sdlink.de/en/Manual/ar620x/)
- AlphaLima's three UART paths and Pro web configuration are confirmed in the earlier bridge review above. A transparent ESP32 bridge can leave radio framing to OpenSoar. Reusing the driver in firmware would shift protocol and state handling into the bridge and would carry the source defects unless repaired.

Verification consisted of release and source checks and visual inspection of the LXNAV diagrams. The installed part number, firmware, J1 occupancy, serial settings and bench behavior remain open.


## Verification record

Read the complete pinned driver and port write declarations, the basic sketch/configuration/license, and Pro configuration and forwarding paths. Confirmed cloned bridge HEADs match the recorded revisions. Reviewed documentation links and Markdown whitespace. All behavior claims above are static source observations or stated consequences. Runtime performance, toolchain compatibility, and physical interoperability remain untested.
