# Technical investigation

Original reviews dated 2026-11-08, preserved from [baseline fbed731](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/tree/fbed731cf5fa7e0f3106886182d14e2282b72a6b). This report records source/manual observations and dated follow-ups. [Sources and evidence](sources.md) identifies the rechecked scope; other historical verification statements below belong to the original reviews. S12 adds a measured host extraction of the send path. No full application build, ESP32 execution, radio bench test, aircraft test or measured interoperability is established here.

Current requirements, decisions and package order live in [Project baseline](project-baseline.md), [Architecture](architecture.md) and [Roadmap](roadmap.md). Their status takes precedence over recommendations in this research report.

## Original source baseline

| Project | Reviewed revision | Commit date recorded in prior review | State recorded in prior review |
| --- | --- | --- | --- |
| OpenSoar | [4a3794a](https://github.com/August2111/OpenSoar/tree/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41) | 2026-10-21 | Not archived |
| ESP32 Serial Bridge | [bb42dc3](https://github.com/AlphaLima/ESP32-Serial-Bridge/tree/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf) | 2026-07-28 | Not archived |
| ESP32 Serial Bridge Pro | [bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/tree/bf263c1880d7630cadef5264dac9aa9654e4096e) | 2026-07-30 | Not archived |

Dates and archive flags above are historical metadata recorded by the original review, not current maintenance claims. The subsequent OpenSoaring baseline is b2e745cfddfe0150f6ed7db467ac18c321b38d01 (S01/S02 in [Sources](sources.md)); the August2111 revision remains as provenance. Neither baseline identifies the installed firmware or Android build.

## Follow-up send reproduction — 2026-11-08

[Historical comparison and host evidence S12](reviews/2026-11-08-becker-send.md) supersede the unmeasured status of the send-length finding below. The PR-associated 2019 driver used explicit lengths. The 7.46.26 Send body was reproduced with a captured byte sink and corrected with a single byte-span call. Full application and physical acceptance remain open.

## OpenSoar driver findings

The original review reports the following observations in the [pinned AR62xx driver](https://github.com/August2111/OpenSoar/blob/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41/src/Device/Driver/AR62xx.cpp).

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

## Becker driver follow-up — 2026-11-08

**Question:** Which existing Becker implementations or protocol leads help preserve the active channel while setting standby? This bounded public-source investigation supports R01/R03/R04/R07/R08 and WP2/WP4. It authorizes no implementation or radio operation. Evidence is registered as S11 in [Sources](sources.md).

### Most useful additional reference: XCSoar PR 449

[Werner86's driver at 9854e3f](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/Device/Driver/AR62xx.cpp) is materially different from the eku60-derived OpenSoar driver. [PR 449](https://github.com/XCSoar/XCSoar/pull/449) is closed and unmerged; its head commit is dated 2020-08-14. It is a useful design reference, not a ready replacement.

| Rechecked source | Useful idea | Limitation |
| --- | --- | --- |
| `Send`, lines 232–235 | Passes the binary message **and its length** to `FullWrite`. | Reports transport completion, not radio acceptance. The old API returns a boolean; current OpenSoar's S02 overload takes a byte span and throws on errors. Porting the call literally would be wrong. |
| `FrequencyToId` / `IdToFrequency`, lines 247–318 | Uses integer kHz and a table of 16 channel suffixes; `PutFrequency` explicitly writes big-endian values. | Channel mapping still needs independent vectors and invalid-input checks. This is source evidence, not manufacturer validation. |
| `PutFrequency`, lines 391–397 | Attempts to refuse tuning before both cached frequencies are defined. | **The guard is incomplete:** members at lines 157/160 are not initialized by the constructor at line 211. At the same revision, [RadioFrequency.hpp lines 45–64](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/RadioFrequency.hpp#L45) declares an uninitialized default constructor; `IsDefined` reads that value. Inference: this guard cannot establish safe startup state. Explicit unknown initialization is required. |
| `SaveActiveAndPassiveFrequency`, lines 350–385 | Requires matching CRC and uses endian helpers to read both frequencies; does not reset merely because a payload byte is A5. | No buffer survives between receive calls, so split frames are lost. It returns after the first valid frequency frame, ignoring later updates in the same chunk. It does not check the declared frame length byte. |
| `PutFrequency`, lines 430–452 | Keeps the other cached frequency in the paired command. | Updates the cache before the write and has no freshness/response tracking. A failed write, stale cache or concurrent local tuning still needs explicit handling. |

The source preface, lines 39–48, supplies an important **reported** explanation: the author says single-frequency commands (IDs 1/2) can be undone by the AR6201 control head, whereas the paired command (decimal 22 / hexadecimal 16) causes a broadcast that synchronizes it. The author attributes this to email from Becker. That email and a matching protocol revision were not supplied, so this is a lead to verify for our hardware, not a universal rule. Do not assume that switching to a single-standby command would solve active-channel preservation.

Comments in this file call the traffic “NMEA”; the implementation actually handles binary A5/14 frames. Those comments do not justify newline handling or mixing this path with navigation sentences.

### LK8000 and the other candidates

The rechecked [LK8000 driver at 8d78c63](https://github.com/LK8000/LK8000/blob/8d78c63f599b894dbff217e007db44be06ee67ee/Common/Source/Devices/devAR620x.cpp) offers several smaller ideas:

- `AR620xPutFreqStandby` and `AR620x_Convert_Answer` invalidate the requested standby value and mark both frequencies valid on received command 22. Unlike OpenSoar, the decoder does not skip a repeated channel message based on its previous CRC. This is useful for state refresh; these flags alone do not prove freshness or correlate a reply to a specific request.
- [Commit e1a6305](https://github.com/LK8000/LK8000/commit/e1a6305468878bf096d9874068f3c51ad3925fa9) changed radio state to integer kHz and fractional-digit arithmetic to avoid rounding artifacts. The current Becker conversion still uses floating point in its base calculation; Werner's integer/table approach is a separate comparison lead.
- `AR620x_Convert_Answer` contains additional status decoding: command 3 volume, 4 squelch, 12 dual mode, 21 voltage and 64 TX/RX state. These are possible future readback references, not verified protocol definitions or approval to add controls.

LK8000 is not a complete fix: `SendCommand` writes twice with two 100 ms sleeps and returns true without verifying a response. Receive conversion is suppressed during sending; parser/global state, A5 resets and missing command-specific length checks remain. `SetAR620xStation` uses cached values without enforcing their validity. Its strict matching-CRC check contrasts with OpenSoar's zero-CRC bypass, but this comparison does not determine the radio's actual CRC exceptions.

The [eku60 branch at a9ab01c](https://github.com/eku60/XCSoar/blob/a9ab01cd4855db8b60deb59675ffe74b5d7a0b55/src/Device/Driver/AR62xx.cpp) adds volume/swap and more private status decoding, but still has the uncounted write, locally assigned ACK and parser/state weaknesses. It is not a repair source for those defects.

The [standalone RadioDriver at a794c4a](https://github.com/rbp28668/RadioDriver/blob/a794c4a994838b94d2b0bfa69ef76cda9a698957/RadioDriver/devAR620x.cpp) uses integer channel arithmetic and a serial abstraction, but `AR620x::AR620x` seeds active/standby with **131.280/121.500** instead of observed radio state. Copying those defaults would conflict with R01. Its send/parser behavior resembles LK8000, including repeated sends and static receive state. No independently validated simulator or response specification was found in the inspected material.

File headers identify OpenSoar, Werner and the inspected LK8000 driver as GPL-2.0-or-later. RadioDriver has no repository-level license in the inspected tree: its Windows driver header says GPL v2, while the inspected Arduino driver has no corresponding header. File-level provenance remains necessary before reuse. No source code was copied into this repository.

### Protocol documentation lead

The PR author [reported receiving Becker documentation](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-667976007), then [reported redesigning the driver against it](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-670971343). A later [comment reports Becker declined publication](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-679080020). This is historical testimony about access and publication, not proof of today's policy or access to the document itself. [Issue 1092](https://github.com/XCSoar/XCSoar/issues/1092) records the same public-documentation obstacle in 2022/2023 and a successful-test report for eku60's code, not our deployment.

[Becker's service-request page](https://becker-avionics.com/service-request/) is available as of this review. Proposed next evidence request, once Tom supplies the radio identity: the applicable **6200-series data-exchange/remote-control protocol**, its revision and supported hardware/software; serial settings; startup/status query and broadcast timing; paired versus single-channel behavior with a control head; frame-length/escaping/CRC rules; actual acceptance/rejection behavior; and example channel/CRC frames. Ask what may be quoted or published in this public project. No contact or form submission was made. No authoritative public protocol document or independent response capture was obtained in this search.

### Consequences for the next increment

The evidence still favors targeted OpenSoar work within the proposed architecture. The inspected OpenSoar `work26` revision [41342db](https://github.com/OpenSoaring/OpenSoar/blob/41342dbabfa89a22b04af3a9ba4c8d9dbe9d52f5/src/Device/Driver/AR62xx.cpp) and `topic/device-misc` revision [e0d9228](https://github.com/OpenSoaring/OpenSoar/blob/e0d9228a590605edb9e22b58bd0b3d48c34483fa/src/Device/Driver/AR62xx.cpp) have the same driver blob as S01. The inspected `master` revision [e34b3dc](https://github.com/OpenSoaring/OpenSoar/blob/e34b3dca3f63bf534c79657b22d181449350b9ca/src/Device/Driver/AR62xx.cpp) differs only in the RadioFrequency include path. No repair was found in those inspected versions; this is not an exhaustive branch/fork audit.

Proposed WP4 scope: map OpenSoar's standby caller, receive lifecycle and displayed result, then specify separate increments for explicit unknown/fresh state, counted transmission, a parser that survives arbitrary stream chunks, and response/readback reporting. A byte-count repair alone must not be treated as bench-ready: making a command reach the radio does not fix its potentially stale active value. A fresh paired read still leaves a possible race with local tuning; WP2 must establish how the protocol handles this, or keep that limitation open.

Two **source-derived illustrations**, not independent protocol vectors, make the existing defects concrete. S11 did not execute them; the separately prepared S12 host review supplies transport evidence for the zero-byte case:

| Illustration | Inferred consequence / proposed later regression |
| --- | --- |
| In S01's mapping, active 118.000 encodes as 00 00. A constructed frequency frame therefore starts A5 14 05 16 00. | The string-based send stops after four bytes instead of transmitting the ten-byte frame. [S12](reviews/2026-11-08-becker-send.md#measured-host-reproduction) independently exercises the extracted send path with a host byte sink and records this result; that is transport evidence, not protocol acceptance. |
| In the inspected channel mapping, 119.030 corresponds to index 00 A5. | A channel payload can contain the parser's header byte. After confirming framing/escaping independently, include this case and every split position, plus two frequency updates in one receive chunk. |

Neither the inspected ACK constants nor LK8000's duplicate sends establish acknowledgment bytes. Keep “written”, “accepted” and “observed radio state” separate. WP1 identity/settings and WP2 independent protocol evidence remain open; no candidate is accepted for runtime use.

**Verification — S11:** On 2026-11-08, read the pinned OpenSoar, Werner, eku60, LK8000 and RadioDriver source paths above; checked Werner's RadioFrequency constructor, current OpenSoar port overloads, LK8000 conversion history, selected branch blob identities, PR metadata/discussion and Becker's service page. Cross-checked findings against R01/R03/R04/R07/R08, local Markdown links/anchors and whitespace. S11 outcomes are static observations, stated inferences or attributed reports; S12's separate host evidence is linked without rerunning it. No application build, harness execution, serial/TCP traffic, radio/ESP32 test or hardware change was performed in S11; no installed-system compatibility or acceptance is established.

## ESP32 bridge comparison

### Basic bridge

The [sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) implements separate TCP servers and UART arrays. TCP-to-UART and UART-to-TCP use explicit byte counts. There is no NMEA parser in those paths.

However, startup prints go to all three UARTs unconditionally. Disabling debug alone does not remove them. Receive loops continue draining after buffer indices saturate, so bursts exceeding the 1,023-byte forwarded capacity lose bytes. Write results are ignored, multiple clients may write to the same UART, and optional Bluetooth input broadcasts to every UART.

The [configuration](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/config.h) defaults to three UARTs, TCP ports 8880–8882, 19200 baud, 8N1, AP mode, debug, and OTA. These defaults describe the project, not the Becker requirements or installed board.

The repository includes an [MIT license](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/LICENSE). It remains a comparison candidate, subject to the forwarding fixes and hardware checks; see ADR-02 for the current evaluation order.

### Pro bridge

The [Pro sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/blob/bf263c1880d7630cadef5264dac9aa9654e4096e/ESP32-Serial-Bridge-pro/ESP32_Serial_Bridge_pro.ino) adds persistent configuration, a web interface, filtering, Bluetooth, and configurable cross-port forwarding.

With both NMEA filter counts zero, UART reception uses counted binary chunks. Enabling filters introduces dollar-sign resets, text matching, and newline insertion. Radio filters must therefore be disabled. Cross-port routes must exclude radio traffic.

TCP reception still drains beyond its 126-byte forwarded capacity. Input is also copied to the debug UART, and write counts are ignored. The serial receive path does stop at its chunk limit.

No standalone license file appeared in the reviewed tree, despite the README referring to one. Reuse permission needs clarification. The web stack and SPIFFS assets also require a reproducible dependency/build audit before choosing this version.

### Existing installed bridge

Its successful navigation service is user-reported. Firmware source, board variant, serial routing, and bidirectional behavior are unknown. Identify and back up its configuration before deciding whether to adapt it. Neither upstream candidate has been shown to match the installed firmware.

## Architecture recommendation from the investigation

Follow-up research identifies the Yuri-rage fork as the first small bridge candidate to evaluate. See [Related projects](related-projects.md) for the updated comparison.

The separate radio TCP endpoint and byte-transparent UART path remain proposals. Prefer investigating the installed firmware first. If it cannot meet requirements, evaluate the Yuri-rage fork before basic AlphaLima, as suggested by the follow-up research. No firmware is selected. Required evaluation includes bounded buffering, checked writes, isolated diagnostics and command-writer ownership. Pro adds filtering/routing and licensing work. [ADR-01/02](architecture.md#decision-record) own the current alternatives and decision gates.

Before radio bench testing, resolve applicable transmission, initialization, framing and response-handling risks. Do so in separate reviewable increments with targeted checks, following WP4/WP5. Establish the real radio response rules before implementing ACK handling. Keep frequency validity explicit, require known active state before standby changes, and refresh state after reconnect or local radio changes.

These are proposed changes. This investigation implements documentation only.

## Detailed open checks

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

The first executable deliverable remains a host-side protocol harness (WP5), after independent expectations and its scope are established. It is not the next project task: WP1 inventory comes first. A simulator based only on the same driver would reproduce its assumptions, so agreement with it alone is insufficient. See the [roadmap](roadmap.md) for dependencies and acceptance criteria.

## Follow-up source verification (2026-11-08)

- The original review recorded the Becker driver in OpenSoar 7.28.05 on 2023-03-24 and 7.46.26, dated 2026-10-30, at the top of the release index at that time. That review reported Becker support marked WIP in the 7.46.26 notes. The release page could not be retrieved during the documentation cleanup (S04); this is not a newly verified release claim. The tag is in the OpenSoaring organization repository, distinct from the August2111 fork used for the initial review. [Release index](https://opensoar.de/releases/) · [7.46.26 notes](https://opensoar.de/releases/v7.46.26/)
- The official tag resolves to b2e745cfddfe0150f6ed7db467ac18c321b38d01. Its driver still uses A5/14 framing, CRC polynomial 8005 hexadecimal, channel conversion, and a command containing both frequency values. It still passes a character pointer without the frame length and assigns ACK locally after the write. Received frequency fields update private state. This confirms the source findings at the release tag. The Android build and actual radio reply behavior still need testing. [Tagged AR62xx source](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp)
- Becker DV14307.03 Issue 06, section 2.5.3.2 (page 59), confirms J1 pins 2/9 as TX2+/−, 3/10 as RX2+/−, and 21 as D_GND (discrete-line ground). Pins 22 and 23 also carry D_GND in that table. Becker lists separate first hardware design manuals. The older tables label pin 2 TX2+ with conflicting direction text (IN in the direction column and OUT+ in the function column). Issue 06 marks it OUT. Pin functions also vary by revision, including J1-5. Match the installed part number, software version, and connector view before relying on a diagram. [Issue 06, page 59](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=59) · [First hardware design manual](https://becker-avionics.com/wp-content/uploads/2017/08/AR6201_IO_SW3050149.pdf#page=22) · [Manual catalog](https://becker-avionics.com/manual-instructions-downloads/)
- LXNAV rev. 21 v2.10, pages 10–11, shows separate AR6201 single-seat, AR6201 twin-seat, and RT6201 remote-head layouts. The AR6201 diagrams use interface 2 for the LXNAV adapter and pin 21 for its ground connection. The RT6201 example uses interface 1 for the adapter and interface 2 for the RCU. These diagrams include a separate RS232-to-RS422 Becker adapter on the LXNAV bridge path. The reseller listing identifies a J1 harness for that product. [LXNAV diagrams](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10) · [Navboys harness listing](https://www.navboys.com/lxnav-to-becker-ar6201-harness-for-radio-bridge-j1-plug.html)
- LXNAV describes enabling remote control and recommends checking Tandem. It gives minimum versions for its integration: bridge 1.16, Becker SCI1050S305 3.05 and SCI1051S305 1.49. SD-Link specifies a free J1, enabled Tandem mode, and AR6201 serial number 3001 or higher for its BLE adapter. These are product-specific compatibility statements. Neither establishes a universal cutoff, an ESP32 baud rate, or a working OpenSoar TCP setup. [LXNAV, page 10](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10) · [SD-Link prerequisites](https://sdlink.de/en/Manual/ar620x/)
- AlphaLima's three UART paths and Pro web configuration are confirmed in the earlier bridge review above. A transparent ESP32 bridge can leave radio framing to OpenSoar. Reusing the driver in firmware would shift protocol and state handling into the bridge and would carry the source defects unless repaired.

Verification consisted of release and source checks and visual inspection of the LXNAV diagrams. The installed part number, firmware, J1 occupancy, serial settings and bench behavior remain open.


## Original verification record

Read the complete pinned driver and port write declarations, the basic sketch/configuration/license, and Pro configuration and forwarding paths. Confirmed cloned bridge HEADs match the recorded revisions. Reviewed documentation links and Markdown whitespace. All behavior claims above are static source observations or stated consequences. Runtime performance, toolchain compatibility, and physical interoperability remain untested.
