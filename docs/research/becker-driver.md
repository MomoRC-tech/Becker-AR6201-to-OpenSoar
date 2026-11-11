<a id="top"></a>

# 📻 Becker driver research

🏠 [Overview](../../README.md) › 🔬 Research › **Becker driver**

What the OpenSoar AR62xx radio driver does today, how other implementations solve the same problems, and which protocol facts are still missing.

> [!NOTE]
> **Status:** source review (S11) plus one host test (S12); nothing tested on a radio. Driver work is on hold until the hardware is finished.

📑 **Contents**

1. [OpenSoar driver findings](#opensoar-driver)
2. [Other implementations](#other-implementations)
3. [What this means for the driver work](#consequences)
4. [Manuals and commercial adapters](#manuals)

Related: 📶 [Bridge firmware](bridge-firmware.md) · 📚 [Sources](sources.md) · ❓ [Open items](../project/open-items.md#software-and-acceptance)

---

<a id="opensoar-driver"></a>

## 1. 🔍 OpenSoar driver findings

Reviewed: OpenSoar 7.46.26, [AR62xx.cpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp) and [Port.hpp](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Port/Port.hpp) (S01/S02).

| Area | Finding |
| --- | --- |
| Sending | `Send()` ignores `msg_size` and passes a character pointer. The `string_view` overload stops at the first zero byte, so binary commands get cut off. |
| Acknowledgment | `Send()` sets ACK itself after writing; nothing on the receive side sets it. "Success" says nothing about the radio accepting the command. |
| Start-up | `radio_para` holds uninitialised values, and both frequencies are read to build a command before any status has arrived. |
| Framing | Every `0xA5` byte restarts reception, even inside the payload or CRC. |
| Receive state | Parser buffers and CRC state are static: shared between instances and kept across device restarts. |
| Validation | A received CRC of zero skips the check; frame lengths are not checked before reading frequencies. |
| Readback | Received frequencies only update private state; OpenSoar's display only gets an "alive" signal. |

**Host test (S12, [review](reviews/2026-11-08-becker-send.md)):** with synthetic data, frames containing zero bytes are cut to 4 or 6 of 10 bytes. Passing the byte span instead sends all 10.

<a id="protocol-details"></a>

### 1.1 Protocol details still to confirm independently

| What the code assumes | Independent evidence needed |
| --- | --- |
| 10-byte frame: A5 14 05 16, two 2-byte channels, 2-byte CRC | Framing, byte order, command meaning |
| CRC with initial value 0, polynomial 0x8005 | Test vectors; the zero-CRC exception |
| Paired frequency command | Active frequency kept; race with local tuning |
| ACK/NAK constants | The radio's real reply format |
| Channel conversion | Valid and invalid channel vectors |
| Start-up and readback | Query, broadcast timing, freshness, display |

---

<a id="other-implementations"></a>

## 2. 📚 Other implementations

Reviewed 2026-11-08 (S11). None is validated on a radio.

### 2.1 XCSoar PR 449 (Werner86)

[Werner86 AR62xx.cpp, 9854e3f](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/Device/Driver/AR62xx.cpp) — [PR 449](https://github.com/XCSoar/XCSoar/pull/449) was closed without merging.

| Code | Useful idea | Limitation |
| --- | --- | --- |
| `Send`, lines 232–235 | Passes the binary message **and its length** to `FullWrite`. | Only proves the bytes left, not that the radio accepted them. The API differs from today's OpenSoar (byte span, throws on error), so it cannot be copied literally. |
| `FrequencyToId` / `IdToFrequency`, lines 247–318 | Integer kHz and a table of 16 channel suffixes; writes big-endian values explicitly. | Channel mapping still needs independent test vectors. |
| `PutFrequency`, lines 391–397 | Tries to refuse tuning until both cached frequencies are known. | **Does not work:** the members (lines 157/160) are never initialised ([RadioFrequency.hpp](https://github.com/Werner86/XCSoar/blob/9854e3f3ba6ed92c2094cdaa4afa8278d67f8f0c/src/RadioFrequency.hpp#L45) has an uninitialised default constructor). An explicit "unknown" state is required. |
| `SaveActiveAndPassiveFrequency`, lines 350–385 | Checks the CRC, reads both frequencies, does not reset on an A5 byte in the payload. | Loses frames split across reads, ignores later frames in the same chunk, does not check the length byte. |
| `PutFrequency`, lines 430–452 | Keeps the other cached frequency in the paired command. | Updates the cache before writing; no freshness or reply tracking. |

Comments in the code (lines 39–48) report that single-frequency commands 1/2 may be undone by the control head, while the paired command 22 (0x16) keeps it in sync. The traffic is binary, even though the code comments call it NMEA.

### 2.2 LK8000 and other drivers

| Source | Useful findings | Limits |
| --- | --- | --- |
| [LK8000 8d78c63](https://github.com/LK8000/LK8000/blob/8d78c63f599b894dbff217e007db44be06ee67ee/Common/Source/Devices/devAR620x.cpp), `AR620xPutFreqStandby` / `AR620x_Convert_Answer` | Marks the requested standby as invalid until confirmed; command 22 validates both; strict CRC | No request/reply matching; static parser; resets on A5; lengths unchecked |
| Same, `SendCommand` | Counted writes | Two writes with 100 ms sleeps; returns true without checking a reply |
| Same, status decoder | Commands 3 volume, 4 squelch, 12 dual, 21 voltage, 64 TX/RX | Taken from code, not a protocol document |
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

Once the radio is identified, ask Becker for: the remote-control protocol for that hardware/software version, serial settings, status timing, paired vs single commands, framing and CRC, replies, channel test vectors, and permission to publish.

---

<a id="consequences"></a>

## 3. ➡️ What this means for the driver work

> [!TIP]
> Start the driver work (WP4) with mapping callers, state and display. Then fix one thing per step: counted send, state freshness, parsing, replies. Even a fresh paired readback leaves a race with someone tuning the radio by hand.

Test cases from the code: 118.000 → `00 00` (cut off by the send bug); 119.030 → `00 A5` (parser reset). These come from the code, not from Becker. After the framing is confirmed, test every split position and several frames per read.

---

<a id="manuals"></a>

## 4. 📖 Manuals and commercial adapters

| Reference | Finding |
| --- | --- |
| [Becker DV14307.03 Issue 06 §2.5.3.2 p.59](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=59) | J1: 2/9 TX2+/−, 3/10 RX2+/−, 21/22/23 D_GND |
| [First-design manual p.22](https://becker-avionics.com/wp-content/uploads/2017/08/AR6201_IO_SW3050149.pdf#page=22), [catalog](https://becker-avionics.com/manual-instructions-downloads/) | The older manual lists pin 2 as an input; Issue 06 as an output. J1-5 also differs by revision — use the manual that matches the radio |
| [LXNAV manual rev.21 pp.10–11](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10), [harness](https://www.navboys.com/lxnav-to-becker-ar6201-harness-for-radio-bridge-j1-plug.html) | AR6201 adapter on interface 2 with ground on pin 21; uses an RS-232 ↔ RS-422 adapter. Needs remote control/Tandem and minimum Becker software SCI1050S305 3.05 / SCI1051S305 1.49 |
| [SD-Link](https://sdlink.de/en/Manual/ar620x/), [installation manual v1.0](https://sdlink.de/wp-content/uploads/2026/02/SD-AR620X-E-NG_en_easyvfr_installation_v1.0.pdf) | Commercial Bluetooth adapter: needs a free J1, Tandem mode and AR6201 serial ≥ 3001 — product-specific, not general requirements |

---

⬆️ [Back to top](#top) · 🏠 [Overview](../../README.md)
