"""Screenshot the bridge status and settings pages for the documentation.

Reads both pages, the image and the settings JSON from a running bridge,
masks private details, and renders them at phone width (390 px, 2x) in the
light theme: docs/images/status-page.png ("All settings" open) and
docs/images/settings-page.png. The settings page login uses the OTA
password from firmware/bridge/credentials.h.

    python tools/status_screenshot.py <bridge address>

Masked before rendering (the repository is public):
- network name and its default -> "your-network", as in credentials.example.h
- Wi-Fi line -> hotspot mode at the configured hotspot address with one phone

Needs Google Chrome or Microsoft Edge. Rerun after every change to the
status page and commit the new image (AGENTS.md).
"""

import argparse
import base64
import json
import re
import shutil
import subprocess
import tempfile
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
IMAGES = ROOT / "docs" / "images"
BROWSERS = [
    r"C:\Program Files\Google\Chrome\Application\chrome.exe",
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    "google-chrome",
    "chromium",
]


def get(host: str, path: str, auth: str = "") -> bytes:
    req = urllib.request.Request(f"http://{host}{path}")
    if auth:
        req.add_header("Authorization", "Basic " + base64.b64encode(f"bridge:{auth}".encode()).decode())
    with urllib.request.urlopen(req, timeout=10) as r:
        return r.read()


def ota_password() -> str:
    text = (ROOT / "firmware" / "bridge" / "credentials.h").read_text()
    return re.search(r'#define\s+OTA_PASSWORD\s+"([^"]*)"', text).group(1)


def shoot(page: str, image: bytes, target: Path, height: int) -> None:
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        (tmp / "device.webp").write_bytes(image)
        (tmp / "page.html").write_text(page, encoding="utf-8")
        (tmp / "frame.html").write_text(
            '<html><body style="margin:0"><iframe src="page.html" '
            f'style="width:390px;height:{height}px;border:0;display:block"></iframe></body></html>',
            encoding="utf-8")
        target.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run([browser(), "--headless=new", "--disable-gpu", "--hide-scrollbars",
                        "--allow-file-access-from-files", "--force-device-scale-factor=2",
                        "--blink-settings=preferredColorScheme=1",  # light theme
                        f"--window-size=390,{height}", f"--screenshot={target}",
                        (tmp / "frame.html").as_uri()],
                       check=True, capture_output=True)
    print(f"{target.relative_to(ROOT)} written; check it before committing")


def browser() -> str:
    for b in BROWSERS:
        if Path(b).exists() or shutil.which(b):
            return b
    raise SystemExit("Chrome or Edge not found")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host", help="bridge address")
    ap.add_argument("--height", type=int, default=1010, help="page height in CSS pixels")
    args = ap.parse_args()

    page = get(args.host, "/").decode("utf-8")
    image = get(args.host, "/device.webp")
    status = json.loads(get(args.host, "/api/status"))
    settings = json.loads(get(args.host, "/api/settings"))

    hotspot = "10.10.10.10"
    for item in settings["items"]:
        if item["key"] == "ns":
            item["value"] = item["default"] = "your-network"
        if item["key"] == "ha":
            hotspot = item["value"]
    status["wifi"] = {"mode": "hotspot", "ip": hotspot, "stations": [-52]}

    mock = ("<script>const _st=" + json.dumps(status) + ",_se=" + json.dumps(settings) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(u.includes('settings')?_se:_st)});</script>")
    page = page.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    page = page.replace('<section class="set"><details>', '<section class="set"><details open>', 1)

    shoot(page, image, IMAGES / "status-page.png", args.height)

    form = get(args.host, "/settings", ota_password()).decode("utf-8")
    mock = ("<script>const _se=" + json.dumps(settings) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(_se)});</script>")
    form = form.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    shoot(form, image, IMAGES / "settings-page.png", 1640)


if __name__ == "__main__":
    main()
