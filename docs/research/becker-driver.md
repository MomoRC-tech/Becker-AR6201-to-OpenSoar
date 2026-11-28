<a id="top"></a>

# 📻 Becker driver research

🏠 [Overview](../../README.md) › 🔬 Research › **Becker driver**

Reference for the Becker AR62xx serial protocol, the stock OpenSoar driver and evidence from other integrations.

> [!NOTE]
> **Status:** the project uses the built-in Becker AR62xx driver. Public sources and synthetic checks establish a candidate wire format. Standby tuning on the project radio awaits the [remaining checks](../project/open-items.md#software-and-acceptance).

<a href="#opensoar-driver"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-becker-driver-1-dark.svg"><img src="../images/nav/menu-research-becker-driver-1.svg" alt="1 · OpenSoar driver findings" height="30"></picture></a> <a href="#other-implementations"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-becker-driver-2-dark.svg"><img src="../images/nav/menu-research-becker-driver-2.svg" alt="2 · Other implementations" height="30"></picture></a> <a href="#consequences"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-becker-driver-3-dark.svg"><img src="../images/nav/menu-research-becker-driver-3.svg" alt="3 · OpenSoar setup" height="30"></picture></a> <a href="#manuals"><picture><source media="(prefers-color-scheme: dark)" srcset="../images/nav/menu-research-becker-driver-4-dark.svg"><img src="../images/nav/menu-research-becker-driver-4.svg" alt="4 · Manuals and commercial adapters" height="30"></picture></a> <!-- page-menu -->

Related: 📶 [Bridge firmware](bridge-firmware.md) · 📚 [Sources](sources.md) · ❓ [Open items](../project/open-items.md#software-and-acceptance)

---

<a id="opensoar-driver"></a>

## 1. 🔍 OpenSoar driver findings

The findings were checked against OpenSoar 7.46.26.1, test build 7.47.27.t09 and the default-branch source (S01, S02, S228, S237).

| Area | Finding |
| --- | --- |
| Sending | `Send()` ignores `msg_size` and passes a character pointer. The `string_view` overload stops at the first zero byte, so binary commands get cut off. |
| Acknowledgment | `Send()` sets ACK itself after writing; nothing on the receive side sets it. "Success" says nothing about the radio accepting the command. |
| Start-up | `radio_para` holds uninitialised values, and both frequencies are read to build a command before any status has arrived. |
| Framing | Every `0xA5` byte restarts reception, even inside the payload or CRC. |
| Receive state | Parser buffers and CRC state are static: shared between instances and kept across device restarts. |
| Validation | A received CRC of zero skips the check; frame lengths are not checked before reading frequencies. |
| Readback | Received frequencies only update private state; OpenSoar's display only gets an "alive" signal. |

The [host test](reviews/2026-11-08-becker-send.md) passed 4 or 6 of 10 bytes to the sink for two zero-containing frames. Both writes returned success. An explicit byte count preserved all 10 bytes in the harness (S12).

<a id="protocol-details"></a>

### 1.1 Frame layout and checksum

Werner86's public driver notes describe the `0x14` 62XX family data exchange protocol after he received Becker's documentation. OpenSoar and LK8000 use the same binary layout for the paired-channel write. Werner86 and LK8000 also decode a radio status frame with that layout (S228).

| Byte offset | Field | Paired-channel frame |
| --- | --- | --- |
| 0 | Start byte | `A5` |
| 1 | Protocol ID | `14` |
| 2 | Body length | `05` for one frame ID byte and four channel bytes. Total size is body length + 5. |
| 3 | Frame ID | `16` hex, 22 decimal |
| 4–5 | Active channel | Unsigned 16-bit channel ID, high byte first |
| 6–7 | Standby channel | Unsigned 16-bit channel ID, high byte first |
| 8–9 | CRC | Unsigned 16-bit value, high byte first |

The implementations calculate the CRC over bytes 0–7, including the header and length, with polynomial `0x8005`, initial value `0x0000`, most-significant bit first, and no final XOR or reflection. LK8000 places the general CRC at offsets `length + 3` and `length + 4`. The length and CRC together distinguish a candidate frame from a payload byte equal to `A5` (S228, S229).

A public Becker manual for a different device documents the same CRC polynomial and includes the check value `A5 14 01 20` → `43 E3` (S245). Use it as an additional checksum regression vector. Its `0x1C` beacon-status protocol does not establish AR6201 commands, acknowledgments, limits or timing; see [documentation search](#public-documentation-search).

### 1.2 Channel IDs and synthetic examples

Use the **displayed channel label** in integer kHz in the public mapping. Divide it by 100 using integer division, subtract 1180 and multiply by 16. Add the zero-based position of the last two digits in this list (S228, S229).

```text
00 05 10 15 25 30 35 40 50 55 60 65 75 80 85 90
```

The mapping gives IDs 0–3039 for labels 118.000–136.990. For example `118.000` and `118.005` map to different IDs even though Becker lists the same 118.0000 MHz carrier for both at different channel spacings. The Becker manual confirms the label convention and channel counts (S230).

| Active / standby label | Active / standby ID (hex) | Complete synthetic frame (hex) | Case |
| --- | --- | --- | --- |
| 118.000 / 123.500 | `0000` / `0370` | `A5 14 05 16 00 00 03 70 53 17` | Host sink received 4 bytes (S12) |
| 123.000 / 118.000 | `0320` / `0000` | `A5 14 05 16 03 20 00 00 66 B7` | Host sink received 6 bytes (S12) |
| 123.000 / 124.000 | `0320` / `03C0` | `A5 14 05 16 03 20 03 C0 6E 37` | Host sink received 10 bytes (S12) |
| 123.000 / 123.830 | `0320` / `03A5` | `A5 14 05 16 03 20 03 A5 6F 69` | `A5` in the payload exercises receive framing (S229) |
| 123.000 / 119.935 | `0320` / `0136` | `A5 14 05 16 03 20 01 36 E0 00` | Zero only in the final CRC byte. The text write would send 9 bytes (S229). |
| 123.000 / 118.810 | `0320` / `0082` | `A5 14 05 16 03 20 00 82 65 BB` | One of the distinct labels in the EasyVFR report (S229, S232) |
| 123.000 / 118.815 | `0320` / `0083` | `A5 14 05 16 03 20 00 83 E5 BE` | The other label in that report (S229, S232) |

The first 256 channel IDs (118.000–119.590, at the listed steps) contain a zero high byte. Either slot in this range triggers OpenSoar's text truncation. Across all 9,241,600 pairings of the listed IDs, the calculated CRC never equals `0x0000` for this exact frame. The `119.935` example shows that a single CRC byte can still be `00`. OpenSoar's receive-side exception for an all-zero CRC is a property of that implementation (S228, S229).

### 1.3 Paired tuning and radio status

| Behaviour | What the sources show |
| --- | --- |
| Single-channel IDs 1 and 2 | Werner86 reports that the built-in AR6201 head can restore the old value because it misses the single-channel update. He attributes that behaviour to a Becker reply. |
| Paired write, ID 22 | The open drivers put the requested standby ID alongside the cached active ID. The report that the paired change synchronizes the head is consistent with the earlier XCSoar contributor's AR6201 bench results. |
| Reported state, ID 22 | Werner86 describes radio broadcasts. His receiver CRC-checks both channels. LK8000 marks requested state invalid and sets both channel validity flags when it decodes a frequency frame. Its two counted writes and fixed waits do not match a return frame to the request. |
| Other frame IDs in LK8000 | Its receive code interprets 3 as volume, 4 as squelch, 12 as dual watch, 21 as voltage and 64 as TX/RX status. The decoder masks bit 7 of the ID. The `QUERY` name in the source supplies no working query sequence. |
| OpenSoar send result | The app sets ACK locally after the write. Compare an outbound frame, any radio-originated state and the front-panel result as separate observations. |

Source comparison and author reports are in S228. A paired write can carry a stale active value if the pilot changes it between the last status and the write. A fresh read reduces that interval. The radio determines the order in which the local and remote changes take effect. See [open items](../project/open-items.md#software-and-acceptance) for the evidence still needed on the matching unit.

<a id="failure-history"></a>

### 1.4 Failure history and capture search

| Evidence | What it supports | Limit |
| --- | --- | --- |
| Counted write replaced by a text-pointer call | The write regression is visible in the OpenSoar diff; the byte-span TODO remains (S237) | Older successful radio tests used earlier implementations |
| Original driver's local ACK | A workaround for the author's report that actions occurred without an acknowledgment (S238) | Does not define Becker's actual reply semantics |
| LK8000 resend and receive pause | Two counted writes, 100 ms pauses and suppression of old buffered status were introduced against station overwrite (S239) | A driver workaround, not a specified Becker timing requirement |
| LK8000 validity flags | Requested state is invalidated and becomes valid on received radio data (S239) | No request-specific reply correlation |
| XCSoar-fork test with no radio action | The author reported Android-to-XCVario data; the reply identified Tandem configuration as a prerequisite (S240) | No raw capture or final radio acceptance result |

The public search found no independent report explicitly diagnosing this exact null-byte truncation, and no usable timestamped two-direction Becker capture with a matching radio/software identity (S241). The same OpenSoar file in two forks is code reuse, not independent confirmation. The EasyVFR `118.815` / `118.810` report supplies a test pair, not a diagnosed instance of this bug (S232).

For a first PR, keep the explicit-length correction and a driver-level recording-port regression separate from reply handling. Reproduce the failure before the correction, then check zero payload/CRC bytes, a nonzero control, partial writes and write failures. This host proof does not need the radio. Full state-confirmation and local-control acceptance stay with the [protocol and radio checks](../project/open-items.md#software-and-acceptance). [Issue #5](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/5) holds the contribution workflow.

---

<a id="other-implementations"></a>

## 2. 📚 Other implementations

S11 and S228 record the code comparison and the contributors' own hardware reports.

### 2.1 XCSoar PR 449 (Werner86)

[Werner86 AR62xx.cpp, 9854e3f](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/Device/Driver/AR62xx.cpp) — [PR 449](https://github.com/XCSoar/XCSoar/pull/449) was closed without merging.

| Code | Useful idea | Limitation |
| --- | --- | --- |
| `Send`, lines 232–235 | Passes the binary message **and its length** to `FullWrite`. | Only proves the bytes left, not that the radio accepted them. The API differs from today's OpenSoar (byte span, throws on error), so it cannot be copied literally. |
| `FrequencyToId` / `IdToFrequency`, lines 247–318 | Integer kHz and a table of 16 channel suffixes. Its frame writer stores the IDs big-endian. | The synthetic channel comparison covers the public mapping (S229). |
| `PutFrequency`, lines 391–397 | Tries to refuse tuning until both cached frequencies are known. | **Does not work:** the members (lines 157/160) are never initialised ([RadioFrequency.hpp](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/RadioFrequency.hpp#L45) has an uninitialised default constructor). An explicit "unknown" state is required. |
| `SaveActiveAndPassiveFrequency`, lines 350–385 | Checks the CRC, reads both frequencies, does not reset on an A5 byte in the payload. | Loses frames split across reads, ignores later frames in the same chunk, does not check the length byte. |
| `PutFrequency`, lines 430–452 | Keeps the other cached frequency in the paired command. | Updates the cache before writing; no freshness or reply tracking. |

### 2.2 LK8000 and other drivers

| Source | Useful findings | Limits |
| --- | --- | --- |
| [LK8000 8d78c63](https://github.com/LK8000/LK8000/blob/8d78c63f599b894dbff217e007db44be06ee67ee/Common/Source/Devices/devAR620x.cpp), `AR620xPutFreqStandby` / `AR620x_Convert_Answer` | Marks the requested standby as invalid until confirmed; command 22 validates both; strict CRC | No request/reply matching; static parser; resets on A5; lengths unchecked |
| Same, `SendCommand` | Counted writes | Two writes with 100 ms sleeps; returns true without checking a reply |
| [LK8000 e1a6305](https://github.com/LK8000/LK8000/commit/e1a6305468878bf096d9874068f3c51ad3925fa9) | Integer kHz arithmetic | Base conversion still floating point |
| [eku60 a9ab01c](https://github.com/eku60/XCSoar/blob/a9ab01cd4855db8b60deb59675ffe74b5d7a0b55/src/Device/Driver/AR62xx.cpp) | More status, volume and swap | Same write, ACK and parser problems as OpenSoar |
| [RadioDriver a794c4a](https://github.com/rbp28668/RadioDriver/blob/a794c4a994838b94d2b0bfa69ef76cda9a698957/RadioDriver/devAR620x.cpp) | Integer conversion, serial abstraction; Teensy and Windows examples | Starts with fixed frequencies 131.280/121.500 instead of reading the radio |

More: LK8000 [PR 941](https://github.com/LK8000/LK8000/pull/941), [PR 940](https://github.com/LK8000/LK8000/pull/940).

**Licenses:** OpenSoar, Werner86 and LK8000 drivers are GPL-2.0-or-later; RadioDriver's Windows header is GPL v2, its Arduino part has no license. No code has been copied.

### 2.3 Historical XCSoar work

| Reference | What it shows |
| --- | --- |
| [PR 189](https://github.com/XCSoar/XCSoar/pull/189), [190](https://github.com/XCSoar/XCSoar/pull/190), [449](https://github.com/XCSoar/XCSoar/pull/449) | Closed without merging. PR 189 reports AR6201 tuning and reconnect tests on Windows/UNIX |
| [Issue 1092](https://github.com/XCSoar/XCSoar/issues/1092) | Reports eku60's success and that Becker would not allow the protocol to be published |

### 2.4 Getting the protocol document

Earlier discussions: [documentation](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-667976007) · [redesign](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-670971343) · [publication refused](https://github.com/XCSoar/XCSoar/pull/449#issuecomment-679080020). Contact: [Becker service request](https://becker-avionics.com/service-request/).

Use the public format for bench diagnosis. The [remaining protocol questions](../project/open-items.md#software-and-acceptance) are specific to the installed radio and can be sent to Becker with its identity. [Issue #4](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/4) holds the research discussion.

<a id="evidence-contacts"></a>

### 2.5 Evidence contacts

| Priority | Contact | Request |
| --- | --- | --- |
| First | Becker technical support via its service form | Revision-matched specification, supported standby operation, reply semantics, reference exchanges and permission to publish examples (S243) |
| Next | Werner86 | Document revision and retained captures from the redesign based on Becker's documentation (S228) |
| Next | Ulli Heynen / AlphaLima | Background and any retained recordings for the resend/receive-pause change (S239) |
| Next | Michael Hoffmann / SD-Link | Retained documentation, captures and the reliability advice mentioned privately after an AR6201 success report (S246) |
| Additional | eku60 / tb59427 | Exact test versions and any serial recordings from the reported radio tests (S228, S240) |
| Additional | calif on volavoile.net | Availability and terms of the channel-memory tool; no public source or trace download was found in the thread (S242) |

Ask for raw bytes in both labelled directions, timestamps, radio model and CM/CH software, interface/settings and the associated user actions. Keep full identifiers and original logs private under the repository rules. Retain public, sanitised evidence with its provenance and permission. See [open items](../project/open-items.md#software-and-acceptance) for the protocol questions. The owner has sent the Becker documentation request; no reply or protocol PDF has been supplied to this project. The other contacts are leads only.

<a id="public-documentation-search"></a>

### 2.6 Public documentation search

| Public material | Use for the PR |
| --- | --- |
| Werner86's documented driver | Frequency-control subset and the author's account of receiving Becker's document (S228) |
| Becker BD406/PBD406 manual | Independent manufacturer checksum examples and a related packet envelope; a different device/protocol (S245) |
| Becker's public manual index | Identifies installation and maintenance documents; no separate AR6201 protocol specification listed in the inspected entries (S244) |

The follow-up search did not locate a downloadable complete AR6201 protocol specification (S247). The name “62XX Transceiver Family Data Exchange Protocol” is the public driver's description of protocol ID 0x14; the original PDF's actual title, revision and document number have not been checked. Werner86 reports Becker's historical reason for controlled distribution as knowing the users to notify of changes (S228). That is a 2020 account, not confirmation of current policy.

The next evidence requests stay in [open items](../project/open-items.md#software-and-acceptance). No AR6201 ACK behavior or radio timing should be inferred from the beacon-decoder manual.

---

<a id="consequences"></a>

## 3. 📱 OpenSoar setup

| OpenSoar device setting | Value |
| --- | --- |
| Driver | **Becker AR62xx**, included in the standard app |
| Port type | **TCP client** |
| Host | Bridge address (default hotspot address `10.10.10.10`) |
| TCP port | Configured Becker port (default `4352`) |

The bridge forwards the driver's bytes both ways. For a standby tuning bench check, compare both frequencies on the Becker front panel before and after the request. The stock driver can report success without a radio reply (S01/S02).

---

<a id="manuals"></a>

## 4. 📖 Manuals and commercial adapters

| Reference | Finding |
| --- | --- |
| Becker Issue 06 and first-design manuals (S03, S10, S163, S230) | Issue 06 lists both J1 interfaces and documents Tandem synchronization. Use the manual that matches the part number and CM/CH versions. The selected pins are on the [wiring page](../hardware/wiring.md#becker). |
| LXNAV radio bridge (S230) | Its single-seat AR6201 adapter uses interface 2, matching the project's selected port. It requires Tandem and lists Becker minimum software SCI1050S305 3.05 and SCI1051S305 1.49. The system installation manual says to check for the Tandem menu on older units around serial 3000. |
| AIR Avionics ACD-57 (S230) | The manual lists testing with AR6201-(022) and software versions 4.06 and 2.06. The Becker diagram shows interface 2. Its public external control protocol is on the ACD side of the adapter. |
| UTRONIX Aviation Companion 422 and SkyDemon (S231) | UTRONIX specifies a configurable 9600-baud default, J1 interface 1, Tandem and AR620x serial above 3000 for this adapter. SkyDemon documents both RX and TX for Becker. A forum user separately reports interface 1 advice from Becker Support and lists 9600 8N1 (S232). |
| SD-Link and EasyVFR (S231, S232) | SD-Link specifies a free J1, Tandem and AR6201 serial ≥ 3001 for its adapter. EasyVFR added a Becker-specific tuning mode. A tester reported tuning worked with beta 4.7.6 and flagged a displayed `118.810` after selecting `118.815`. |

The Windows RadioDriver example uses `9600,n,8,1` on its radio link (S231). This supports the candidate bench setting. The separate LX Navigation bridge manual gives 9600 8N1 at the **LX instrument's user port**, upstream of its RS-422 adapter. The published setups show which port each vendor uses. See [radio configuration](../project/open-items.md#software-and-acceptance) for verification on the selected interface.

---

<kbd>[⬆️ Back to top](#top)</kbd> <kbd>[🏠 Overview](../../README.md)</kbd>
