# Bridge firmware

The project's own Wi-Fi bridge firmware for Zander, FLARM and Becker, developed in the steps of the [firmware plan](../../docs/project/roadmap.md#firmware-plan) ([ADR-02](../../docs/design/architecture.md#adr-02)). It started from ESP-Serial-Bridge; origin and license below. Current state: step 3 done, running on the bench ESP32 (S105–S107); step 3a ([settings page](../../docs/project/roadmap.md#settings-page)) in progress: stored settings, their display and the settings page run on the bench ESP32 (S109–S111); step 3b, the [data monitor](#data-monitor), too (S112).

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

| Device | UART | RX | TX | TCP port | UDP port | Baud |
| --- | --- | --- | --- | --- | --- | --- |
| 📈 Zander | UART0 | GPIO32 | GPIO33 | 10110 | 14550 | 19200 |
| 📡 FLARM | UART1 | GPIO26 | GPIO27 | 2000 | 14551 | 19200 |
| 📻 Becker | UART2 | GPIO16 | GPIO17 | 4352 | — | 9600 |

- Pins: [UART plan](../../docs/hardware/wiring.md#overview). TCP ports and the hotspot address: [firmware plan](../../docs/project/roadmap.md#firmware-plan).
- Baud rates: Zander and FLARM are candidates from the device documents (S22, S23); the Becker value 9600 8N1 is the best lead found, from independent controller code, not a Becker document (S108). The installed settings are [open items](../../docs/project/open-items.md#installed-equipment).
- UDP ports: upstream values; [open item](../../docs/project/open-items.md#software-and-acceptance). A UDP port of 0 switches UDP off for that UART; the radio port uses it.
- UDP output is a Wi-Fi broadcast without acknowledgement: single datagrams can be lost (S105). Use TCP where every byte counts.

| Setting | Value |
| --- | --- |
| Wi-Fi start-up | `MODE_STA_AP`: join the network in `credentials.h` for 15 s, otherwise open hotspot `WiFi-BridgeTom` at 10.10.10.10. The upstream modes `MODE_STA` and `MODE_AP` stay selectable |
| Hostname | `wifi-bridgetom`, for mDNS and OTA, as the OTA loader |
| OTA | ArduinoOTA with the password from `credentials.h`, in both Wi-Fi modes |
| Log output | `DEBUG` off: UART0 serves the Zander, so nothing is printed |
| UART receive buffer | 4096 bytes per UART, so bytes are kept while the loop waits on a UART write |
| Build-time checks | The build stops if UDP or Bluetooth is pointed at the radio port, or if not exactly one Wi-Fi mode is selected |
| `BATTERY_SAVER` | Fixed so it lowers the transmit power; off |
| Version | `VERSION` in `config.h`, `0.<plan step>` until bench acceptance; shown on the status page as "Wi-Fi bridge 0.3a-ESP32" |
| Stored settings | `STORED_SETTINGS`: baud rates, frames, TCP and UDP ports, the network and the hotspot in this file are defaults; valid values saved on the [settings page](#settings-page) override them in `MODE_STA_AP`. An invalid stored value falls back to its default |
| Status page | `STATUS_PAGE`: read-only page on port 80 and `/api/status` as JSON, open to everyone on the bridge's Wi-Fi; see below |

<a id="status-page"></a>

## Status page

Open `http://<bridge address>/` (10.10.10.10 in hotspot mode) in a browser. [`status_page.h`](status_page.h) holds the page; the forwarding loop only adds to counters, and a low-priority task on the other CPU core serves it. The page refreshes its values every 2 s in place, so opened sections stay open. Light or dark follows the phone.

![Status page on a phone, all settings open](../../docs/images/status-page.png)

| Part | Shows |
| --- | --- |
| Header | Photo of the housing lid ([`device_image.h`](device_image.h), made by `tools/device_image.py`), firmware version, uptime, Wi-Fi: network address and the bridge's signal to the router, or hotspot address and the signal of each connected phone (no device addresses) |
| Device row | Live dot (green while data arrives, pulses on new bytes), state (receiving, quiet for how long, no data yet), baud rate and frame, TCP port, connected apps; receive errors and short writes in red when not zero |
| Details (collapsed) | Bytes from and to the device since start (TCP and UDP), UDP port, receive errors (buffer full, FIFO overflow, framing, parity, break), short writes (TCP writes that took fewer bytes than offered), UDP not sent (broadcasts the network stack refused); link to the [data monitor](#data-monitor) |
| All settings (collapsed) | Active baud, frame, TCP and UDP port per device; network, join time, hotspot name and address. Amber: saved on the bridge; red: saved value invalid, default in use. Passwords are never sent |

| Address | Content |
| --- | --- |
| `/` | The page, about 7.4 kB |
| `/device.webp` | Lid photo, 5.3 kB, cached for a day |
| `/api/status` | Live counters as JSON |
| `/api/settings` | Active settings, their defaults and source (`default`, `changed`, `invalid`) and the trial state as JSON |
| `/monitor`, `/api/monitor` | [Data monitor](#data-monitor) |

After any change to the status, settings or monitor page, take new screenshots from the running bridge and commit them with the change: `python tools/status_screenshot.py <bridge address>`. It masks the network name, shows hotspot mode and uses demo data for the monitor, since the repository is public.

<a id="settings-page"></a>

## Settings page

Open *Change settings* under *All settings*, or `http://<bridge address>/settings`. The browser asks for a login: user `bridge`, password = the OTA password from `credentials.h`. [`settings_page.h`](settings_page.h) holds the page and its handlers; [`settings.h`](settings.h) checks and stores the values.

![Settings page on a phone](../../docs/images/settings-page.png)

| What | How it works |
| --- | --- |
| Values | Per device: baud, frame, TCP port, UDP port (0 = off; always off on the radio port). Wi-Fi: network name and password, join time, hotspot name, password and address. Pins, radio port rules, hostname and OTA password stay fixed in the firmware |
| Checks | Every field is checked on the bridge; a refused save names each wrong field and changes nothing. Ports may not repeat; UDP not on 53, 67, 68, 3232, 5353 |
| Passwords | Never shown. Leave the field empty to keep the current one |
| Save and restart | Values equal to the default are not stored, so they show as default. The bridge restarts and uses the new values |
| Trial | A change to Wi-Fi, hotspot or ports runs on trial. Reconnect, open the settings page and press *Keep* within 10 minutes. Otherwise the bridge restores the values from before and restarts; after the 3rd start without *Keep* it restores them at once. Serial-only changes apply directly |
| Reset to defaults | Forgets all stored values, including a trial, and restarts with the firmware defaults |
| Storage | NVS namespaces `bridge` (active values, format version, trial marks) and `bridge-prev` (backup during a trial). Survives restarts and OTA updates; a full flash erase over USB clears it |

| Address | Content |
| --- | --- |
| `GET /settings` | The page (password) |
| `POST /api/settings` | Check and save form fields named by their keys (`b0`, `f0`, `t0`, `u0` ... `ns`, `np`, `nj`, `hs`, `hp`, `ha`), then restart (password) |
| `POST /api/keep` | Keep settings on trial (password) |
| `POST /api/reset` | Forget all stored values, then restart (password) |

Bench check: `python tools/settings_test.py <bridge address> [--revert]` runs the password, refusal, save, trial, *Keep*, reset and (with `--revert`, 10 minutes) automatic revert checks. The bridge must be in network mode, so its address stays the same across restarts.

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

There is no serial log. Find the address through mDNS (`wifi-bridgetom.local`) or the router; in hotspot mode it is 10.10.10.10.

## Loop check (step 2)

Fit jumper wires on the bare board (positions in [modules](../../docs/hardware/modules.md#esp32-pinout)): J2-8 IO33 → J2-10 IO26, J2-11 IO27 → J2-7 IO32, J3-11 IO17 → J3-12 IO16. Then, from a PC on the same network:

```bash
python tools/bridge_loop_test.py <bridge address> --radio-loop
```

It sends every byte value and a pseudo-random block over TCP and UDP and checks that the looped port returns exactly the same bytes, and that UDP to the radio port reaches nothing. Use the jumpers only while these pins are not soldered to the modules ([warning](../../docs/project/roadmap.md#firmware-plan)).
