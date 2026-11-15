# Public OTA loader image (hotspot only)

Ready-made image for the first USB flash of a bridge ESP32; later firmware goes over its hotspot. All settings (board, partition scheme, hotspot, OTA, flash address, checksums) are in [`manifest.json`](manifest.json). Source and build: [OTA loader](../README.md).

> [!WARNING]
> The passwords are public. Anyone in range who knows this repository can upload firmware while this image runs. Bench use only.

Not yet run on a board; the hotspot is not yet proven ([open item 15](../../../docs/project/open-items.md#bench-hardware)). Flashing and troubleshooting: [Flash over USB](../README.md#flash-over-usb-once).
