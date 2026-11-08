# Technical investigation

Evidence: [sources](sources.md). Source inspection unless labelled measured/reported. No application/device acceptance.

## Original source baseline

| Project | Reviewed revision | Commit date recorded in prior review | State recorded in prior review |
| --- | --- | --- | --- |
| OpenSoar | [4a3794a](https://github.com/August2111/OpenSoar/tree/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41) | 2026-10-21 | Not archived |
| ESP32 Serial Bridge | [bb42dc3](https://github.com/AlphaLima/ESP32-Serial-Bridge/tree/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf) | 2026-07-28 | Not archived |
| ESP32 Serial Bridge Pro | [bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/tree/bf263c1880d7630cadef5264dac9aa9654e4096e) | 2026-07-30 | Not archived |

Primary review: OpenSoaring `b2e745cfddfe0150f6ed7db467ac18c321b38d01` (S01/S02). Installed build unknown.

## Follow-up send reproduction — 2026-11-08

[S12 host regression](reviews/2026-11-08-becker-send.md): zero-containing frames truncated to 4/10 or 6/10 bytes; byte-span correction preserves 10/10. Synthetic transport evidence only.

## OpenSoar driver findings

Source: [AR62xx.cpp, 4a3794a](https://github.com/August2111/OpenSoar/blob/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41/src/Device/Driver/AR62xx.cpp); rechecked at S01. [Port.hpp](https://github.com/August2111/OpenSoar/blob/4a3794a02211eb03b21ddb8a6b8ec5ad62ee3b41/src/Device/Port/Port.hpp).

| Area | Source observation and consequence |
| --- | --- |
| Transmission | `Send()` ignores `msg_size` and passes a character pointer. The `string_view` overload determines length at the first zero byte, truncating binary commands. |
| Acknowledgment | `Send()` assigns ACK after writing. No receive-side response assignment or condition notification exists in this driver. Success does not establish radio acceptance. |
| Initialization | `radio_para` has uninitialized doubles. Both frequencies are read when constructing a command, before any guaranteed status reception. |
| Framing | Every `0xA5` resets reception, including inside payload or CRC. |
| Receive state | Static parser buffers and CRC deduplication state are shared across instances and survive device recreation. |
| Validation | Zero received CRC bypasses verification. Command-specific minimum lengths are unchecked before frequency fields are read. |
| Readback | Conversion is suppressed during sending. Frequencies update private state, while `NMEAInfo` only receives an alive update. |

### Protocol details to validate independently

| Source implementation | Independent evidence needed |
| --- | --- |
| 10-byte frame: A5 14 05 16, two 2-byte channels, 2-byte CRC | Framing, byte order, command semantics |
| CRC initial 0, polynomial 0x8005 | Vectors; zero-CRC exception |
| Paired frequency command | Active preservation, local-tuning race |
| ACK/NAK constants | Actual response format; constants prove no response bytes |
| Channel conversion | Valid/invalid channel vectors |
| Startup/readback | Query, broadcast timing, freshness, UI display |

## Becker driver follow-up — 2026-11-08

S11; R01/R03/R04/R07/R08. No candidate validated for operation.

### Most useful additional reference: XCSoar PR 449

[Werner86 AR62xx.cpp, 9854e3f](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/Device/Driver/AR62xx.cpp); [PR 449](https://github.com/XCSoar/XCSoar/pull/449): closed/unmerged at review.

| Rechecked source | Useful idea | Limitation |
| --- | --- | --- |
| `Send`, lines 232–235 | Passes the binary message **and its length** to `FullWrite`. | Reports transport completion, not radio acceptance. The old API returns a boolean; current OpenSoar's S02 overload takes a byte span and throws on errors. Porting the call literally would be wrong. |
| `FrequencyToId` / `IdToFrequency`, lines 247–318 | Uses integer kHz and a table of 16 channel suffixes; `PutFrequency` explicitly writes big-endian values. | Channel mapping still needs independent vectors and invalid-input checks. This is source evidence, not manufacturer validation. |
| `PutFrequency`, lines 391–397 | Attempts to refuse tuning before both cached frequencies are defined. | **The guard is incomplete:** members at lines 157/160 are not initialized by the constructor at line 211. At the same revision, [RadioFrequency.hpp lines 45–64](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/RadioFrequency.hpp#L45) declares an uninitialized default constructor; `IsDefined` reads that value. Inference: this guard cannot establish safe startup state. Explicit unknown initialization is required. |
| `SaveActiveAndPassiveFrequency`, lines 350–385 | Requires matching CRC and uses endian helpers to read both frequencies; does not reset merely because a payload byte is A5. | No buffer survives between receive calls, so split frames are lost. It returns after the first valid frequency frame, ignoring later updates in the same chunk. It does not check the declared frame length byte. |
| `PutFrequency`, lines 430–452 | Keeps the other cached frequency in the paired command. | Updates the cache before the write and has no freshness/response tracking. A failed write, stale cache or concurrent local tuning still needs explicit handling. |

**Reported**, source lines 39–48: single-frequency commands 1/2 may be undone by the control head; paired command 22/0x16 reportedly synchronizes it. Becker email/protocol revision unavailable. Binary A5/14 traffic; source comments calling it NMEA do not justify text processing.

### LK8000 and the other candidates

| Source / functions | Useful findings | Limits |
| --- | --- | --- |
| [LK8000 8d78c63](https://github.com/LK8000/LK8000/blob/8d78c63f599b894dbff217e007db44be06ee67ee/Common/Source/Devices/devAR620x.cpp), AR620xPutFreqStandby / AR620x_Convert_Answer | Invalidates requested standby; command 22 validates both; repeated channel messages decoded; strict CRC | No request correlation/freshness; SetAR620xStation ignores validity; static parser, A5 resets, unchecked command lengths; receive conversion suppressed during send |
| Same, SendCommand | Counted writes | Two writes, two 100 ms sleeps; returns true without response verification |
| Same, status decoder | Commands 3 volume, 4 squelch, 12 dual, 21 voltage, 64 TX/RX | Source observations, not verified protocol definitions |
| [LK8000 e1a6305](https://github.com/LK8000/LK8000/commit/e1a6305468878bf096d9874068f3c51ad3925fa9) | Integer kHz state/fraction arithmetic | Becker base conversion still floating point |
| [eku60 a9ab01c](https://github.com/eku60/XCSoar/blob/a9ab01cd4855db8b60deb59675ffe74b5d7a0b55/src/Device/Driver/AR62xx.cpp) | More private status, volume/swap | Uncounted write, local ACK, parser/state defects remain |
| [RadioDriver a794c4a](https://github.com/rbp28668/RadioDriver/blob/a794c4a994838b94d2b0bfa69ef76cda9a698957/RadioDriver/devAR620x.cpp), constructor/send/parser | Integer conversion, serial abstraction | Seeds 131.280/121.500 instead of observed state; repeated sends/static parser; no validated simulator |

Licenses inspected: OpenSoar/Werner/LK8000 driver GPL-2.0-or-later; RadioDriver Windows header GPL v2, Arduino header/repository license absent. File-level permission required; no code copied.

### Protocol documentation lead

[PR documentation report](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-667976007) · [redesign report](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-670971343) · [publication refusal report](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-679080020) · [issue 1092](https://github.com/XCSoar/XCSoar/issues/1092).

Historical reports only; current access policy unknown. [Becker service](https://becker-avionics.com/service-request/): contact lead, no submission.

Request after radio identification: matching remote-control protocol/revision; supported HW/SW; serial settings; status timing; paired/single-command behavior; framing/escaping/CRC; responses; channel vectors; publication permission. No independent protocol document/capture obtained.

### Consequences for the next increment

| Inspected OpenSoar revision | Driver result |
| --- | --- |
| [work26 41342db](https://github.com/OpenSoaring/OpenSoar/blob/41342dbabfa89a22b04af3a9ba4c8d9dbe9d52f5/src/Device/Driver/AR62xx.cpp) | Same blob as S01 |
| [topic/device-misc e0d9228](https://github.com/OpenSoaring/OpenSoar/blob/e0d9228a590605edb9e22b58bd0b3d48c34483fa/src/Device/Driver/AR62xx.cpp) | Same blob as S01 |
| [master e34b3dc](https://github.com/OpenSoaring/OpenSoar/blob/e34b3dca3f63bf534c79657b22d181449350b9ca/src/Device/Driver/AR62xx.cpp) | RadioFrequency include-path change only |

WP4 proposal: caller/state/UI mapping, then separate counted-send, unknown/fresh-state, stream-parser and response increments. Local-tuning race unresolved even after fresh paired readback.

Source-derived cases: 118.000 → 00 00 (send truncation); 119.030 → 00 A5 (parser reset). Not independent protocol vectors. Test every split position and multiple frames per read after framing validation.

**S11 verification:** pinned drivers, constructors, overloads, branch blobs, conversion history and discussions inspected. No execution/device tests. S12 supplies separate host evidence.

## ESP32 bridge comparison

| Candidate | Source findings | Limits |
| --- | --- | --- |
| [AlphaLima basic sketch](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/ESP32-Serial-Bridge.ino) | Separate TCP/UART paths; counted writes; no NMEA parser | Unconditional UART banners; drains beyond 1,023-byte capacity; unchecked writes; multiple writers; Bluetooth broadcasts |
| [Basic config](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/config.h) | 3 UARTs, TCP 8880–8882, 19200/8N1, AP/debug/OTA | Examples, not installed settings |
| [Basic license](https://github.com/AlphaLima/ESP32-Serial-Bridge/blob/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf/LICENSE) | MIT | Preserve notices |
| [Pro bf263c1](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro/blob/bf263c1880d7630cadef5264dac9aa9654e4096e/ESP32-Serial-Bridge-pro/ESP32_Serial_Bridge_pro.ino) | Persistent config/web/filtering/routes; counted UART chunks with filters off | TCP drains beyond 126 bytes; debug copy; unchecked writes; filters alter binary traffic; cross-port exclusion required |
| Pro licensing/build | No standalone license found despite README reference | Permission, web/SPIFFS dependencies and reproducibility unresolved |
| Installed bridge | Reported working navigation | Firmware/routing/returns unknown; preserve as backup |

## Architecture recommendation from the investigation

Current design: [architecture](architecture.md). Candidate order: Yuri, basic AlphaLima; no firmware selected. Required: bounded/checked forwarding, isolated diagnostics, exclusive command ownership, navigation coexistence.

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

## Follow-up source verification (2026-11-08)

Manual details below: **prior review** unless rechecked in S09/S10. Installed applicability unknown.

| Reference | Finding / limitation |
| --- | --- |
| [Release index](https://opensoar.de/releases/), [7.46.26](https://opensoar.de/releases/v7.46.26/) | 2026-10-30; Becker WIP; rechecked S09. Installed build unknown |
| [Tagged driver b2e745c](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp) | Same send/state defects; source only |
| [DV14307.03 Issue 06 §2.5.3.2 p.59](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=59) | J1 2/9 TX2+/−; 3/10 RX2+/−; 21/22/23 D_GND; discrete-line ground |
| [First-design manual p.22](https://becker-avionics.com/wp-content/uploads/2017/08/AR6201_IO_SW3050149.pdf#page=22), [catalog](https://becker-avionics.com/manual-instructions-downloads/) | Older pin 2 direction conflicts (IN vs OUT+); Issue 06 OUT. J1-5 also revision-dependent. Match part/SW/connector view |
| [LXNAV rev.21 v2.10 pp.10–11](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10), [harness listing](https://www.navboys.com/lxnav-to-becker-ar6201-harness-for-radio-bridge-j1-plug.html) | AR6201 adapter on interface 2, ground pin 21; RT6201 adapter on interface 1, RCU on 2; product uses RS232↔RS422 adapter |
| Same LXNAV manual | Remote control/Tandem; minimum bridge 1.16, Becker SCI1050S305 3.05 / SCI1051S305 1.49; product-specific |
| [SD-Link prerequisites](https://sdlink.de/en/Manual/ar620x/) | Free J1, Tandem, AR6201 serial ≥3001 for its BLE adapter; not universal ESP32 requirements |

## Original verification record

Pinned driver/port, bridge sketches/config/licenses and LXNAV diagrams inspected. No full build, bridge/radio execution or interoperability evidence.
