"""Screenshot the bridge status and settings pages for the documentation.

Reads both pages, the image and the settings JSON from a running bridge,
masks private details, and renders them at phone width (390 px, 2x) in the
light theme: docs/images/status-start.png (the status page as it opens, for the README),
docs/images/status-page.png (startup temperatures, lifetime record and settings expanded),
docs/images/settings-page.png, docs/images/monitor-page.png,
docs/images/log-page.png and docs/images/saved-page.png. The settings page login comes from the
BRIDGE_PASSWORD environment variable (none needed with the bench build). The monitor, the log, startup and live chip temperatures, the supply voltage
(value and chart), the device traffic, the lifetime record and the saved log show fixed demo data, never real data: real NMEA contains
positions, the real log network names and addresses, and a bench board
without the divider has no meaningful voltage.

    python tools/status_screenshot.py <bridge address>

Masked before rendering (the repository is public):
- a stored network name -> "your-network"
- the bench build's "-BENCH" version suffix -> removed (the pages are the same)
- Wi-Fi line -> hotspot mode at the configured hotspot address with one phone

Needs Google Chrome or Microsoft Edge. Rerun after every change to the
status page and commit the new image (AGENTS.md).
"""

import argparse
import base64
import datetime
import json
import math
import os
import random
import re
import shutil
import subprocess
import tempfile
import time
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
    """Settings login from the BRIDGE_PASSWORD environment variable; empty sends no login (bench build)."""
    return os.environ.get("BRIDGE_PASSWORD", "")


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


def demo_supply() -> tuple[dict, dict]:
    """A made-up five-hour day: charged battery on the ground, slow decline in flight,
    short dips while the radio transmits."""
    rnd = random.Random(7)
    points = []
    for k in range(300):
        mean = 12.95 - 0.55 * k / 300 - 0.15 * math.exp(-k / 20) + rnd.uniform(-0.02, 0.02)
        dip = 0.35 + rnd.uniform(0, 0.25) if rnd.random() < 0.18 else rnd.uniform(0.02, 0.06)
        points.append([round((mean - dip) * 100), round(mean * 100), round((mean + rnd.uniform(0.02, 0.05)) * 100)])
    history = {"sensor": True, "interval_s": 60, "low_v": 11.5, "age_s": 25, "current": [1235, 1240, 1244],
               "points": points}
    status = {"sensor": True, "volts": 12.41, "raw_volts": 12.38, "calibration": 1.002, "tcp_port": 4353,
              "tcp_clients": 1, "sentences": 18000, "skipped": 0, "low_v": 11.5}
    return status, history


def demo_log(firmware: str) -> dict:
    start = int(DEMO_START.timestamp() * 1000)
    lines = [{"q": q, "b": b, "t": t, "c": start + t if b == 2 else 0, "l": l, "s": s, "m": m.format(fw=firmware)}
             for q, (b, t, l, s, m) in enumerate(DEMO_LOG, 1)]
    return {"firmware": firmware, "boot": 2, "seq": len(lines), "now": 66000, "lost": False, "lines": lines}


def demo_saved(firmware: str) -> str:
    """Saved log as /api/saved sends it: start, Unix ms, ms since start, level, source, message."""
    day = int(time.time() // 86400 * 86400 * 1000) - 2 * 86400 * 1000 + 13 * 3600 * 1000  # two days ago, 13:00 UTC
    rows = [
        (55, day, 900, "I", "system", f"Start, firmware {firmware}, reset: power on"),
        (55, day + 1520000, 1521000, "W", "wifi", "Network connection lost: BEACON_TIMEOUT (200)"),
        (55, day + 5410000, 5411000, "W", "system", "Chip temperature high: 81 °C for 60 s; the bridge keeps running"),
        (56, 0, 950, "W", "system", "Start, firmware " + firmware + ", reset: brownout, the supply voltage dropped"),
        (56, 0, 64000, "W", "system", "Supply voltage low: 11.32 V for 10 s"),
        (57, day + 86400000, 880, "I", "system", f"Start, firmware {firmware}, reset: power on"),
        (57, day + 86400000 + 734000, 735000, "W", "ports", "FLARM: 3 receive errors, last: framing, check baud and frame"),
    ]
    return "# used 16384 of 131072 bytes\n" + "".join("\t".join(str(x) for x in r) + "\n" for r in rows)


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
    ap.add_argument("--height", type=int, default=2650, help="status page height in CSS pixels")
    args = ap.parse_args()

    page = get(args.host, "/").decode("utf-8")
    image = get(args.host, "/device.webp")
    status = json.loads(get(args.host, "/api/status"))
    settings = json.loads(get(args.host, "/api/settings"))

    hotspot = "10.10.10.10"
    for item in settings["items"]:
        if item["key"] == "ns" and item["value"]:
            item["value"] = "your-network"
        if item["key"] == "ha":
            hotspot = item["value"]
    status["wifi"] = {"mode": "hotspot", "ip": hotspot, "stations": [-52]}
    status["firmware"] = status["firmware"].replace("-BENCH", "")  # bench build: pages as in the normal build
    status["supply"], history = demo_supply()
    status["uptime_s"] = 5 * 3600 + 25  # matches the five-hour demo chart
    for port, (rx, tx, ago) in zip(status.get("ports", []), ((1843200, 0, 180), (2211840, 0, 240), (5120, 5120, 900))):
        # Demo traffic, as in flight: every device sends and OpenSoar is connected; bench values vary from run to run.
        port.update(bytes_from_device=rx, bytes_to_device=tx, last_data_ms_ago=ago, tcp_clients=1,
                    uart_errors=0, short_writes=0)
    if "boot_samples" in status.get("chip_temp", {}):
        # Synthetic uncorrected readings for the startup table in public screenshots.
        status["chip_temp"]["boot_samples"] = [
            {"ms": i * 10000 if i else 98, "celsius": round((raw - 32) / 1.8, 1)}
            for i, raw in enumerate((110, 113, 115, 116, 119, 121, 123, 124, 125, 126, 127, 129, 130))
        ]
        status["chip_temp"]["celsius"] = 56.2
    if "lifetime" in status:  # demo record, consistent with the demo chart
        now = int(time.time())
        status["lifetime"] = {"storage": True, "starts": 57, "up_s": 123 * 3600 + 25 * 60, "since_unix": now - 40 * 86400,
                              "max_chip": {"celsius": 61.4, "up_s": 98 * 3600 + 12 * 60, "unix": now - 9 * 86400},
                              "min_supply": {"volts": 11.86, "up_s": 121 * 3600 + 3 * 60, "unix": now - 2 * 86400}}

    mock = ("<script>const _st=" + json.dumps(status) + ",_se=" + json.dumps(settings) + ",_su=" + json.dumps(history) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(u.includes('settings')?_se:"
            "u.includes('supply')?_su:_st)});</script>")
    page = page.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    shoot(page, image, IMAGES / "status-start.png", 575)  # as it opens: header and device rows (README)
    page = page.replace('<section class="set"><details>', '<section class="set"><details open>', 1)
    page = page.replace('id="lt" hidden><details>', 'id="lt" hidden><details open>', 1)
    page = page.replace('id="bootTemps" hidden><details>', 'id="bootTemps" hidden><details open>', 1)

    shoot(page, image, IMAGES / "status-page.png", args.height)

    form = get(args.host, "/settings", ota_password()).decode("utf-8")
    mock = ("<script>const _se=" + json.dumps(settings) + ",_st=" + json.dumps(status) +
            ";window.fetch=u=>Promise.resolve({json:()=>Promise.resolve(u.includes('status')?_st:_se)});</script>")
    form = form.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
    shoot(form, image, IMAGES / "settings-page.png", 3130)

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

    if status.get("saved_log"):
        text = demo_saved(status["firmware"].replace("Wi-Fi bridge ", ""))
        page = get(args.host, "/saved").decode("utf-8")
        mock = ("<script>const _t=" + json.dumps(text) +
                ";window.fetch=u=>Promise.resolve({ok:true,text:()=>Promise.resolve(_t)});</script>")
        page = page.replace("/device.webp", "device.webp").replace("<main>", mock + "<main>", 1)
        shoot(page, image, IMAGES / "saved-page.png", 840)


if __name__ == "__main__":
    main()
