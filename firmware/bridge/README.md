# Bridge firmware

🏠 [Overview](../../README.md) › 📚 [Documentation](../../docs/README.md) › 💾 Firmware › **Bridge firmware**

ESP32 firmware for Zander, FLARM, Becker and supply voltage over TCP. The [roadmap](../../docs/project/roadmap.md#work-packages) tracks current acceptance. Bench evidence and firmware revisions are in the [source register](../../docs/research/sources.md#source-register).

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
| Wi-Fi start-up | `MODE_STA_AP`, no network built in (set it on the settings page; without one the hotspot opens at once), 15 s join time, hotspot on 2.4 GHz channel 11 (`HOTSPOT_CHANNEL`); behaviour: [user manual](../../docs/manual/user-manual.md#wifi-startup). The join can miss 15 s, at about −90 dBm (S142) and once at −45 dBm (S149); the bench bridge uses 30 s, set on the settings page. The upstream modes `MODE_STA` and `MODE_AP` stay selectable |
| Hostname | `wifi-bridgetom`, sent to the router in DHCP and used by OTA, as the OTA loader. No mDNS since 0.5.21 (owner decision): reach the bridge by its address |
| OTA | ArduinoOTA with the OTA password ([default logins](../../docs/manual/user-manual.md#logins)), in both Wi-Fi modes |
| Log output | `DEBUG` off: UART0 serves the Zander, so nothing is printed there. `WEB_LOG` shows the log lines and the ESP32 system messages on the [web log](#web-log); the build stops if both are on |
| UART receive buffer | 4096 bytes per UART, so bytes are kept while the loop waits on a UART write |
| Build-time checks | The build stops if UDP is switched on, if Bluetooth is pointed at the radio port, or if not exactly one Wi-Fi mode is selected |
| `BATTERY_SAVER` | Fixed so it lowers the transmit power; off |
| Version | `VERSION` in `config.h`, `0.<plan step>` until bench acceptance; shown on the status page |
| Stored settings | `STORED_SETTINGS`: baud rates, frames, TCP ports, the network and the hotspot in this file are defaults; valid values saved on the [settings page](#settings-page) override them in `MODE_STA_AP`. An invalid stored value falls back to its default |
| Status page | `STATUS_PAGE`: read-only page on port 80 and `/api/status` as JSON, open to everyone on the bridge's Wi-Fi; see below |
| Bridge dials the app | Step 4a: in hotspot mode the bridge connects to every phone on the hotspot (stations with a DHCP address) on the four app ports, where OpenSoar's *TCP port* entries listen; one attempt per phone and port every 3 s (a station that drops 3 attempts in a row without any answer, such as a laptop firewall, every 30 s), one connect in progress at a time, connects never wait, keepalive as on the server ports. A connected session takes an app slot and runs like an accepted app; Becker only while no app owns the radio. Zander, FLARM and Becker from the loop task, the voltage port from the status task. Network mode (0.5.23, increment 3): one phone per port, the address from the settings page (*Phone address*), else the last app accepted on a Zander, FLARM or Becker port, else the network gateway (the phone on its own hotspot). Since 0.5.25 the bridge learns only from new accepted connections in automatic mode. The status task saves the latest change to NVS `bridge-dial`, with at least 5 s between attempts. A rapid change can take effect before it is saved for the next restart. Dial state per port on the status page and in `/api/status` (0.5.26, increment 4). A station address outside the hotspot range (an early join, S211) is not dialed. A dialed session the app reset is closed and dialed again (0.5.26; before, it stayed open without data). Evidence S207, S209, S211, S219, S220, S222, S223 |
| Hotspot DHCP | No router option (default gateway) and no DNS server: a phone cannot send its internet traffic to the bridge, whatever it answers to Android's *no internet* prompt (S208, S209). Stations that join before the hotspot address is set are sent off once to rejoin with a 10.10.10.x address |
| Socket budget | lwIP has 16 sockets (fixed in the core libraries). At most 6 app connections in all ports together (`APP_SOCKET_BUDGET`), dial attempts included, so the web server, OTA and the self-check always find one (S211) |
| Self-healing | Network self-check: a loopback connection to port 80 every 30 s, a planned restart after 3 failures. Status-task watch: a planned restart after 60 s without its heartbeat (bench build: abort, so `/api/coredump` shows where it hung). Web server: connections without a request are dropped after 0.5 s, a reply gives up after 3 s without progress (S211), chunked replies included since 0.5.24 (S221) |
| Wi-Fi buffers | At most 16 dynamic receive and 16 transmit buffers (core: 32 each), set by starting Wi-Fi before the core does; heap dips to 0 came from these buffers (S199) |
| Supply voltage | `SUPPLY_SENSE`: 12 V input on GPIO34, OpenVario port 4353 and chart; pin, divider, port and warning levels under *Supply voltage* in `config.h`; see [below](#supply-voltage) |
| Chip temperature | `CHIP_TEMP`: the ESP32's own temperature on the status page and in the web log, no shutdown; warning levels under *Chip temperature* in `config.h`; see [below](#chip-temperature) |
| Lifetime record | `LIFETIME_RECORD`: starts, operating time and extremes kept in NVS through power cuts; see [below](#lifetime-record) |
| Saved log | `SAVED_LOG`: errors, warnings and each start kept in LittleFS through power cuts, page `/saved`; see [below](#saved-log) |

<a id="status-page"></a>

## Status page

Open `http://<bridge address>/` (10.10.10.10 in hotspot mode) in a browser. Android hotspot tips (S152): [user manual](../../docs/manual/user-manual.md#troubleshooting). TCP apps reach the bridge either way (S151). [`status_page.h`](status_page.h) holds the page; the forwarding loop only adds to counters, and a task at idle priority on the other CPU core serves it, so a slow request cannot starve the idle task and trip the task watchdog (S181). The page refreshes its values every 2 s in place, so opened sections stay open. Light or dark follows the phone.

![Status page on a phone, all settings open](../../docs/images/status-page.png)

| Part | Shows |
| --- | --- |
| Header | Photo of the housing lid ([`device_image.h`](device_image.h), made by `tools/device_image.py`), firmware version, uptime, [chip temperature](#chip-temperature), Wi-Fi: network address and the bridge's signal to the router, or hotspot address and the signal of each connected phone. Network mode: the phone the bridge dials and where its address came from (settings page, last phone seen, gateway) |
| Device row | Live dot (green while data arrives, pulses on new bytes), state (receiving, quiet for how long, no data yet), baud rate and frame, TCP port, connected apps; receive errors and short writes in red when not zero. With no app connected: whom the bridge dials and why it is not connected yet (*app not listening*, *no answer*, with the number of tries) |
| Details (collapsed) | Bytes from and to the device since start, receive errors (buffer full, FIFO overflow, framing, parity, break), short writes (data an app could not take: skipped in whole lines for a Zander or FLARM app, session closed on the radio port), *Bridge dials*: each phone and the state of its dial. A receive error on the radio line also closes the radio session, so no damaged reply reaches the app; link to the [data monitor](#data-monitor) |
| Supply voltage | Value of the last second, OpenVario port and connected apps (with no app: the dial state, as for a device), and the [chart](#supply-voltage) |
| Lifetime record (collapsed) | Operating time, starts, highest chip temperature and lowest supply voltage, with when they happened; see [below](#lifetime-record) |
| All settings (collapsed) | Active baud, frame and TCP port per device; network, join time, hotspot name and address; supply voltage settings. Amber: saved on the bridge; red: saved value invalid, default in use. Passwords are never sent |

| Address | Content |
| --- | --- |
| `/` | The page, about 7.4 kB |
| `/device.webp` | Lid photo, 5.3 kB, cached for a day |
| `/api/status` | Live counters, the supply voltage, the chip temperature and the lifetime record as JSON. Each port and `supply` carry `dial`: one entry per phone the bridge dials, `ip`, `state` (`connected`, `connecting`, `waiting`, `refused` = no app listening, `no answer`, `error`) and `attempts` (failed tries since the last session). Network mode: `wifi.dial_to` and `wifi.dial_source` |
| `/api/settings` | Active settings, their defaults and source (`default`, `changed`, `invalid`), the trial state and the device profiles with the presets that fit each port, as JSON |
| `/monitor`, `/api/monitor` | [Data monitor](#data-monitor) |
| `/log`, `/api/log` | [Web log](#web-log) |
| `/api/supply` | [Supply voltage](#supply-voltage) chart data |
| `/saved`, `/api/saved` | [Saved log](#saved-log) |

After any change to the status, settings, monitor or log page, take new screenshots from the running bridge and commit them with the change: `python tools/status_screenshot.py <bridge address>`. It masks the network name, shows hotspot mode and uses demo data for the monitor and the log, since the repository is public.

<a id="settings-page"></a>

## Settings page

Open *Change settings* under *All settings*, or `http://<bridge address>/settings`. No login (owner decision, 0.5.17): whoever is on the bridge's Wi-Fi can change settings; the hotspot password protects it ([logins](../../docs/manual/user-manual.md#logins)). [`settings_page.h`](settings_page.h) holds the page and its handlers; [`settings.h`](settings.h) checks and stores the values.

![Settings page on a phone](../../docs/images/settings-page.png)

| What | How it works |
| --- | --- |
| Detect | On either navigation input, press **Detect** for a one-time receive-only scan. Only that input pauses. The bridge tries its current settings, measures the incoming bit rate, then tries supported rates with 8N1. Three matching sentences identify Zander or FLARM. The result can also report both streams, generic NMEA, undecodable activity or silence. The bridge restores forwarding before showing the result, even if the browser leaves. **Use these settings** fills the form. **Save and restart** applies it. The Becker port stays manual. |
| Layout | A section menu at the top (Zander, FLARM, Becker, Network, Hotspot, Supply voltage, Chip temperature, Records, Restart) opens one section at a time; all start closed, `#Network` and the like open one directly. A section shows *n changed* or *check* (a refused field); the Save bar stays at the bottom of the screen. Under each TCP port the matching OpenSoar entry: *Device → Port TCP port*, the port and the driver |
| Values | Per device: device profile, baud, frame, TCP port. Network: network name and password, join time, phone address (network mode; empty = automatic). Hotspot: name, password and address. Supply voltage: sensor fitted, calibration factor, TCP port. Chip temperature: [offset](#chip-temperature). Pins, radio port rules, hostname and OTA password stay fixed in the firmware |
| Device profiles | Choose the instrument on a port: Zander ZS1, Classic FLARM, Becker AR6201 or Custom. A preset fills in baud and frame; changing either by hand selects Custom. Only presets that fit the port's wiring are offered and accepted: Zander and FLARM ports (RS-232, bridge receives only) take ZS1 or Classic FLARM, the Becker port (RS-422, both ways) takes AR6201. The page shows the port's wiring under the choice. Preset values are the candidates in `config.h`. Settings saved before profiles existed keep working: a preset that no longer matches its baud and frame shows as Custom. The TCP port stays separate
| Checks | Every field is checked on the bridge; a refused save names each wrong field and changes nothing. TCP ports, including the supply voltage port, may not repeat and may not be 80 |
| Passwords | Never shown. Leave the field empty to keep the current one |
| Save and restart | Values equal to the default show as default. The bridge restarts after a successful save. A flash write failure shows an error and skips the restart |
| Trial | Wi-Fi, hotspot and port changes run on trial (`SETTINGS_TRIAL_MS`, `SETTINGS_TRIAL_BOOTS`); behaviour: [user manual](../../docs/manual/user-manual.md#trial). Serial-only changes apply directly |
| Reset to defaults | Forgets all stored values, including a trial, and restarts with the firmware defaults |
| Restart bridge | Restarts with the stored settings and writes nothing; during a trial it counts as a start ([user manual](../../docs/manual/user-manual.md#trial)). The confirm dialog says how many restarts are left. The status page links to it under *All settings* |
| Storage | One NVS record in `bridge-state` holds the overrides, rollback values and trial state (format 3 since 0.5.23; format 2 is read once and carried over). Firmware 0.5.8 reads legacy `bridge` / `bridge-prev` settings and writes the new record on the first change or during trial recovery. The legacy keys stay until a reset, so firmware downgraded to 0.5.7 reads the old configuration. Settings survive restarts and OTA updates. A full USB flash erase clears them |

| Address | Content |
| --- | --- |
| `GET /settings` | The page |
| `POST /api/nav-scan`, `GET /api/nav-scan` | Start with form field `port=0` or `port=1`, cancel with `cancel=<scan id>`, or read progress with GET. Baud and frame describe the setting used to decode the stream. |
| `POST /api/settings` | Check and save form fields named by their keys (`p0`, `b0`, `f0`, `t0` ... `ns`, `np`, `nj`, `hs`, `hp`, `ha`, `vs`, `vc`, `vt`, `ct`), then restart |
| `POST /api/chip-temp/calibrate` | Form fields `room` (°C) and `cold=on`; stores the [chip temperature offset](#chip-temperature), answers `{"ok":true,"offset":…}`, then restarts. Refused with a `message` when the checks fail |
| `POST /api/keep` | Keep settings on trial |
| `POST /api/reset` | Forget all stored values, then restart |
| `POST /api/restart` | Answer `{"ok":true}`, then restart with the stored settings. For scripts: `curl -X POST http://<bridge address>/api/restart` |
| `POST /api/lifetime/reset` | Start a new [lifetime record](#lifetime-record) |
| `POST /api/saved/clear` | Delete the [saved log](#saved-log) |

Use `python tools/settings_test.py <bridge address> [--revert]` on a spare bridge in network mode. The script ends with a factory reset. For a configured bench bridge, compare its settings and origins before and after OTA, then let a temporary change on an unused TCP port revert and verify that the original settings and listener return.

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

Open *Log* at the bottom of the status page, or `http://<bridge address>/log`. No password, like the status page. [`weblog.h`](weblog.h) keeps the lines; [`log_page.h`](log_page.h) holds the page.

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
- **Collection:** errors and warnings also go to the [saved log](#saved-log), which survives a power cut. Always on, in RAM: the last 96 Error/Warning/Info lines and, separately, the last 48 Debug lines (about 16 kB). The buffers sit in `.noinit` RAM, so they survive a restart, crash or watchdog reset, not a power cut. A line is copied under a short lock; nothing blocks and nothing goes to a UART.
- **Counters as lines:** once per second the web task compares the port counters: apps connected, data arriving or quiet for 10 s, receive errors and short writes (at most one line per port every 5 s, with the kind of the last receive error). A refused radio app is logged at most every 30 s, with the number of further refusals. Memory and Wi-Fi signal as Debug every 60 s.
- **ESP32 system messages:** the ESP-IDF log output is redirected into the log as source System, so none goes out of UART0 on the Zander transmit pin. This core build compiles in only its error messages.
- **Data:** `GET /api/log?since=<sequence>&clock=<phone time in Unix ms>` returns the newer lines as JSON, oldest first, with `o`, the clock offset of the current start (Unix ms minus ms since start, 0 = not known): `q` sequence, `b` start number, `t` ms since that start, `c` clock time in Unix ms UTC (0 = not known), `l` level, `s` source, `m` message. `lost` is true when lines were overwritten before they were fetched.
- **Privacy:** saved logs contain the network name and app addresses. Do not commit them to this public repository unredacted.

Bench check: `python tools/log_test.py <bridge address>`, and after an OTA upload `--kept` to check that the lines from before the restart remain.

<a id="supply-voltage"></a>

## Supply voltage

The bridge measures its 12 V input through the [divider](../../docs/hardware/power.md#voltage-sense) on GPIO34 and sends it to OpenSoar ([ADR-07](../../docs/design/architecture.md#adr-07), [plan](../../docs/project/roadmap.md#supply-voltage)). [`supply.h`](supply.h) holds the measurement, the OpenVario port and the chart data. Everything runs in the status task; the forwarding loop is not touched. Value and chart are on the [status page](#status-page) (screenshot with demo data).

| Part | How it works |
| --- | --- |
| Reading | Every 100 ms the mean of 16 calibrated ADC samples (`analogReadMilliVolts`, 11 dB); × 11 for the divider, × the calibration factor. One value per second for OpenSoar and the status page |
| Plausible | 5–30 V. Outside it nothing is sent, the page shows *No reading*, and after 10 s the log warns once |
| OpenSoar | `$POV,V,12.62*hh` once per second on TCP 4353, up to 4 apps. In OpenSoar: add a device **OpenVario**, TCP client, the bridge address, port 4353. Data from OpenSoar (MacCready, ballast, bugs, polar) is read and dropped. An app that falls behind skips sentences; one that takes nothing for 60 s is closed |
| Chart | One point per minute: mean as a line, lowest to highest as a band, from the 100 ms values. The whole time the bridge is on, up to 12 hours (720 points) without scrolling; then the oldest drop out. Times on the axis are the phone's clock. A dashed line marks the warning level. The page reloads the chart every 30 s |
| Kept | The minute points sit in `.noinit` RAM like the web log: kept through a restart, crash or OTA update, cleared at power on. The minute running during a restart is lost |
| Web log | Source System: the first reading after start; a warning after 10 s below 11.5 V, and Info when it is back above 12.0 V |
| Settings | *Sensor fitted* on or off (default on; switch it off on a board without the divider, whose open pin reads noise); *Calibration factor* 0.900–1.100, with a helper: enter the multimeter reading at the XT30 and press *Set factor*; *TCP port*. A port change runs on trial, like the other ports |

`GET /api/supply` returns `points`, the full minutes oldest first as `[lowest, mean, highest]` in hundredths of a volt (`null` = no reading that minute), `current` for the minute still running, and `age_s`, the seconds since the newest full minute ended.

Bench check: `python tools/supply_test.py <bridge address>` checks the status block, the sentences and their checksum, that data from an app is dropped without closing it, and the chart data. With the sensor switched off: `--expect-off`. Accuracy against a multimeter: [bring-up step 13](../../docs/hardware/wiring.md#bring-up).

<a id="chip-temperature"></a>

## Chip temperature

The ESP32 reads its own internal temperature sensor ([plan](../../docs/project/roadmap.md#firmware-plan), step 3e). [`chiptemp.h`](chiptemp.h) runs in the status task; the forwarding loop is not touched.

| Part | How it works |
| --- | --- |
| What it measures | The ESP32 chip, not the air in the housing. On the classic ESP32 the sensor is undocumented, with no specified accuracy (S131); offsets are reported to differ from chip to chip. Read it as a trend |
| Reading | Once per second, shown as the mean of the last 10 s |
| Offset | Setting `ct` (*Chip temperature* › *Offset, °C*, 0.0 to 60.0, default 0 = none), subtracted from every shown, logged and recorded chip temperature. Stored like the other settings, no trial |
| Calibration | **Calibrate chip temperature** on the settings page, only after a power-on start (`esp_reset_reason()` = `ESP_RST_POWERON`) with a working first reading. Shows that reading and its time; the user confirms at least 30 min off in the same place and enters the room temperature (0 to 45 °C). Offset = first reading − room temperature, accepted from 0 to 60 °C, about ±5 °C. About +38 °C on the bench bridge (S187). Steps for users: [user manual](../../docs/manual/user-manual.md#chip-calibration) |
| Uncorrected | While the offset is 0, the status page marks each chip temperature *uncorrected* (link to the settings page), web-log lines add *(uncorrected)*, the Debug memory line *raw* |
| Status page | Header line *Chip temperature*, red when the raw reading is at or above the warning level; *no reading* when the chip's sensor does not work (it then returns 128 °F at every read) |
| Startup temperature | At entry to `setup()`, before Wi-Fi or the startup delay, take one sample. Then take one at each 10 s mark through 120 s, including while Wi-Fi connects. **Chip before Wi-Fi** shows the first reading and its time. Expand **Chip temperature during startup** for the full series. |
| `/api/status` | `chip_temp.celsius` is the live mean with the offset applied, `offset` the offset, `corrected` false while it is 0, `power_on` true after a power-on start, `warn_c` the warning level (raw). `chip_temp.boot_samples` grows to 13 entries, each with `ms` (actual milliseconds since firmware boot) and `celsius` (single reading with the offset applied, or `null` if unavailable). Entries stay in RAM until restart. |
| Web log | Source System, values with the offset applied: the first reading after start; a warning after 60 s with the raw reading at or above 100 °C; Info when it is back below 95 °C raw. Calibration: one Settings Info line. The Debug memory line every 60 s ends with the chip temperature. Without a working sensor, one Info line 30 s after start |
| No shutdown | The bridge keeps forwarding at any temperature. Losing FLARM and radio data in flight is worse than running warm |
| Warning level | 100 °C on the raw reading, before the offset, set by the owner to cover the sensor's offset: the assembled bridge read 72–80 °C open on a desk at 23 °C room temperature while the ESP32 stayed barely warm to the touch at 0.8 W input (S142) |

<a id="thermal-bench"></a>

### Two-hour FLARM load and cold-start reference

| Bench observation | Result on this ESP32 |
| --- | --- |
| Two-hour load, 21 Nov 2026 | Firmware 0.5.8, FLARM input on DE-9 pin 2 at 19,200 baud, 8N1. The PC sent 1,536 bytes/s (80% line rate) and read the stream over Wi-Fi. |
| Chip sensor | **81.2 °C** at the start, **93.8 °C** peak, **90.7 °C** at two hours. Median over the last 30 minutes was **89.9 °C**. |
| Supply and forwarding | 12.03–12.08 V, zero restarts, UART errors or TCP disconnects. The first frame arrived two bytes short. All subsequent bytes matched. |
| Cold start, 22 Nov 2026 | After an unpowered soak in a room at about **24 °C**, the first status reading was **59.4 °C at 2 s** of reported uptime. First readings at 10, 30 and 60 s were 60.3, 62.3 and 64.7 °C. |
| Cold start and 30 min load at 80 MHz, 23 Nov 2026 | Firmware 0.5.15, room about **25 °C**: first reading **62.8 °C at 0.12 s**, 72.8 °C after 2 min idle. Zander input at 80% line rate, radio loop and supply port: **84.8 °C** mean over the last 15 minutes (83.2–87.0 °C); at 240 MHz the same load read 88–95 °C (S187). |

![ESP32 sensor readings at ten-second intervals and five-minute rolling median during the two-hour FLARM load](../../docs/images/flarm-two-hour-chip-temperature.png)

**Rough estimate of peak chip temperature: about 58 °C.** The sensor peak was 34.4 °C above the next morning’s first reading (93.8 − 59.4). Adding that difference to the reported room temperature gives 58.4 °C. This assumes a constant sensor offset and a chip still at room temperature at the first reading. The 0.12 s reading of 23 Nov puts the offset at about **+38 °C**: about 47 °C real under load at 80 MHz, and the 100 °C warning near 62 °C real (S187). Any earlier self-heating would raise the estimate by the same amount under that assumption. The available records and published data do not quantify it.

The chart shows the sensor readings. Enclosure, regulator and clamp temperatures require the separate [housing heat check](../../docs/project/open-items.md#housing). [Bench evidence and thermal references](../../docs/research/sources.md#thermal-bench-evidence)

<a id="lifetime-record"></a>

## Lifetime record

A few values kept through power cuts ([plan](../../docs/project/roadmap.md#firmware-plan), step 3f). [`lifetime.h`](lifetime.h) runs in the status task; the forwarding loop never writes to the flash.

| Value | How it is counted |
| --- | --- |
| Operating time | Seconds since the record was started, counted from each ESP32 start |
| Starts | Every start, also restarts by the firmware |
| Highest chip temperature | Highest 1 min mean of the [chip temperature](#chip-temperature) with the offset applied; a new offset (calibration or by hand) corrects the stored value at the next start (`chipoff` in NVS keeps the offset it was recorded with), from 2 min after each start, with the operating time and, if the bridge knows the clock, the date. Single readings scatter by several degrees and run high just after a start (S134) |
| Lowest supply voltage | Lowest [supply voltage](#supply-voltage) held for 10 s, so a short dip or the decay at switch-off does not count; with operating time and date. Only with the sensor switched on |

| Part | How it works |
| --- | --- |
| Storage | One NVS blob, namespace `lifetime`, in the partition the settings use. *Reset to defaults* on the settings page does not touch it; a full flash erase over USB clears it |
| Flash writes | At every start; then every 10 min; at most once a minute while a new extreme is being set; before a restart by the firmware. A power cut loses at most the last 10 min of operating time |
| Status page | *Lifetime record*, collapsed, above *All settings* |
| Reset | Settings page, *Records*, *Reset lifetime record*. Starts a new record with this start as the first |
| Web log | Info at each start with the start number and operating time; a warning if the storage cannot be opened or a save fails |

> [!IMPORTANT]
> While the flash is written, the ESP32 pauses code from flash on both cores, and the UART receive interrupt of this core build does not run (S133). Keep flash writes rare and out of the forwarding loop. `python tools/records_test.py <bridge address> --write-load 120` runs the step 5 acceptance test while the record is written every 2 s.

Bench check: `python tools/records_test.py <bridge address>` checks the status block, the reset, and that the record survives a restart with one more start. It resets the record of that board. Survival of a power cut needs the USB cable pulled by hand.

<a id="saved-log"></a>

## Saved log

Errors, warnings and one line per start, kept through power cuts ([plan](../../docs/project/roadmap.md#firmware-plan), step 3g). Open *saved log* at the bottom of the status page or the log page, or `http://<bridge address>/saved`. [`savedlog.h`](savedlog.h) holds the file handling and the page.

![Saved log with demo data](../../docs/images/saved-page.png)

| Part | How it works |
| --- | --- |
| What is kept | Error and Warning lines of the [web log](#web-log), and one line per start with the firmware and the reset reason. Info and Debug stay in RAM only |
| Start numbers | From the [lifetime record](#lifetime-record), so they keep counting through power cuts; *Reset lifetime record* starts them again at 1 |
| Times | The clock time if the bridge knew it when the line was saved, otherwise seconds after that start (`+95.1s`) |
| Flash writes | The status task appends new lines at most every 10 s, in one write; opening the page does not add a write, so the last few seconds may follow on the next reload. Lines written just before a crash or restart are still in RAM at the next start and are saved then. If more lines arrive than the RAM log holds before they are saved, a warning line says so |
| Storage | LittleFS on the 128 kB `spiffs` partition, formatted on first use. File `/saved.log`; above 32 kB it becomes `/saved.old` and the older one is deleted, so both stay below about 64 kB. Survives OTA updates; a full flash erase over USB clears it |
| Page | Lines grouped by start, errors red, warnings amber; *Reload*, *Save* as a text file. Read-only, no password |
| Clear | Settings page, *Records*, *Clear saved log* |

`GET /api/saved` returns `# used <bytes> of <bytes>`, then one line each, oldest first, tab-separated: start number, clock time in Unix ms (0 = not known), ms since that start, level, source, message.

Bench check: `python tools/records_test.py <bridge address>` also checks the saved log: this start's line, a warning saved within 10 s, both kept through a restart, and clearing. `--write-load 120` adds a warning every 2 s during the acceptance test.

<a id="build"></a>

## Build

Same toolchain and partition scheme as the [OTA loader](../ota-loader/README.md#build): Arduino-ESP32 3.3.x, board **ESP32 Dev Module**, Partition Scheme **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**, CPU Frequency **160MHz (WiFi/BT)** (fewer heap dips than 80 MHz, S199; 240 MHz runs too hot, S187).

No credentials file is needed: every build carries the public [default logins](../../docs/manual/user-manual.md#logins) from [`config.h`](config.h) and no network. Set the network on the settings page after the first upload. For a bridge with its own OTA password, change `OTA_PASSWORD` in `config.h` locally before building and do not commit it.

```bash
python tools/build_bridge.py
```

The script builds in `build/bridge/` at the repository root, outside the sketch, and prints toolchain, duration, image size and any sketch warnings. A build takes between 1 and 5 minutes (S104, S105).

To test the hotspot fallback without switching off the network, build `python tools/build_bridge.py fallback-test` (in `build/bridge-fallback-test/`) and upload it. It joins a network that does not exist, so the hotspot opens after 15 s. Without an OTA upload it boots the previous image after 10 minutes, so it never stays installed by accident. The variant is selected by a generated `bridge_variant.h`, ignored by Git and removed after the build.

For unattended bench tests, build `python tools/build_bridge.py bench-open` (in `build/bridge-bench-open/`) and upload it once. [`bench.h`](bench.h) holds the variant.

| Part | Bench build |
| --- | --- |
| Marking | Version ends in `-BENCH`; every start logs a warning with the minutes left |
| Revert | After 8 hours of operating time, counted in NVS across restarts and power cuts, it boots the image that ran before the upload. An OTA upload stops the countdown |
| Revert after a bench build | A bench build installed over another bench build never reverts (the other slot would be a bench build too): upload a normal build. Normal builds clear the countdown record |
| Heap sampler | `GET /api/heapdiag` returns the lowest free byte heap in each of the last 300 seconds, sampled every millisecond (S199) |
| Core dump | `GET /api/coredump` returns the dump a panic or watchdog reset left in the coredump partition (404 if none). Decode with `esp-coredump info_corefile -t raw -c dump.bin build/bridge-bench-open/bridge.ino.elf` (needs `xtensa-esp32-elf-gdb` from the esp32 board package on the path); keep the ELF of the running build |
| OTA | Uploads without password, so a test script can install builds |

> [!WARNING]
> Anyone on the bridge's Wi-Fi can change its settings or upload firmware while the bench build runs. Bench only; check that the version no longer ends in `-BENCH` before the bridge goes near the aircraft.

Without a rebuild: save a network name that does not exist on the settings page. The change runs on [trial](../../docs/manual/user-manual.md#trial), so the old name comes back by itself, or set it back over the hotspot and press *Keep*. A second USB Wi-Fi adapter on the PC can join the hotspot while the first keeps the internet connection; give the second adapter a higher interface metric (for example 100), so Windows keeps internet traffic on the first.

## Upload over Wi-Fi

The board must run an image with an OTA receiver (OTA loader or this firmware). Arduino IDE: network port `wifi-bridgetom`. Command line, with `espota.py` from the esp32 board package:

```bash
python espota.py -i <bridge address> -a "<OTA password>" -f build/bridge/bridge.ino.bin
```

Shortcut: `python tools/ota_upload.py build/bridge/bridge.ino.bin [bridge address]` (default 10.10.10.10) keeps the OTA password in the Windows Credential Manager (`--store` to enter or replace it, `--forget` to delete it). Assistants prepare the image and the command; the owner starts the upload.

There is no serial log; see the [web log](#web-log). Find the address in the router (name `wifi-bridgetom`); in hotspot mode it is 10.10.10.10.

<a id="loop-check"></a>

## Loop check (step 2)

Fit jumper wires on the bare board (positions in [modules](../../docs/hardware/modules.md#esp32-pinout)): J2-8 IO33 → J2-10 IO26, J2-11 IO27 → J2-7 IO32, J3-11 IO17 → J3-12 IO16. Then, from a PC on the same network:

```bash
python tools/bridge_loop_test.py <bridge address> --radio-loop
```

It sends every byte value and a pseudo-random block over TCP and checks that the looped port returns exactly the same bytes, and that UDP sent to the former upstream UDP ports reaches no device. Use the jumpers only while these pins are not soldered to the modules ([warning](../../docs/project/roadmap.md#firmware-plan)).

Add `--radio-owner` for step 4. It checks that a second radio client is refused, navigation keeps flowing, and the radio port accepts the next client after a clean TCP close. The [wired checks](../../docs/project/roadmap.md#firmware-plan) cover the MAX490 path and recovery after Wi-Fi loss.

<a id="slow-app-check"></a>

## Slow-app check (step 5)

**Assembled bridge.** Disconnect the instruments. Use the [DE-9 breakout and RS-232 source wiring](../../docs/hardware/wiring.md#bus-test). Fit links 4–8 and 5–9 for `--radio-loop`. Run the PC checks with the navigation and radio clients on the phone disconnected. Wired mode needs pyserial and reads the active TCP ports, baud rates and serial formats from the bridge.

```bash
python tools/bridge_acceptance_test.py <bridge address> --feed COM4 --input flarm --radio-loop --baseline --seconds 15
python tools/bridge_acceptance_test.py <bridge address> --feed COM4 --input flarm --radio-loop --seconds 30
python tools/bridge_acceptance_test.py <bridge address> --feed COM4 --input flarm --radio-loop --seconds 120
```

| Run | What to check |
| --- | --- |
| Baseline | The selected input receives every byte in order at 80% of its line rate. The other input stays quiet. The radio returns each probe through the final echo. The default delay limit is 2 s. |
| Short slow-client run | The healthy reader and radio still pass while a second navigation client falls behind. The bridge reports skipped data for the slow client and keeps it connected. |
| Sustained slow-client run | The bridge log records a closure after 60 s without send progress. The healthy reader and radio continue. A client can stop reading before its TCP buffers fill, so elapsed test time alone cannot establish a stall. |
| One hour on all three ports | Connect two independent RS-232 sources, one to each navigation input, plus the DE-9 radio loop. Use the command below with the actual PC port names. The sources send different streams so swapped inputs fail. The radio carries random 1–64-byte bursts with every byte value, and nothing may follow the final echo. Every minute the script reads the bridge log: a restart fails the run, and the lowest free memory is printed. `--supply` also reads port 4353 and expects about one valid `$POV` sentence per second. |
| Radio robustness | `radio_robustness_test.py` with the DE-9 radio loop: a second client is refused, an owner that never reads is closed, the next owner receives no stale bytes (also after an owner that closes mid-echo), and the owner keeps its echo through repeated refused connections. The dead-owner takeover after a Wi-Fi cut stays a manual check. |

```bash
python tools/bridge_acceptance_test.py <bridge address> --zander-feed COM5 --flarm-feed COM4 --radio-loop --supply --seconds 3600
python tools/radio_robustness_test.py <bridge address>
```

**Phone checks.** Disconnect the PC test clients so OpenSoar can own the radio port. Feed recognizable Zander and FLARM sentences for the phone checks. Verify all three device connections and that fresh navigation data appears in OpenSoar. The Becker loop checks the transport only. Power-cycle the bridge in network mode and hotspot mode, then drop and restore phone Wi-Fi. Record which ports resume automatically, how long they take, and whether the phone needed a manual reconnect. Repeat with mobile data enabled. With a second phone, put its navigation client in the background or turn its screen off and check the first phone keeps receiving current data. Record the app version and Wi-Fi mode with each result. For the setup guide, capture the three stock OpenSoar device entries and a recognizable sentence from each navigation feed in the app monitor. Use synthetic bench data and crop private network details. [Step 5 acceptance criteria](../../docs/project/roadmap.md#firmware-plan)

**Bare ESP32 only.** The original `--radio-loop --baseline` and `--radio-loop` commands without serial-feed options use the step 2 GPIO jumpers (S120). Never fit GPIO jumpers once the converters are soldered.

---

<kbd>[🏠 Overview](../../README.md)</kbd> <kbd>[📚 Documentation](../../docs/README.md)</kbd>
