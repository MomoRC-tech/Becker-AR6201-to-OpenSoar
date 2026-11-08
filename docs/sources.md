# Sources and evidence

Reviewed for documentation cleanup on 2026-11-08. A source observation is not a runtime or hardware result. Keep source revision/date and scope with each claim.

## Evidence labels

| Label | Meaning |
| --- | --- |
| Rechecked source | Read again during this cleanup at the stated revision; consequences inferred from code are identified as such. |
| Prior review | Preserved investigation at S00; not independently repeated in this cleanup. |
| Reported | Project/user/vendor statement; no matching measurement supplied. |
| Assumption / proposed | Working hypothesis or design choice; not a verified fact or approval. |
| Measured | Reproducible test with setup, versions, input, expected/actual outcome and evidence. No radio/bridge measurements exist in this repository yet. |

## Source register

| ID | Source / baseline | Checked scope and limits |
| --- | --- | --- |
| S00 | [Repository before cleanup, fbed731](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/tree/fbed731cf5fa7e0f3106886182d14e2282b72a6b) | All three Markdown files read. Preserves prior source reviews and reported installation context; no executable artifacts, raw measurements or license file in that tree. The referenced prior chat informed continuity but is not a hardware source. |
| S01 | [OpenSoaring AR62xx.cpp, b2e745c](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Driver/AR62xx.cpp) | Rechecked source: send path, constructor/state, frame construction, receive/parser paths and registration. This pinned review baseline is not claimed to be the latest commit or installed build. |
| S02 | [Port.hpp, same revision](https://github.com/OpenSoaring/OpenSoar/blob/b2e745cfddfe0150f6ed7db467ac18c321b38d01/src/Device/Port/Port.hpp) | Rechecked FullWrite declarations: binary span and string_view overloads. Supports F02 together with S01. |
| S03 | [Becker manual catalog](https://becker-avionics.com/manual-instructions-downloads/) | Rechecked live catalog on 2026-11-08: distinct 6200-series and AR6201 first-hardware-design manuals. Does not identify which applies to the installed radio. |
| S04 | [OpenSoar 7.46.26 release notes](https://opensoar.de/releases/v7.46.26/) | Prior review associates S01 with this release and records Becker WIP. Page retrieval failed during this cleanup; release/date/WIP details were not independently revalidated. |
| S05 | [Technical investigation](technical-investigation.md); AlphaLima [basic](https://github.com/AlphaLima/ESP32-Serial-Bridge) / [Pro](https://github.com/AlphaLima/ESP32-Serial-Bridge-Pro) | Prior pinned August2111, AlphaLima basic/Pro reviews and Becker/LXNAV/SD-Link manual observations. Detailed source URLs and revisions remain there. Apart from S01/S02/S03, these checks were not repeated. |
| S06 | [Related projects](related-projects.md) | Prior pinned LK8000, Yuri-rage, RadioDriver, DroneBridge and XCVario reviews plus historical XCSoar issues/PRs. Candidate/licensing/build claims require rechecking before selection or copying. |
| S07 | [OpenAI workflow guidance](https://developers.openai.com/cookbook/examples/codex/iterating-development-workflows-with-codex) | Read on 2026-11-08 for repository work instructions. Our file layout and package gates are project conventions, not mandatory Codex infrastructure. |

The detailed manual pinouts, product-specific version cutoffs and license metadata in S05/S06 remain research leads. They do not establish compatibility, universal protocol rules, reuse permission for every file, or a wiring instruction for this installation.

## Rechecked facts

S08 — [Yuri/basic AlphaLima source evaluation](reviews/2026-11-08-yuri-bridge.md), rechecked 2026-11-08: pinned sketches, Yuri configuration/license/tree and default-branch histories. Yuri 130f731 has later functional fixes; AlphaLima bb42dc3 has newer documentation only since their shared 2020 revision. See the review for exact references, proposed increments and verification limits; no runtime evidence.

| ID | Observation | Evidence and limit |
| --- | --- | --- |
| F01 | S00 contains README and two docs, with no application code/tests. | Repository tree inspection; no claim about private or external firmware. |
| F02 | Send ignores msg_size and passes a char pointer to FullWrite; Port offers a string_view overload. | S01 lines 247–265; S02 lines 87–93. Inferred consequence: implicit string length stops at a zero byte instead of honoring the binary frame length. Not reproduced on a running build here. |
| F03 | Send assigns ACK locally after writing; success follows that value. No receive-side assignment/notification implements actual acknowledgment in this driver. | S01 Send and receive methods, especially lines 264–278. Establishes a source defect, not the radio's real acknowledgment format. |
| F04 | The command reads both stored frequencies; the constructor does not initialize them. Reception can update private state. | S01 Radio/radio_para, constructor, SetAR620xStation and AR620xConvertAnswer. Unknown/stale active state is a design risk; physical consequences remain untested. |
| F05 | Parser resets on A5, keeps static state, allows zero received CRC and suppresses conversion while sending. Frequency decoding lacks command-specific length checks. | S01 lines 459–558. Implementation observations; whether any exception is protocol-valid requires independent evidence. |
| F06 | Manufacturer supplies distinct manuals for different hardware generations. | S03. Supports identifying the installed device before choosing connector/settings guidance. |

For new evidence, add source revision or manual issue/page, retrieval date, exact scope, and whether it is observed, inferred or measured. Preserve conflicting evidence and resolve it explicitly. Do not turn historical reports or agreement between related codebases into independent protocol validation.
