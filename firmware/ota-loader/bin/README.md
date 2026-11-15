# Public OTA loader image (hotspot only)

Ready-made image for the first USB flash of a bridge ESP32. Afterwards, upload all firmware over the bridge's hotspot. Source, variants and build steps: [OTA loader](../README.md).

> [!NOTE]
> **Status:** built and checked for private data, not yet run on a board. The hotspot itself is not yet proven ([open item 15](../../../docs/project/open-items.md#bench-hardware)). Keep a USB cable at hand for the first use.

## What you must know

| Setting | Value |
| --- | --- |
| Board | ESP32 with 4 MB flash; built as **ESP32 Dev Module** |
| Partition scheme | **Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)**; every later image must use the same scheme and keep an OTA receiver |
| Built with | Arduino-ESP32 and arduino-cli versions in [`manifest.json`](manifest.json) |
| Wi-Fi | Hotspot only: `WiFi-BridgeTom`, WPA2, password `BridgeTom-setup`; ESP32 address 192.168.4.1 |
| OTA | ArduinoOTA, hostname `wifi-bridgetom`, password `BridgeTom-ota` |
| Pins | No GPIO or instrument UART configured; boot log on UART0 at 115200 baud |

> [!WARNING]
> The passwords are public. Anyone in range who knows this repository can upload firmware while this image runs. Bench use only: install a private build before the bridge goes near the aircraft.

## Files

| File | Use | Flash address |
| --- | --- | --- |
| `ota-loader-hotspot-only.merged.bin` | First flash over USB: bootloader, partition table and app in one file | `0x0` |
| `ota-loader-hotspot-only.bin` | App only, for an OTA upload onto a board that already runs a loader | — (OTA) |
| `manifest.json` | Board package version and SHA-256 of both images | — |

## Flash and check

UBEC and all instruments unplugged ([power rules](../../../docs/hardware/power.md#supply)).

```bash
esptool --port COM3 flash-id    # expect "Detected flash size: 4MB"
esptool --port COM3 write-flash 0x0 firmware/ota-loader/bin/ota-loader-hotspot-only.merged.bin
```

The serial log should show `Build: hotspot only` and `Hotspot "WiFi-BridgeTom" open, IP 192.168.4.1`. Then join the hotspot and upload over Wi-Fi:

```bash
python espota.py -i 192.168.4.1 -a "BridgeTom-ota" -f firmware.bin
```

Driver, boot-mode and firewall problems: see [Flash over USB](../README.md#flash-over-usb-once).
