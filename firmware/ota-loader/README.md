# OTA loader

Bench loader for the Wi-Fi bridge ESP32: flash it once over USB, then upload all later firmware over Wi-Fi. Design and rules: [ADR-05](../../docs/design/architecture.md#adr-05). Bench test: [bring-up step 3](../../docs/hardware/wiring.md#bring-up).

| Behaviour | Value |
| --- | --- |
| Start-up | Join the configured network for up to 15 s, otherwise open the hotspot |
| Hotspot | `WiFi-BridgeTom`, WPA2, ESP32 address 192.168.4.1 |
| OTA | ArduinoOTA, password-protected, hostname `wifi-bridgetom`; available in both modes until the next restart |
| Pins | No GPIO or instrument UART configured; boot log on UART0 TX (GPIO1) only |

## Variants

| Variant | Compiler flag | Wi-Fi | Passwords | Where |
| --- | --- | --- | --- | --- |
| Public | `-DHOTSPOT_ONLY` | Hotspot only | Public defaults, see [`bin/manifest.json`](bin/manifest.json) | [`bin/`](bin/README.md), committed |
| Private | — | Network, then hotspot | `credentials.h` | `build/private/`, local only |
| Fallback test | `-DFALLBACK_TEST` | Joins a network that does not exist, so the hotspot fallback runs | `credentials.h` | `build/fallback-test/`, local only |

> [!WARNING]
> Anyone with this repository knows the public passwords and can upload firmware while the public build runs. Use it on the bench only; install a private build before the bridge goes near the aircraft.

The fallback test build boots the previously installed image after 5 minutes without an OTA upload, so it never stays installed by accident.

## Build

Arduino-ESP32 board package 3.3.4 or later 3.x, board **ESP32 Dev Module**, Partition Scheme **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**. For the private and test builds, copy `credentials.example.h` to `credentials.h` and fill in your network and two passwords of at least 8 characters each. `credentials.h` is ignored by Git; never commit it.

```bash
python tools/build_ota_loader.py                  # public, plus private if credentials.h exists
python tools/build_ota_loader.py fallback-test    # one variant
```

The script builds one variant at a time and prints its duration. The first build after a board package update compiles the core and takes about 4 minutes; repeat builds take about 20 s. It copies the public images and a [`manifest.json`](bin/manifest.json) with board package version and SHA-256 to `bin/`; commit the three files together.

## Flash over USB (once)

UBEC and all instruments unplugged ([power rules](../../docs/hardware/power.md#supply)).

```bash
esptool --port COM3 flash-id   # expect "Detected flash size: 4MB"
esptool --port COM3 write-flash 0x0 firmware/ota-loader/bin/ota-loader-hotspot-only.merged.bin
```

For a private build, flash `build/private/ota-loader.ino.merged.bin` instead, or use the Arduino IDE upload button. Serial monitor at 115200 baud shows the network result and IP address. Use your board's port (`/dev/ttyUSB0` on Linux). `esptool` ships with the esp32 board package in `tools/esptool_py`.

| Problem | Fix |
| --- | --- |
| No COM port; Device Manager shows "CP2102 USB to UART Bridge Controller" with an error | Install the Silicon Labs CP210x driver, then replug |
| `Wrong boot mode detected (0x13)` | Hold BOOT, tap EN, release BOOT, then retry |

## Upload over Wi-Fi (every later image)

In the Arduino IDE, pick the network port `wifi-bridgetom` and upload as usual; it asks for the OTA password. Without mDNS, use the IP address shown in the serial log (192.168.4.1 in hotspot mode):

```bash
python espota.py -i 192.168.4.1 -a "<OTA password>" -f firmware.bin
```

`espota.py` ships with the esp32 board package in its `tools` folder. If authentication succeeds but the upload ends with `No response from device`, Windows Firewall blocks the data transfer: the bridge connects back to the PC. Allow the program that runs `espota` (Python, or `espota.exe`; its path changes with each board package update). Every new image must be built with the same partition scheme and must contain an OTA receiver, otherwise the next update needs USB again.
