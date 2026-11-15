# OTA loader

Bench loader for the Wi-Fi bridge ESP32: flash it once over USB, then upload all later firmware over Wi-Fi. Design and rules: [ADR-05](../../docs/design/architecture.md#adr-05). Bench test: [bring-up step 3](../../docs/hardware/wiring.md#bring-up).

| Behaviour | Value |
| --- | --- |
| Start-up | Join the configured network for up to 15 s, otherwise open the hotspot |
| Hotspot | `WiFi-BridgeTom`, WPA2, ESP32 address 192.168.4.1 |
| OTA | ArduinoOTA, password-protected, hostname `wifi-bridgetom`; available in both modes until the next restart |
| Pins | No GPIO or instrument UART configured; boot log on UART0 TX (GPIO1) only |

## Set up

1. Arduino IDE with the Espressif **esp32** board package, 3.3.4 or later 3.x.
2. Copy `credentials.example.h` to `credentials.h` and fill in your network and two passwords of at least 8 characters each. `credentials.h` is ignored by Git; never commit it.
3. Board **ESP32 Dev Module**, Partition Scheme **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**.

## Flash over USB (once)

UBEC and all instruments unplugged ([power rules](../../docs/hardware/power.md#supply)).

```bash
esptool --port COM3 flash-id   # expect "Detected flash size: 4MB"
arduino-cli compile --upload -p COM3 \
  --fqbn esp32:esp32:esp32:PartitionScheme=min_spiffs firmware/ota-loader
```

The Arduino IDE upload button does the same. Serial monitor at 115200 baud shows the network result and IP address. Use your board's port (`/dev/ttyUSB0` on Linux). `esptool` ships with the esp32 board package in `tools/esptool_py`.

| Problem | Fix |
| --- | --- |
| No COM port; Device Manager shows "CP2102 USB to UART Bridge Controller" with an error | Install the Silicon Labs CP210x driver, then replug |
| `Wrong boot mode detected (0x13)` | Hold BOOT, tap EN, release BOOT, then retry |

## Upload over Wi-Fi (every later image)

In the Arduino IDE, pick the network port `wifi-bridgetom` and upload as usual; it asks for the OTA password. Without mDNS, use the IP address shown in the serial log (192.168.4.1 in hotspot mode):

```bash
python espota.py -i 192.168.4.1 -a "<OTA password>" -f firmware.bin
```

`espota.py` (`espota.exe` on Windows) ships with the esp32 board package in its `tools` folder. If authentication succeeds but the upload ends with `No response from device`, allow `espota.exe` through Windows Firewall: the bridge connects back to the PC for the data transfer. Every new image must be built with the same partition scheme and must contain an OTA receiver, otherwise the next update needs USB again.
