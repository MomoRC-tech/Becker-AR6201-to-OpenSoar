# Public OTA loader image (hotspot only)

🏠 [Overview](../../../README.md) › 📚 [Documentation](../../../docs/README.md) › 💾 Firmware › **Public OTA loader image (hotspot only)**

Ready-made image for the first USB flash of a bridge ESP32; later firmware goes over its hotspot. All settings (board, partition scheme, hotspot, OTA, flash address, checksums) are in [`manifest.json`](manifest.json). Source and build: [OTA loader](../README.md).

> [!WARNING]
> The passwords are public. Anyone in range who knows this repository can upload firmware while this image runs. Bench use only.

Tested on a bench board: hotspot opens at start-up and a phone joins with the public password (S100 in [sources](../../../docs/research/sources.md)). The `manifest.json` field `tested_on_board` is written by the build script and stays `false` until the next rebuild. Flashing and troubleshooting: [Flash over USB](../README.md#flash-usb).

---

<kbd>[🏠 Overview](../../../README.md)</kbd> <kbd>[📚 Documentation](../../../docs/README.md)</kbd>
