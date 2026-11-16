"""Build the bridge firmware with arduino-cli.

    python tools/build_bridge.py

Builds firmware/bridge/ for the ESP32 Dev Module with the same toolchain and
partition scheme as the OTA loader, so the image can later be uploaded over
Wi-Fi. The build folder lives outside the sketch in build/bridge/ (ignored by
Git); a build folder inside the sketch is picked up as sketch source and forces
a full core rebuild every time. A build takes about 4-5 minutes, also when
only the sketch changed (S104 in docs/research/sources.md).

Needs firmware/bridge/credentials.h (ignored by Git; same format as the OTA
loader's). The image contains its passwords, so it stays in build/bridge/.

Prints the toolchain, the duration, the image size and any warning or error
from the sketch files. Nothing is uploaded.
"""

import re
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SKETCH = ROOT / "firmware" / "bridge"
BUILD = ROOT / "build" / "bridge"
FQBN = "esp32:esp32:esp32:PartitionScheme=min_spiffs"
SKETCH_FILES = ("bridge.ino", "config.h")


def core_version() -> str:
    out = subprocess.run(["arduino-cli", "core", "list"], capture_output=True, text=True, check=True).stdout
    match = re.search(r"^esp32:esp32\s+(\S+)", out, re.MULTILINE)
    return match.group(1) if match else "unknown"


def cli_version() -> str:
    out = subprocess.run(["arduino-cli", "version"], capture_output=True, text=True, check=True).stdout
    match = re.search(r"Version:\s*(\S+)", out)
    return match.group(1) if match else "unknown"


def main() -> int:
    if not (SKETCH / "credentials.h").exists():
        print("firmware/bridge/credentials.h is missing: copy credentials.example.h (or the OTA loader's credentials.h).")
        return 2
    cmd =["arduino-cli", "compile", "--fqbn", FQBN, "--warnings", "all", "--build-path", str(BUILD), str(SKETCH)]
    print(f"[bridge] arduino-cli {cli_version()}, esp32 core {core_version()}, {FQBN}", flush=True)
    print(f"[bridge] building in {BUILD.relative_to(ROOT).as_posix()}/ ...", flush=True)
    start = time.monotonic()
    result = subprocess.run(cmd, capture_output=True, text=True)
    seconds = time.monotonic() - start
    log = result.stdout + result.stderr
    sketch_lines = [
        line
        for line in log.splitlines()
        if any(name in line for name in SKETCH_FILES) and ("warning" in line or "error" in line)
    ]
    sizes = [line for line in log.splitlines() if line.startswith(("Sketch uses", "Global variables use"))]

    if result.returncode != 0:
        print(f"[bridge] FAILED after {seconds:.0f} s", flush=True)
        print("\n".join(sketch_lines) or log[-2000:], flush=True)
        return 1
    print(f"[bridge] ok in {seconds:.0f} s", flush=True)
    for line in sizes + sketch_lines:
        print(f"[bridge] {line}", flush=True)
    image = BUILD / "bridge.ino.bin"
    if image.exists():
        print(f"[bridge] image {image.relative_to(ROOT).as_posix()}, {image.stat().st_size} bytes", flush=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
