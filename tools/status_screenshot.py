"""Screenshot the bridge status page for the documentation.

Reads the page, its image and both JSON endpoints from a running bridge,
masks private details, and renders it at phone width (390 px, 2x) with
"All settings" open, in the light theme, into docs/images/status-page.png.

    python tools/status_screenshot.py <bridge address>

Masked before rendering (the repository is public):
- network name -> "your-network", as in credentials.example.h
- Wi-Fi line -> hotspot mode at the configured hotspot address with one phone

Needs Google Chrome or Microsoft Edge. Rerun after every change to the
status page and commit the new image (AGENTS.md).
"""

import argparse
import json
import shutil
import subprocess
import tempfile
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TARGET = ROOT / "docs" / "images" / "status-page.png"
BROWSERS = [
    r"C:\Program Files\Google\Chrome\Application\chrome.exe",
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    "google-chrome",
    "chromium",
]


def get(host: str, path: str) -> bytes:
    with urllib.request.urlopen(f"http://{host}{path}", timeout=10) as r:
        return r.read()


def browser() -> str:
    for b in BROWSERS:
        if Path(b).exists() or shutil.which(b):
            return b
    raise SystemExit("Chrome or Edge not found")


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host", help="bridge address")
    ap.add_argument("--height", type=int, default=960, help="page height in CSS pixels")
    args = ap.parse_args()

    page = get(args.host, "/").decode("utf-8")
    image = get(args.host, "/device.webp")
    status = json.loads(get(args.host, "/api/status"))
    settings = json.loads(get(args.host, "/api/settings"))

    hotspot = "10.10.10.10"
    for item in settings["items"]:
        if item["key"] == "ns":
            item["value"] = "your-network"
        if item["key"] == "ha":
            hotspot = item["value"]
    status["wifi"] = {"mode": "hotspot", "ip": hotspot, "stations": [-52]}

    mock = ("<script>const _st=" + json.dumps(status) + ",_se=" + json.dumps(settings) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(u.includes('settings')?_se:_st)});</script>")
    page = page.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    page = page.replace('<section class="set"><details>', '<section class="set"><details open>', 1)

    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)
        (tmp / "device.webp").write_bytes(image)
        (tmp / "page.html").write_text(page, encoding="utf-8")
        (tmp / "frame.html").write_text(
            '<html><body style="margin:0"><iframe src="page.html" '
            f'style="width:390px;height:{args.height}px;border:0;display:block"></iframe></body></html>',
            encoding="utf-8")
        TARGET.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run([browser(), "--headless=new", "--disable-gpu", "--hide-scrollbars",
                        "--allow-file-access-from-files", "--force-device-scale-factor=2",
                        "--blink-settings=preferredColorScheme=1",  # light theme
                        f"--window-size=390,{args.height}", f"--screenshot={TARGET}",
                        (tmp / "frame.html").as_uri()],
                       check=True, capture_output=True)
    print(f"{TARGET.relative_to(ROOT)} written; check it before committing")


if __name__ == "__main__":
    main()
