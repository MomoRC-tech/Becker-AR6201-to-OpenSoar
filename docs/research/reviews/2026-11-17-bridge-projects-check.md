# Bridge projects check: AlphaLima and Yuri against the project specification

S108, 2026-11-17 · WP6 · R02–R04/R06/R10/R12–R14 · follows the [first source review](2026-11-08-yuri-bridge.md) (S08) and the [adoption review](2026-11-16-yuri-adoption.md) (S103)

Which communication, set-up and OpenSoar-related specifications do the two bridge projects implement, where do they come from, what do their issues and pull requests report, and which other projects or sources help to make the bridge work with OpenSoar?

> [!NOTE]
> Source reading and web research only. Nothing was built, flashed or run. The owner's answers to the proposals of [section 7](#proposals) are recorded there, 2026-11-17.

---

## Result

- **Neither project specifies anything for OpenSoar or the instruments.** Both are generic three-UART TCP bridges. Their defaults (SSID `LK8000`, 192.168.4.1, ports 8880–8882, 19200 or 115200 baud, UDP 14550–14552) are author choices with no cited source. Our settings come from the installed system and device documents ([comparison](#comparison), [provenance](#provenance)).
- **Their issues hold no radio or OpenSoar defect.** The relevant ones (buffer overruns, soft-AP start-up, short passwords, Android dropping a Wi-Fi without internet) are already handled in our firmware or are phone settings ([issues](#issues)).
- **Three new findings matter for firmware step 4 and the bench tests:**
  1. OpenSoar reopens a TCP client port only after a socket error, never after a data timeout ([OpenSoar side](#opensoar)).
  2. In Arduino-ESP32 3.3.12 a TCP write to a client that does not read can block the whole forwarding loop for about 10 s per call. Keepalive starts only after 2 h idle ([ESP32 side](#esp32)).
  3. A "second client is refused" rule can therefore lock OpenSoar out of the radio after a Wi-Fi drop, while the stale first connection still holds the port.
- **Radio baud rate:** the Becker 6200 Series manual states none. The only value found is 9600 8N1 in the RadioDriver Becker controller code. Our configuration still has the upstream placeholder 115200 ([open item](../../project/open-items.md#software-and-acceptance)).
- **Other projects:** none bridges a Becker radio to OpenSoar. GS-Traffic2WIFI (MIT) and BtBridge/XCVario (GPL-2.0) are useful references for client handling ([other projects](#other-projects)).

---

<a id="comparison"></a>

## 1. Specification comparison

| Item | [AlphaLima bb42dc3](https://github.com/AlphaLima/ESP32-Serial-Bridge/tree/bb42dc3db926f9baf1f5faa76cc21bad1dd80cdf) | [Yuri 130f731](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) | This project ([bridge settings](../../../firmware/bridge/README.md#project-settings)) |
| --- | --- | --- | --- |
| Stated purpose | "Flight equipment devices like (Radio, Vario FLARM)" to a flight computer | Same README text; ESP8266 support added | Zander, FLARM, Becker AR6201 → OpenSoar |
| Wi-Fi start-up | Hotspot only (`MODE_AP`) | Network only (`MODE_STA`) or hotspot; waits for the network without limit | Network for 15 s, then hotspot (R12); both upstream modes kept |
| Address, SSID | 192.168.4.1, `LK8000` / `Flightcomputer` | 192.168.4.1 in hotspot mode; placeholder SSID | 10.10.10.10, `WiFi-BridgeTom`, as today's bridge ([ADR-05](../../design/architecture.md#adr-05)) |
| TCP ports | 8880 / 8881 / 8882 | 8880 / 8881 / 8882 | 10110 / 2000 / 4352, as today's OpenSoar entries |
| UDP | None | Broadcast on 14550 / 14551 / 14552 | Zander and FLARM only; none on the radio port |
| Baud | 19200 on all three | 115200 on all three | 19200 / 19200 candidates (S22, S23); radio 115200 placeholder |
| Pins | RX 21/16/15, TX 1/17/4 | Same as AlphaLima | [UART plan](../../hardware/wiring.md#overview): RX 32/26/16, TX 33/27/17 |
| Clients | 4 per port | 4 per port | 4 per port; radio: one owner (step 4) |
| Writers to a device | Every TCP client and Bluetooth | Every TCP client, UDP and Bluetooth | Radio: one TCP owner, no UDP or Bluetooth (build check) |
| Buffers | 1,024 bytes; writes not counted | 1,024 bytes, bounded reads; writes not counted | 1,024-byte buffers, 4,096-byte UART receive buffers; short writes counted |
| Debug output | On UART0 (`DEBUG_COM 0`) | On UART0 (`Serial`) | Off; status page instead |
| OTA | ArduinoOTA, no password | ArduinoOTA, no password | Password in both Wi-Fi modes (R13) |
| Status or settings page | None (the separate Pro version has a settings page) | None | Read-only page and JSON in its own task (R14) |
| Radio-specific handling | None | None | Single owner, counted writes, no protocol handling in the bridge (step 4) |
| OpenSoar-specific handling | None; comments mention the RoboRemo app | None | Ports and address match the existing OpenSoar device entries |
| License | MIT | MIT, AlphaLima copyright | MIT notice kept in `firmware/bridge/` |

---

<a id="provenance"></a>

## 2. Where the specifications come from

| Specification | AlphaLima and Yuri | This project |
| --- | --- | --- |
| Device serial settings | Not sourced; one default for every port | Zander handbook and NMEA supplement (S22), FLARM installation manual and FTD-012 data port ICD (S23); installed values still from Tom ([installed equipment](../../project/open-items.md#installed-equipment)) |
| Radio interface and protocol | None | Becker DV14307.03 Issue 06 J1 pinout (S10, S20); protocol only from driver code (S01, S11), Becker refused publication ([driver research](../becker-driver.md#manuals)) |
| Network endpoints | Author choice; UDP ports are the usual MAVLink ports (Yuri's drone use) | Tom's equipment report and today's OpenSoar entries (S21, [existing installation](../../project/existing-installation.md#configuration)) |
| OpenSoar behaviour | Not considered | OpenSoar 7.46.26 source (S01, S24); this review adds the TCP client and reopen logic of v7.47.27.t09 |
| Electrical levels | README picture; "search for TTL RS232 converter" | ESP32 datasheet and module data (S17, S35), divider calculation |
| Test evidence | Author statements in issues ("worked for days") | Bench records S97–S107; byte-exact loop test script |

---

<a id="issues"></a>

## 3. Issues and pull requests

Yuri has issues switched off; its pull requests are listed. AlphaLima has 12 open and 9 closed issues and no open pull request.

| Item | Topic | For this project |
| --- | --- | --- |
| [AlphaLima #1](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/1), [#3](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/3) | 8-bit buffer index overflow; link stops after hours because an index was not reset | Fixed in Yuri; our loop resets both indices. The one-hour run of step 5 covers it |
| [AlphaLima #2](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/2) | UART transmit overflow above 128 bytes on old cores | Not with Arduino-ESP32 3.3.x: the UART write waits for FIFO space |
| [AlphaLima #10](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/10) | Above 230 kBd the TCP write is too slow | Matches the write timing in [section 5](#esp32); our rates are far lower |
| [AlphaLima #11](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/11) | `softAPConfig` before the hotspot is up crashes; a 2 s delay fixes it | Our start-up waits 2 s before `softAPConfig`; hotspot passed on the bench (S106) |
| [AlphaLima #19](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/19) | Android drops mobile data or the bridge Wi-Fi because the Wi-Fi has no internet | Phone setting, not firmware. Also seen with XCSoar ([XCSoar #629](https://github.com/XCSoar/XCSoar/issues/629)): ports connect only after "stay connected without internet". New [open item](../../project/open-items.md#software-and-acceptance) |
| [AlphaLima #23](https://github.com/AlphaLima/ESP32-Serial-Bridge/issues/23) | Hotspot keeps the old name when the password has fewer than 8 characters | Our build requires at least 8 characters |
| AlphaLima #8, #13–#17, #20–#22 | Virtual COM ports, ESP8266, telnet mode, compile errors, cable replacement, MAVLink, inverted serial, ESP32-S2 | Not relevant |
| [Yuri PR #2](https://github.com/yuri-rage/ESP-Serial-Bridge/pull/2) (open) | Set the hostname before the Wi-Fi mode | Already so in our start-up |
| [Yuri PR #3](https://github.com/yuri-rage/ESP-Serial-Bridge/pull/3) (open) | Watchdog in the companion client sketch for host reset and long outage | Client sketch only; the same outage cases belong in step 5 |
| Yuri PR #4, #5 (closed) | ESP32-S3, PlatformIO, smaller transmit chunks, speed tests (fork takurx) | Not needed at our data rates |

No issue or pull request in either project concerns a radio, OpenSoar or XCSoar.

---

<a id="opensoar"></a>

## 4. OpenSoar side: TCP client behaviour

Checked in OpenSoar [v7.47.27.t09](https://github.com/OpenSoaring/OpenSoar/tree/d143b07e4ea217f70dea5c966d5291288398404c), the newest test build. `AR62xx.cpp` is identical to the reviewed 7.46.26 file (S01) and to 7.46.26.1.

| Behaviour | Source | Consequence |
| --- | --- | --- |
| Connect timeout 30 s; 1 s send timeout | `TCPClientPort.cpp`, `ConnectNext`, `OnSocketConnectSuccess` | A write to a stalled bridge fails after 1 s |
| No reopen on data timeout for TCP client ports | `Config.cpp`, `ShouldReopenOnTimeout`: `TCP_CLIENT` returns false | After a bridge power cycle without a socket error, a port can stay on "no data" |
| The AR62xx driver also has no timeout | `AR62xx.cpp`, `ar62xx_driver`: `NO_TIMEOUT` | Same for the radio port |
| Reopen after a socket error every 30 s, after repeated failures every 60 s | `Descriptor.cpp`, `AutoReopen` | OpenSoar reconnects at most every 30 s after the bridge closes the socket |
| Earlier XCSoar defect "TCP client does not reconnect" | [XCSoar #650](https://github.com/XCSoar/XCSoar/issues/650), fixed 2021 | Fixed before OpenSoar split off; recheck in step 5 |

Inference: in hotspot mode a bridge restart removes the phone's Wi-Fi, which usually closes its sockets. When phone and bridge share another network, the phone may keep a dead connection open. Step 5 should test the power cycle in both Wi-Fi modes.

Not checked: whether OpenSoar binds its sockets to the Wi-Fi network on Android. Code search hit the rate limit.

---

<a id="esp32"></a>

## 5. ESP32 side: write and keepalive behaviour

Checked in Arduino-ESP32 [3.3.12](https://github.com/espressif/arduino-esp32/blob/3.3.12/libraries/Network/src/NetworkClient.cpp) (the project toolchain, S104) and lwIP defaults in [esp-lwip](https://github.com/espressif/esp-lwip/blob/2.1.3-esp/src/include/lwip/priv/tcp_priv.h).

| Behaviour | Source | Consequence |
| --- | --- | --- |
| `write()` retries up to 10 times with a 1 s `select` each while the socket cannot take data | `NetworkClient::write`, `WIFI_CLIENT_MAX_WRITE_RETRY`, `WIFI_CLIENT_SELECT_TIMEOUT_US` | A client that stops reading blocks the single loop for about 10 s per write, for all three ports |
| Accepted clients get `SO_KEEPALIVE` | `NetworkServer::accept` | Keepalive is on |
| Keepalive idle time 7,200 s, then 9 probes 75 s apart | lwIP `TCP_KEEPIDLE_DEFAULT`, `TCP_KEEPINTVL_DEFAULT`, `TCP_KEEPCNT_DEFAULT` | A silent dead client is only noticed after more than 2 h |

Calculated, not measured: a vanished radio owner holds the port until a write to it fails or keepalive expires. If the radio sends nothing, that is over 2 h. During that time a "refuse the second client" rule refuses OpenSoar's reconnect.

---

<a id="other-projects"></a>

## 6. Other projects and sources

| Project | What it does | Useful for us |
| --- | --- | --- |
| [GS-Traffic2WIFI 7b1d41d](https://github.com/gs-avionik-gunnar/GS-Traffic2WIFI/tree/7b1d41d2a97f66d70715768199aa15746753cadf), MIT | One FLARM/NMEA serial port to TCP, GDL90 over UDP; network with hotspot fallback; web server in its own task | Drops a client after three short writes in a row ("NMEA clients reconnect on their own"); bounded serial work per loop pass. Same architecture choices as ours. Its writes still use the blocking core call |
| [BtBridge 62ebaf6](https://github.com/iltis42/BtBridge/tree/62ebaf63b6522b3dbd273aa22181e6ce32764842), GPL-2.0 (XCVario author) | Two serial ports, FLARM and radios, Wi-Fi TCP 8880/8881 or Bluetooth | Socket server in its own task; drops a client on send error. Only the first client may write to the device, later clients are read-only. Reference only (GPL) |
| [XCVario](https://github.com/iltis42/XCVario/tree/1c5ad83a9e0911b81bb0bd5a47243bea5746f6c2), GPL-2.0 | Glider vario with serial/Wi-Fi routing | Already listed in [bridge firmware](../bridge-firmware.md#alternatives) |
| [RadioDriver a794c4a](https://github.com/rbp28668/RadioDriver/tree/a794c4a994838b94d2b0bfa69ef76cda9a698957) | Teensy controller for a Becker AR620x | Opens the radio link at 9600 8N1 (`RadioDriver.ino` line 157; BeckerDriver `SerialLink.cpp` line 52). Code, not a Becker document |
| [LXNAV RS485/RS232 bridge manual](https://www.lx-avionik.de/wp/download/manuals/lx232BridgeManualEnglishVer0210Rev21.pdf#page=10) v2.10 Rev 21, p. 10, §2.4.2.4 | Commercial bridge to the AR6201 | "A remote control should be enabled on the Becker radio"; enable Tandem: MDE at switch-on, configuration screen, Tandem checkbox. No baud rate stated |
| [SD-Link AR620x](https://sdlink.de/en/Manual/ar620x/) and its [installation manual](https://sdlink.de/wp-content/uploads/2026/02/SD-AR620X-E-NG_en_easyvfr_installation_v1.0.pdf#page=2) v1.0, p. 2 | Commercial Bluetooth adapter | AR6201 needs serial number 3001 or higher and Tandem mode; J1 free. No baud rate stated |
| [Becker DV14307.03 Issue 06](https://www.becker-avionics.com/wp-content/uploads/00_Literature/6200_Series_IO_en.pdf#page=23) p. 23 | Manual for software 4.06 / 2.06 | Tandem mode synchronises two controllers; service mode uses RS-422 with a proprietary protocol. No baud rate in the text |
| [XCSoar #772](https://github.com/XCSoar/XCSoar/issues/772) | Several drivers on one hub port | Backlogged in 2026; confirms one TCP port per device, as in [ADR-01](../../design/architecture.md#adr-01) |
| [XCSoar #1092](https://github.com/XCSoar/XCSoar/issues/1092) | Becker driver | Last comments 2025: no Becker support in XCSoar while the protocol is unpublished |

Searched without result: an ESP32 or Wi-Fi bridge for a Becker radio with XCSoar, LK8000 or OpenSoar, and any public AR620x protocol document.

---

<a id="proposals"></a>

## 7. Proposals for the firmware plan

Owner's answers of 2026-11-17 in the last column; the [firmware plan](../../project/roadmap.md#firmware-plan) follows them.

| # | Proposal | Why | Owner |
| :-: | --- | --- | --- |
| 1 | Radio owner rule: a new connection takes over the radio port and closes the old one, instead of being refused | OpenSoar reconnects after a Wi-Fi drop while the stale connection may hold the port for hours ([section 5](#esp32)). Still one writer at a time. Alternative: refuse, but detect dead owners with a short keepalive on the radio socket | ✅ Refuse a second client; drop a dead owner by keepalive and, in hotspot mode, when its phone leaves (revised after the daily-use check below) |
| 2 | Write to clients without the core's 10 s retry: send only what the socket accepts and close a client that stays full, as GS-Traffic2WIFI does | Keeps navigation flowing when one client stalls (R02, K05) | ❌ Keep the core behaviour; step 5 tests a connected phone that stops reading; if it delays other data, the fix is required before aircraft use |
| 3 | Add to step 5: bridge power cycle in both Wi-Fi modes with OpenSoar connected; OpenSoar must return to data on all three ports without a manual reconnect | OpenSoar reopens TCP client ports only after a socket error ([section 4](#opensoar)) | ✅ Added to step 5 |
| 4 | Add a phone test with mobile data on: all three ports connect and stay connected | AlphaLima #19, XCSoar #629 | ✅ Added to step 5 |
| 5 | Set the radio baud rate from the installed radio; test 9600 8N1 first if nothing better is found | Only code evidence (RadioDriver); 115200 is an upstream placeholder | ✅ 9600 8N1 set now, until the installed setting is known |

### 7.1 Daily-use check

Rechecked with the owner the same day: does any answer create a problem for pilots in daily use?

| Answer | Daily-use effect | Result |
| --- | --- | --- |
| New connection takes over the radio | Two devices with the Becker entry (for example panel tablet and phone) kick each other off every 30 s, the OpenSoar reopen interval | Replaced by refuse plus dead-owner check |
| Keep blocking client writes | A connected phone that stops reading (screen off, app in background) can delay all ports, FLARM traffic included, by about 10 s per write | Step 5 tests this case; a delay makes the fix required before aircraft use |
| Power-cycle and mobile-data tests | Tests only | No effect |
| Radio at 9600 8N1 | A wrong value only stops frequency transfer; the radio's own controls stay (R05) | No effect |
| Tandem or remote mode on the radio | Required by LXNAV and SD-Link; LXNAV also sets the speaker volume source to "Both". Changes radio behaviour | Not changed; separate installation decision ([open item](../../project/open-items.md#software-and-acceptance)) |

---

## Limits

- Source reading only. Write timing and keepalive values are taken from the code, not measured.
- Android socket behaviour after Wi-Fi loss is inferred from reports, not tested.
- The Becker manual was read as extracted text; tables or figures with serial settings may have been missed.
- The radio baud rate, Tandem setting and remote-control availability of the installed radio stay open until the radio is identified.
