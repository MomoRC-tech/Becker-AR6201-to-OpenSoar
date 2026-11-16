# Public OTA loader image (hotspot only)

Ready-made image for the first USB flash of a bridge ESP32; later firmware goes over its hotspot. All settings (board, partition scheme, hotspot, OTA, flash address, checksums) are in [`manifest.json`](manifest.json). Source and build: [OTA loader](../README.md).

> [!WARNING]
> The passwords are public. Anyone in range who knows this repository can upload firmware while this image runs. Bench use only.

This exact image has not yet run on a board; the same sketch passed the hotspot test as private and test builds (S99 in [sources](../../../docs/research/sources.md)). Flashing and troubleshooting: [Flash over USB](../README.md#flash-over-usb-once).
