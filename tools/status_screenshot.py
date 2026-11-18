"""Screenshot the bridge status and settings pages for the documentation.

Reads both pages, the image and the settings JSON from a running bridge,
masks private details, and renders them at phone width (390 px, 2x) in the
light theme: docs/images/status-page.png ("All settings" open),
docs/images/settings-page.png, docs/images/monitor-page.png and
docs/images/log-page.png. The settings page login uses the OTA password from
firmware/bridge/credentials.h. The monitor and the log show fixed demo data,
never real data: real NMEA contains positions, the real log network names and
addresses.

    python tools/status_screenshot.py <bridge address>

Masked before rendering (the repository is public):
- network name and its default -> "your-network", as in credentials.example.h
- Wi-Fi line -> hotspot mode at the configured hotspot address with one phone

Needs Google Chrome or Microsoft Edge. Rerun after every change to the
status page and commit the new image (AGENTS.md).
"""

import argparse
import base64
import datetime
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


DEMO_LINES = [  # made-up NMEA, round fake position
    (0, "$GPRMC,120000.00,A,4800.000,N,00800.000,E,045.0,090.0,171126,,,A*00"),
    (0, "$PZAN1,00450,123456*00"),
    (0, "$PZAN2,0820,0012*00"),
    (0, "$PZAN3,+00,AFR,1,*00"),
    (1, "$PZAN5,SF,MUEHL,123.4,1,10,MC,0.5*00"),
    (0, "$GPRMC,120001.00,A,4800.010,N,00800.020,E,045.2,090.5,171126,,,A*00"),
    (0, "$PZAN1,00452,123456*00"),
    (0, "$PZAN2,0821,0015*00"),
    (0, "$PZAN3,+00,AFR,1,*00"),
]


def demo_monitor() -> dict:
    chunks, t = [], 61234
    for d, line in DEMO_LINES:
        chunks.append({"t": t, "d": d, "x": (line + "\r\n").encode().hex()})
        t += 180 if d else 330
    return {"seq": len(chunks), "lost": False, "now": t, "chunks": chunks}


DEMO_START = datetime.datetime(2026, 11, 18, 14, 2, 0)  # clock time of start 2; start 1 had no phone time
DEMO_LOG = [  # (start number, ms since start, level, source, message); start 2 is the current one
    (1, 412, "I", "system", "Start, firmware {fw}, reset: power on"),
    (1, 431, "I", "settings", "Settings: defaults, nothing stored"),
    (1, 937, "I", "wifi", "Joining network your-network for up to 15 s"),
    (1, 3260, "I", "wifi", "Connected to the network, address 192.168.1.50"),
    (1, 95120, "I", "system", "Firmware update started (sketch)"),
    (1, 118470, "I", "system", "Firmware update received, restarting"),
    (2, 409, "I", "system", "Start, firmware {fw}, reset: restart by the firmware"),
    (2, 428, "I", "settings", "Settings: defaults, nothing stored"),
    (2, 936, "I", "wifi", "Joining network your-network for up to 15 s"),
    (2, 15940, "W", "wifi", "Network your-network not reached, opening the hotspot"),
    (2, 17985, "I", "wifi", "Hotspot WiFi-BridgeTom open at 10.10.10.10"),
    (2, 31200, "I", "wifi", "A phone joined the hotspot, 1 connected"),
    (2, 31950, "I", "system", "Clock set from a phone"),
    (2, 33010, "I", "ports", "Zander: data arriving"),
    (2, 33020, "I", "ports", "FLARM: data arriving"),
    (2, 35870, "I", "ports", "Zander: 1 app connected (before 0)"),
    (2, 35900, "I", "radio", "App 10.10.10.2 took the radio port"),
    (2, 41550, "W", "radio", "App 10.10.10.3 refused: another app owns the radio port (0 more since the last line)"),
    (2, 60010, "D", "system", "Memory free 182 kB, lowest 171 kB; hotspot, 1 phone(s)"),
    (2, 64300, "W", "ports", "FLARM: 3 receive errors, last: framing, check baud and frame"),
]


def demo_log(firmware: str) -> dict:
    start = int(DEMO_START.timestamp() * 1000)
    lines = [{"q": q, "b": b, "t": t, "c": start + t if b == 2 else 0, "l": l, "s": s, "m": m.format(fw=firmware)}
             for q, (b, t, l, s, m) in enumerate(DEMO_LOG, 1)]
    return {"firmware": firmware, "boot": 2, "seq": len(lines), "now": 66000, "lost": False, "lines": lines}


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

    demo = demo_monitor()
    empty = {"seq": demo["seq"], "lost": False, "now": demo["now"], "chunks": []}
    mon = get(args.host, "/monitor").decode("utf-8")
    mock = ("<script>const _d=" + json.dumps(demo) + ",_e=" + json.dumps(empty) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(u.includes('since=0')?_d:_e)});</script>")
    mon = mon.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    shoot(mon, image, IMAGES / "monitor-page.png", 760)

    demo = demo_log(status["firmware"].replace("Wi-Fi bridge ", ""))
    empty = dict(demo, lines=[])
    page = get(args.host, "/log").decode("utf-8")
    mock = ("<script>localStorage.clear();const _d=" + json.dumps(demo) + ",_e=" + json.dumps(empty) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(u.includes('since=0')?_d:_e)});</script>")
    page = page.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    shoot(page, image, IMAGES / "log-page.png", 900)


if __name__ == "__main__":
    main()
