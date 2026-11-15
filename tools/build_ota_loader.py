"""Build the OTA loader variants with arduino-cli.

    python tools/build_ota_loader.py                   # public + private (if credentials.h exists)
    python tools/build_ota_loader.py public            # hotspot-only build only
    python tools/build_ota_loader.py private fallback-test

Variants:
    public         -DHOTSPOT_ONLY, no credentials.h. Copied to
                   firmware/ota-loader/bin/ and committed.
    private        Network + hotspot, passwords from credentials.h. Local only.
    fallback-test  Temporary hotspot fallback test build. Local only.

Build folders stay in firmware/ota-loader/build/ (ignored by Git), so a
repeat build only recompiles the sketch. Each variant prints its own status
line and duration.
"""

import hashlib
import json
import re
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SKETCH = ROOT / "firmware" / "ota-loader"
BUILD = SKETCH / "build"
PUBLIC_BIN = SKETCH / "bin"
FQBN = "esp32:esp32:esp32:PartitionScheme=min_spiffs"

VARIANTS = {
    "public": "HOTSPOT_ONLY",
    "private": None,
    "fallback-test": "FALLBACK_TEST",
}


def core_version() -> str:
    out = subprocess.run(["arduino-cli", "core", "list"], capture_output=True, text=True, check=True).stdout
    match = re.search(r"^esp32:esp32\s+(\S+)", out, re.MULTILINE)
    return match.group(1) if match else "unknown"


def cli_version() -> str:
    out = subprocess.run(["arduino-cli", "version"], capture_output=True, text=True, check=True).stdout
    match = re.search(r"Version:\s*(\S+)", out)
    return match.group(1) if match else "unknown"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def build(name: str) -> bool:
    flag = VARIANTS[name]
    out_dir = BUILD / name
    cmd = ["arduino-cli", "compile", "--fqbn", FQBN, "--warnings", "all", "--build-path", str(out_dir)]
    if flag:
        cmd += ["--build-property", f"compiler.cpp.extra_flags=-D{flag}"]
    cmd.append(str(SKETCH))

    print(f"[{name}] building ...", flush=True)
    start = time.monotonic()
    result = subprocess.run(cmd, capture_output=True, text=True)
    seconds = time.monotonic() - start
    log = result.stdout + result.stderr
    sketch_warnings = [line for line in log.splitlines() if "ota-loader.ino" in line and ("warning" in line or "error" in line)]
    size = next((line for line in log.splitlines() if line.startswith("Sketch uses")), "")

    if result.returncode != 0:
        print(f"[{name}] FAILED after {seconds:.0f} s", flush=True)
        print("\n".join(sketch_warnings) or log[-2000:], flush=True)
        return False
    print(f"[{name}] ok in {seconds:.0f} s. {size}", flush=True)
    for line in sketch_warnings:
        print(f"[{name}] {line}", flush=True)

    if name == "public":
        PUBLIC_BIN.mkdir(exist_ok=True)
        files = {
            "ota-loader-hotspot-only.bin": out_dir / "ota-loader.ino.bin",
            "ota-loader-hotspot-only.merged.bin": out_dir / "ota-loader.ino.merged.bin",
        }
        manifest = {"fqbn": FQBN, "esp32_core": core_version(), "arduino_cli": cli_version(), "files": {}}
        for target, source in files.items():
            shutil.copyfile(source, PUBLIC_BIN / target)
            manifest["files"][target] = {"bytes": source.stat().st_size, "sha256": sha256(source)}
        (PUBLIC_BIN / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
        print(f"[{name}] copied to {PUBLIC_BIN.relative_to(ROOT).as_posix()}/", flush=True)
    return True


def main(argv: list[str]) -> int:
    names = argv or ["public"] + (["private"] if (SKETCH / "credentials.h").exists() else [])
    unknown = [n for n in names if n not in VARIANTS]
    if unknown:
        print(f"Unknown variant(s): {', '.join(unknown)}. Choose from {', '.join(VARIANTS)}.")
        return 2
    if any(n != "public" for n in names) and not (SKETCH / "credentials.h").exists():
        print("credentials.h is missing: copy credentials.example.h first.")
        return 2
    ok = all([build(n) for n in names])
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
