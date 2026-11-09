# Existing installation: claim verification

2026-11-09 · WP1 / bounded WP2 research · [source register](../sources.md)

Tom's report describes working Zander/FLARM reception through the existing ESP32, **without Becker integration**. Equipment, settings and operation remain **reported**, not measured. Manuals and pinned source establish capabilities, not the installed configuration.

## Claim assessment

| Claim | Assessment / evidence |
| --- | --- |
| Becker AR6201-(022), ETSO-2C37e/2C38e | Consistent with Becker DV14307.03 Issue 06 (February 2021), pp.16/36: 8.33/25 kHz, 6 W variant, article 0636.339-910. Model remains reported; type plate and software needed. Certification text does not identify the serial settings. S10/S21 |
| ESP32 model unknown | Exact board/firmware still unknown. Prior photos support a WROOM-family board only (S13). Do not substitute the separately selected new-build board. |
| Classic FLARM IGC, series 6 | Plausible family identification. Manufacturer installation manual v221 (2016), p.1 covers F6/F7/F8/F9, hardware version 3. Confirm label prefix, selected socket and firmware. S23 |
| Zander ZS1; only two Zanders | ZS1 is reported. “Only two” is too ambiguous to establish compatibility: the supplied handbook distinguishes ZS1, ZS1G and ZS1R and references SR940; software and options also matter. S22 pp.2/48/50 |
| No RS-232 converter because no DE9 connector | Incorrect reasoning. RS-232 is an electrical interface; RJ45 and DE9 are connector forms. Classic FLARM explicitly uses RS-232 over RJ45. ESP GPIO is not RS-232; Espressif specifies 3.6 V maximum GPIO tolerance. Conversion may be concealed or discrete; its presence/absence is unverified. S23/S19 |
| Instrument TX is tapped into ESP32 | Receive-only forwarding is plausible, but the actual level conversion, signal reference and return wiring are unknown. The word “processed” does not establish whether firmware parses or merely forwards bytes. No direct instrument-to-GPIO wiring established. |
| Both have RJ45; FLARM signal/interface is uniform across navigation systems | Partly. Classic Power/Data is RJ45/RS-232; shared sentence definitions do not make every port, setting or parser identical. The Zander crop shows eight contacts with different data assignments; actual connector/view remains unverified. See [connector evidence](../connections.md#zander-zs1). |
| Zander emits all listed flight/navigation values; OpenSoar understands them | The 3037 supplement lists them; the reviewed OpenSoar driver consumes only a subset. See table below. Installed ZS1 and OpenSoar versions unknown. |
| Latest Zander has FLARM input; unused here | 3037 supplement confirms a FLARM function exists; it does not establish which hardware revision first had that socket. No FLARM-to-Zander connection and an inactive input are reported; menu setting unverified. |
| 12 V bus, Hobbywing 3A UBEC 2–6S; instruments separately powered | Consistent with manufacturer part 86010010: 5.5–26 V input, jumper-selectable **5 or 6 V** output. Installed setting/output and board supply connection unmeasured. Separate instrument supplies are reported, not evidence of galvanic isolation. S14/S21 |

## Reported TCP configuration

| Feed | Address | Driver | Status |
| --- | --- | --- | --- |
| Zander | `10.10.10.10:10110` | `Zander / SDI` | Reported existing input |
| FLARM | `10.10.10.10:2000` | `FLARM` | Reported existing input |
| Becker | `10.10.10.10:4352` | `AR62xx` | Reported entry/intended path; no working radio integration |

These are bridge configuration values, not instrument protocol standards or proof of listening services. No network probe performed. The pasted `http://` links do not establish HTTP; the report describes TCP byte streams. Separate endpoints do not prove bidirectional UART wiring. S24 confirms the navigation driver names; Becker source limitations remain in S01/S12.

## Zander output versus OpenSoar consumption

**Rechecked source:** ZS1 program 3037 supplement, pp.1–2; OpenSoar 7.46.26 source `b2e745cfddfe0150f6ed7db467ac18c321b38d01` (S24). “Read” means the source stores a value, not measured display/priority behavior.

| Sentence / values | Reviewed OpenSoar behavior |
| --- | --- |
| `GPRMC`: position, ground speed, track, time/date | Generic `NMEAParser::RMC` reads these; reached through `DeviceDescriptor::ParseNMEA` |
| `GPRMB`: destination position/distance/bearing | No RMB handler in reviewed generic parser or Zander driver |
| `PZAN1`: barometric altitude, instrument serial number | `PZAN1` reads altitude, skips serial number; treats altitude as QNH based on a source comment reporting email confirmation, not independent manual proof |
| `PZAN2`: TAS, vario | `PZAN2` reads both; km/h conversion, vario `(value - 10000) / 100` m/s; stores total-energy vario |
| `PZAN3`: wind component, wind direction/speed, validity | `PZAN3` skips the first three fields (component/sign/validity); reads direction/speed and checks validity, with old/new-format handling |
| `PZAN4`: MacCready, set wind component, wing loading, best L/D | `PZAN4` reads **MacCready only** |
| `PZAN5`: vario/speed-to-fly mode, destination, distance/units, compass | `PZAN5` reads **mode only** (`VA`/`SF`) |

The supplement explicitly changes `PZAN3` from older versions. It specifies 19,200 baud when the ZS1 FLARM function is enabled, otherwise 4,800. **Conditional inference:** 4,800 is a candidate only if this firmware/configuration matches Tom's unit. No FLARM cable alone does not prove the menu setting or baud rate. Baud rates belong to serial links, not TCP ports.

## FLARM traffic and serial scope

**Rechecked source:** FTD-012 revision history 7.22, §5 and §8.2: RS-232/8N1, configurable baud/output; some Classic extension ports emit only `PFLAU`. `PFLAA` traffic requires at least 19,200 baud. It carries relative position/height, ID, track, speed and climb; fields/targets can be absent. It describes nearby traffic, not only opposing traffic. `PFLAU` is the primary alarm source.

Distance/absolute target altitude may be derived from relative data. A Classic target ID is not a callsign. Newer `PFLAM` identity messaging is unavailable on Classic (§8.20, pp.55–56). OpenSoar can resolve names through local/FLARMnet data (`TrafficDatabases::FindNameById`, S24). Displayed names therefore do not prove callsign transmission by this unit.

## Document applicability and retained material

| Document | Verified version / useful scope | Retention |
| --- | --- | --- |
| [ZS1 handbook](../reference/ZS1_Deutsch.pdf) | `ZS1_Man_d`, D3029, 2004-09-16; pp.46/48 PC/NMEA cable. Older than the supplement; not a complete 3037 reference | Unmodified supplied PDF, 51 pages |
| [ZS1 NMEA supplement](../reference/ZS1_NMEA_3037.pdf) | Program 3037, 2010-10-14; 2 pages, German/English | Unmodified supplied PDF; preferred supplied sentence reference for 3037 |
| [FTD-116 FAMP](https://www.flarm.com/en/integration/flarm-famp-public-protocol/) | Supplied v1.02, 2026-09-07; still listed by manufacturer at review. Radio-over-air protocol, not ESP serial protocol | Link/hash only; outside bridge implementation scope |
| [FTD-012 Data Port ICD](https://www.flarm.com/wp-content/uploads/2025/09/FTD-012-Data-Port-Interface-Control-Document-ICD-7.22.pdf) | History p.2 confirms 7.22 dated 2025-09-24 despite stale page headers “1.1 / 2022-06-06”; manufacturer catalog lists this download | Serial reference; S23 corrects S09's earlier revision uncertainty |
| [Classic FLARM installation manual](https://www.flarm.com/wp-content/uploads/2024/04/FLARM_InstallationManual_D.pdf) | v221, 2016-12-18; still manufacturer's listed F6/F7/F8/F9 manual. Upload date 2024 is not the manual date | Model-family connector reference; actual match pending |

No authoritative newer Zander release obtained: manufacturer website timed out and searches did not establish current documentation. Neither “old” nor a newer filename proves incompatibility/compatibility with the installed unit. The cropped Zander diagram has no visible revision/page; retained as a private source observation (S25), not a matched installation drawing. PDF copyrights remain with their authors; no repository license assigned.

Supplied file SHA-256:

| File | SHA-256 |
| --- | --- |
| `ZS1_Deutsch.pdf` | `0d103b402edea8e5c3798d9979a7df01493b3f0a8c6553fe19912b59b07c2a80` |
| `ZS1_NMEA_3037.pdf` | `036188492bb829e5c49123323c2939aaddc1ab9f387e08dd4e6f8ba072bd1cb2` |
| `FTD-116-FLARM-FAMP-Public-Protocol-v1.02.pdf` | `561c59a848e0219600265f38fe6fabc7a4f48fda027c18468e9936e345fb2d73` |

## Verification

Supplied PDFs extracted; Zander NMEA p.1/handbook p.46 and FLARM connector p.6/revision-history p.2 rendered and visually checked; both supplied diagrams inspected. Manufacturer sources and pinned parser functions checked; both saved PDFs hash-matched; 62 local link/anchor checks and whitespace checks passed; documentation consistency reviewed. Rendered pages readable despite Poppler font-substitution warnings. No application builds/tests, live traffic, voltage/continuity measurements, firmware changes, radio commands or aircraft work. WP1/WP2 remain open; [remaining evidence](../roadmap.md#wp1--existing-installation-first-priority).
