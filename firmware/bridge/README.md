# Bridge firmware

Wi-Fi bridge firmware for Zander, FLARM and Becker, based on ESP-Serial-Bridge ([ADR-02](../../docs/design/architecture.md#adr-02)). The standalone ESP32 runs step 4 firmware with single-client radio ownership (S114) and the [web log](#web-log) (S116). See the [firmware plan](../../docs/project/roadmap.md#firmware-plan) for the remaining wired checks.

## Origin and license

| Item | Value |
| --- | --- |
| Upstream | [yuri-rage/ESP-Serial-Bridge](https://github.com/yuri-rage/ESP-Serial-Bridge) |
| Pinned commit | [`130f731416d43c89bce5626ef989b3e65918c9ce`](https://github.com/yuri-rage/ESP-Serial-Bridge/tree/130f731416d43c89bce5626ef989b3e65918c9ce) |
| Imported files | `ESP-Serial-Bridge.ino` as [`bridge.ino`](bridge.ino) (arduino-cli needs the sketch named after its folder), [`config.h`](config.h), [`LICENSE`](LICENSE) |
| License | MIT, see [`LICENSE`](LICENSE); keep the notice with every copy |
| Line endings | CRLF as at the import; [`.gitattributes`](.gitattributes) keeps them, so `git diff 56d1b23 -- firmware/bridge` shows every project change. Each change is marked `// Project:` |

<a id="project-settings"></a>

## Project settings

Set in `config.h`; everything else is upstream.

| Device | UART | RX | TX | TCP port | Baud |
| --- | --- | --- | --- | --- | --- |
| 📈 Zander | UART0 | GPIO32 | GPIO33 | 10110 | 19200 |
| 📡 FLARM | UART1 | GPIO26 | GPIO27 | 2000 | 19200 |
| 📻 Becker | UART2 | GPIO16 | GPIO17 | 4352 | 9600 |

- Pins: [UART plan](../../docs/hardware/wiring.md#overview). TCP ports and the hotspot address: [firmware plan](../../docs/project/roadmap.md#firmware-plan).
- Baud rates: Zander and FLARM are candidates from the device documents (S22, S23); the Becker value 9600 8N1 is the best lead found, from independent controller code, not a Becker document (S108). The installed settings are [open items](../../docs/project/open-items.md#installed-equipment).
- UDP is not implemented: the upstream UDP broadcast is removed, OpenSoar connects by TCP. Reason: broadcast over Wi-Fi has no acknowledgement and lost single datagrams on the bench, with and without load (S112, [issue #1](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/1)). The build stops if `PROTOCOL_UDP` is defined.

| Setting | Value |
| --- | --- |
| Wi-Fi start-up | `MODE_STA_AP`: join the network in `credentials.h` for 15 s, otherwise open hotspot `WiFi-BridgeTom` at 10.10.10.10. The upstream modes `MODE_STA` and `MODE_AP` stay selectable |
| Hostname | `wifi-bridgetom`, for mDNS and OTA, as the OTA loader |
| OTA | ArduinoOTA with the password from `credentials.h`, in both Wi-Fi modes |
| Log output | `DEBUG` off: UART0 serves the Zander, so nothing is printed there. `WEB_LOG` shows the log lines and the ESP32 system messages on the [web log](#web-log); the build stops if both are on |
| UART receive buffer | 4096 bytes per UART, so bytes are kept while the loop waits on a UART write |
| Build-time checks | The build stops if UDP is switched on, if Bluetooth is pointed at the radio port, or if not exactly one Wi-Fi mode is selected |
| `BATTERY_SAVER` | Fixed so it lowers the transmit power; off |
| Version | `VERSION` in `config.h`, `0.<plan step>` until bench acceptance; shown on the status page |
| Stored settings | `STORED_SETTINGS`: baud rates, frames, TCP ports, the network and the hotspot in this file are defaults; valid values saved on the [settings page](#settings-page) override them in `MODE_STA_AP`. An invalid stored value falls back to its default |
| Status page | `STATUS_PAGE`: read-only page on port 80 and `/api/status` as JSON, open to everyone on the bridge's Wi-Fi; see below |

<a id="status-page"></a>

## Status page

Open `http://<bridge address>/` (10.10.10.10 in hotspot mode) in a browser. [`status_page.h`](status_page.h) holds the page; the forwarding loop only adds to counters, and a low-priority task on the other CPU core serves it. The page refreshes its values every 2 s in place, so opened sections stay open. Light or dark follows the phone.

![Status page on a phone, all settings open](../../docs/images/status-page.png)

| Part | Shows |
| --- | --- |
| Header | Photo of the housing lid ([`device_image.h`](device_image.h), made by `tools/device_image.py`), firmware version, uptime, Wi-Fi: network address and the bridge's signal to the router, or hotspot address and the signal of each connected phone (no device addresses) |
| Device row | Live dot (green while data arrives, pulses on new bytes), state (receiving, quiet for how long, no data yet), baud rate and frame, TCP port, connected apps; receive errors and short writes in red when not zero |
| Details (collapsed) | Bytes from and to the device since start, receive errors (buffer full, FIFO overflow, framing, parity, break), short writes (TCP writes that took fewer bytes than offered); link to the [data monitor](#data-monitor) |
| All settings (collapsed) | Active baud, frame and TCP port per device; network, join time, hotspot name and address. Amber: saved on the bridge; red: saved value invalid, default in use. Passwords are never sent |

| Address | Content |
| --- | --- |
| `/` | The page, about 7.4 kB |
| `/device.webp` | Lid photo, 5.3 kB, cached for a day |
| `/api/status` | Live counters as JSON |
| `/api/settings` | Active settings, their defaults and source (`default`, `changed`, `invalid`), the trial state and the device profiles with the presets that fit each port, as JSON |
| `/monitor`, `/api/monitor` | [Data monitor](#data-monitor) |
| `/log`, `/api/log` | [Web log](#web-log) |

After any change to the status, settings, monitor or log page, take new screenshots from the running bridge and commit them with the change: `python tools/status_screenshot.py <bridge address>`. It masks the network name, shows hotspot mode and uses demo data for the monitor and the log, since the repository is public.

<a id="settings-page"></a>

## Settings page

Open *Change settings* under *All settings*, or `http://<bridge address>/settings`. The browser asks for a login: user `bridge`, password = the OTA password from `credentials.h`. [`settings_page.h`](settings_page.h) holds the page and its handlers; [`settings.h`](settings.h) checks and stores the values.

![Settings page on a phone](../../docs/images/settings-page.png)

| What | How it works |
| --- | --- |
| Values | Per device: device profile, baud, frame, TCP port. Wi-Fi: network name and password, join time, hotspot name, password and address. Pins, radio port rules, hostname and OTA password stay fixed in the firmware |
| Device profiles | Choose the instrument on a port: Zander ZS1, Classic FLARM, Becker AR6201 or Custom. A preset fills in baud and frame; changing either by hand selects Custom. Only presets that fit the port's wiring are offered and accepted: Zander and FLARM ports (RS-232, bridge receives only) take ZS1 or Classic FLARM, the Becker port (RS-422, both ways) takes AR6201. The page shows the port's wiring under the choice. Preset values are the candidates in `config.h`. Settings saved before profiles existed keep working: a preset that no longer matches its baud and frame shows as Custom. The TCP port stays separate
| Checks | Every field is checked on the bridge; a refused save names each wrong field and changes nothing. TCP ports may not repeat and may not be 80 |
| Passwords | Never shown. Leave the field empty to keep the current one |
| Save and restart | Values equal to the default are not stored, so they show as default. The bridge restarts and uses the new values |
| Trial | A change to Wi-Fi, hotspot or ports runs on trial. Reconnect, open the settings page and press *Keep* within 10 minutes. Otherwise the bridge restores the values from before and restarts; after the 3rd start without *Keep* it restores them at once. Serial-only changes apply directly |
| Reset to defaults | Forgets all stored values, including a trial, and restarts with the firmware defaults |
| Storage | NVS namespaces `bridge` (active values, format version, trial marks) and `bridge-prev` (backup during a trial). Survives restarts and OTA updates; a full flash erase over USB clears it |

| Address | Content |
| --- | --- |
| `GET /settings` | The page (password) |
| `POST /api/settings` | Check and save form fields named by their keys (`p0`, `b0`, `f0`, `t0` ... `ns`, `np`, `nj`, `hs`, `hp`, `ha`), then restart (password) |
| `POST /api/keep` | Keep settings on trial (password) |
| `POST /api/reset` | Forget all stored values, then restart (password) |

Bench check: `python tools/settings_test.py <bridge address> [--revert]` runs the password, refusal, save, device profile, trial, *Keep*, reset and (with `--revert`, 10 minutes) automatic revert checks. The bridge must be in network mode, so its address stays the same across restarts.

<a id="data-monitor"></a>

## Data monitor

Open *Data monitor* at the bottom of the status page, *Show data* in a device's details, or `http://<bridge address>/monitor`. No password, like the status page: any client on the bridge's Wi-Fi can already read the TCP ports. [`monitor.h`](monitor.h) holds the page and the buffers.

![Data monitor with demo data](../../docs/images/monitor-page.png)

- **What it shows:** the latest data of one device, both directions in time order: *from* = sent by the device to the apps, *to* = sent by an app to the device. Times are seconds since the bridge started.
- **Text or hex:** text joins NMEA into lines (Zander and FLARM start in text); hex shows every byte (the Becker radio starts in hex). Non-printable bytes show as a dot in text.
- **Pause, Clear, Download:** Download saves the shown lines as a text file, for example for radio protocol captures.
- **Collection:** 64 chunks of up to 64 bytes per device, in RAM only. The forwarding loop copies bytes in only while a monitor page has asked for data in the last 10 s; otherwise nothing is collected. If more arrives between two polls than the buffer holds, the page says so.
- **Data:** `GET /api/monitor?port=<0-2>&since=<sequence>` returns the newer chunks as hex in JSON.

Bench check: `python tools/monitor_test.py <bridge address>` with the step 2 jumpers.

<a id="web-log"></a>

## Web log

Open *Log* at the bottom of the status page, or `http://<bridge address>/log`. No password, like the status page. [`weblog.h`](weblog.h) keeps the lines; [`log_page.h`](log_page.h) holds the page. Message kinds per source: [plan](../../docs/project/roadmap.md#web-log).

![Web log with demo data, Events view](../../docs/images/log-page.png)

| Control | What it does |
| --- | --- |
| **Important / Events / All** | Errors and warnings / adds Info, what the bridge does (default) / adds Debug details |
| Source buttons | Show or hide Wi-Fi, Ports, Radio, Settings, System. The browser remembers view and sources |
| **Pause**, **Clear** | Freeze the view (lines keep arriving) / empty the view; the bridge keeps its lines |
| **Copy** | Puts the shown lines on the clipboard, with a header: firmware, save time, view. Works over plain `http`, where the browser's clipboard API is blocked, through the older copy command |
| **Save** | Downloads the same text as `bridge-log-<date>-<time>.txt`. Columns: date and time with milliseconds (`-` when not known), seconds since start, start number, level, source, message |

- **Lines:** clock time, level letter (E, W, I, D), source, message. A date line marks each new day. On a PC, hover over the time for the seconds since the start.
- **Clock:** the bridge has no clock of its own. The log page sends the phone's time with every request; the first one sets the ESP32 clock, and the lines of this start logged before it get their time from it ("Clock set from a phone"). A phone more than 2 s off corrects it ("Clock corrected"). The order of the lines is the order the bridge wrote them; times always run in that order: each start has one clock offset, a line takes its time and its number together under the lock, and a correction re-dates every line of that start, also on an open page. The clock keeps running through a restart, so lines after it get their time at once; after a power cut the time is unknown until a phone opens the log. Unknown times show as seconds after the start, for example `+95.1s`.
- **Restarts:** grey lines are from before a restart; a separator marks it, and the first line after it names the reset reason (power on, restart by the firmware, crash, watchdog, brownout).
- **Collection:** always on, in RAM: the last 96 Error/Warning/Info lines and, separately, the last 48 Debug lines (about 16 kB). The buffers sit in `.noinit` RAM, so they survive a restart, crash or watchdog reset, not a power cut. A line is copied under a short lock; nothing blocks and nothing goes to a UART.
- **Counters as lines:** once per second the web task compares the port counters: apps connected, data arriving or quiet for 10 s, receive errors and short writes (at most one line per port every 5 s, with the kind of the last receive error). A refused radio app is logged at most every 30 s, with the number of further refusals. Memory and Wi-Fi signal as Debug every 60 s.
- **ESP32 system messages:** the ESP-IDF log output is redirected into the log as source System, so none goes out of UART0 on the Zander transmit pin. This core build compiles in only its error messages.
- **Data:** `GET /api/log?since=<sequence>&clock=<phone time in Unix ms>` returns the newer lines as JSON, oldest first, with `o`, the clock offset of the current start (Unix ms minus ms since start, 0 = not known): `q` sequence, `b` start number, `t` ms since that start, `c` clock time in Unix ms UTC (0 = not known), `l` level, `s` source, `m` message. `lost` is true when lines were overwritten before they were fetched.
- **Privacy:** saved logs contain the network name and app addresses. Do not commit them to this public repository unredacted.

Bench check: `python tools/log_test.py <bridge address>`, and after an OTA upload `--kept` to check that the lines from before the restart remain.

## Build

Same toolchain and partition scheme as the [OTA loader](../ota-loader/README.md#build): Arduino-ESP32 3.3.x, board **ESP32 Dev Module**, Partition Scheme **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**.

Copy [`credentials.example.h`](credentials.example.h) to `credentials.h` and fill in the network and two passwords of at least 8 characters each; the OTA loader's `credentials.h` has the same format. `credentials.h` is ignored by Git; never commit it. The built image contains the passwords and stays in `build/bridge/`.

```bash
python tools/build_bridge.py
```

The script builds in `build/bridge/` at the repository root, outside the sketch, and prints toolchain, duration, image size and any sketch warnings. A build takes between 1 and 5 minutes (S104, S105).

To test the hotspot fallback without switching off the network, build `python tools/build_bridge.py fallback-test` (in `build/bridge-fallback-test/`) and upload it. It joins a network that does not exist, so the hotspot opens after 15 s. Without an OTA upload it boots the previous image after 10 minutes, so it never stays installed by accident. The variant is selected by a generated `bridge_variant.h`, ignored by Git and removed after the build.

## Upload over Wi-Fi

The board must run an image with an OTA receiver (OTA loader or this firmware). Arduino IDE: network port `wifi-bridgetom`. Command line, with `espota.py` from the esp32 board package:

```bash
python espota.py -i <bridge address> -a "<OTA password>" -f build/bridge/bridge.ino.bin
```

There is no serial log; see the [web log](#web-log). Find the address through mDNS (`wifi-bridgetom.local`) or the router; in hotspot mode it is 10.10.10.10.

## Loop check (step 2)

Fit jumper wires on the bare board (positions in [modules](../../docs/hardware/modules.md#esp32-pinout)): J2-8 IO33 → J2-10 IO26, J2-11 IO27 → J2-7 IO32, J3-11 IO17 → J3-12 IO16. Then, from a PC on the same network:

```bash
python tools/bridge_loop_test.py <bridge address> --radio-loop
```

It sends every byte value and a pseudo-random block over TCP and checks that the looped port returns exactly the same bytes, and that UDP sent to the former upstream UDP ports reaches no device. Use the jumpers only while these pins are not soldered to the modules ([warning](../../docs/project/roadmap.md#firmware-plan)).

Add `--radio-owner` for step 4. It checks that a second radio client is refused, navigation keeps flowing, and the radio port accepts the next client after a clean TCP close. The [wired checks](../../docs/project/roadmap.md#firmware-plan) cover the MAX490 path and recovery after Wi-Fi loss.

<a id="slow-app-check"></a>

## Slow-app check (step 5)

With the same jumpers:

```bash
python tools/bridge_acceptance_test.py <bridge address> --radio-loop --baseline
python tools/bridge_acceptance_test.py <bridge address> --radio-loop
```

It streams NMEA-like lines into the Zander port at 80 % of the line rate and reads them back on the FLARM port, with a second app there that never reads, like a phone with OpenSoar in the background. It passes when the bridge keeps taking data, the normal app gets every byte unchanged, and neither the FLARM data nor the radio loop is delayed by more than 2 s. `--baseline` leaves out the slow app. Firmware 0.4.4 fails it: one app that stops reading blocks all ports (S119).
