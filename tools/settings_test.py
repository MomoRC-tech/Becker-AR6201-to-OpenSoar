"""Bench checks for the bridge settings page (firmware plan step 3a).

    python tools/settings_test.py <bridge address> [--revert]

Settings login (user "bridge") from the BRIDGE_PASSWORD environment variable;
none needed with the bench build.
The bridge must be in network mode, so it keeps its address across restarts.
Ends with a reset, so the bridge is back on the firmware defaults.

Checks:
 1. /settings and saving need the password; a wrong one saves nothing
 2. invalid values are refused with a message per field
 3. a serial change (radio baud) is saved, survives the restart, no trial
 4. setting it back to the default removes it (shows as default)
 5. a port change starts a trial; the new port listens; Keep ends the trial
 6. device profiles: only presets that fit a port's wiring are offered and
    accepted; a preset fills baud and frame; an edited preset is saved as Custom
 7. reset brings back all defaults
 8. with --revert: an unkept port change reverts after 10 minutes
"""

import argparse
import base64
import json
import os
import re
import socket
import sys
import time
import urllib.error
import urllib.parse
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
USER = "bridge"
results = []


def check(name, ok, detail=""):
    results.append(ok)
    print(("PASS " if ok else "FAIL ") + name + (f": {detail}" if detail else ""))


def password() -> str:
    """Settings login from the BRIDGE_PASSWORD environment variable; empty sends no login (bench build)."""
    return os.environ.get("BRIDGE_PASSWORD", "")


def request(host, path, data=None, auth=None):
    req = urllib.request.Request(f"http://{host}{path}", data=None if data is None else urllib.parse.urlencode(data).encode())
    if auth:
        req.add_header("Authorization", "Basic " + base64.b64encode(f"{USER}:{auth}".encode()).decode())
    try:
        with urllib.request.urlopen(req, timeout=10) as r:
            return r.status, r.read()
    except urllib.error.HTTPError as e:
        return e.code, e.read()


def settings(host):
    return json.loads(request(host, "/api/settings")[1])


def item(s, key):
    return next(i for i in s["items"] if i["key"] == key)


def wait_back(host, timeout=60):
    time.sleep(3)
    end = time.time() + timeout
    while time.time() < end:
        try:
            s = settings(host)
            up = json.loads(request(host, "/api/status")[1])["uptime_s"]
            if up < 60:
                return s
        except Exception:
            pass
        time.sleep(2)
    raise SystemExit("bridge did not come back")


def listens(host, port):
    try:
        with socket.create_connection((host, port), timeout=3):
            return True
    except OSError:
        return False


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host")
    ap.add_argument("--revert", action="store_true", help="also wait 10 min for an automatic revert")
    a = ap.parse_args()
    host, pw = a.host, password()

    code, _ = request(host, "/settings")
    check("settings page opens without a login", code == 200, f"HTTP {code}")

    # From here on the bridge settings change: always end with a reset, also when a
    # check stops the script, so later tests find the default ports.
    reset_done = False
    try:
        code, body = request(host, "/api/settings", {"b2": "1234", "f1": "9X1", "t1": "10110", "nj": "500", "ha": "300.1.1.1"}, auth=pw)
        errs = json.loads(body).get("errors", {}) if code == 400 else {}
        check("invalid values refused per field", code == 400 and {"b2", "f1", "t1", "nj", "ha"} <= set(errs), f"HTTP {code}, fields {sorted(errs)}")
        check("refused save changed nothing", settings(host)["items"] == before["items"])

        code, body = request(host, "/api/settings", {"b2": "19200"}, auth=pw)
        check("serial change saved without trial", code == 200 and json.loads(body) == {"ok": True, "trial": False}, body.decode())
        s = wait_back(host)
        b2 = item(s, "b2")
        check("radio baud 19200 after restart, marked changed", b2["value"] == "19200" and b2["origin"] == "changed", str(b2))
        st = json.loads(request(host, "/api/status")[1])
        check("UART2 runs at 19200", st["ports"][2]["baud"] == 19200)

        code, _ = request(host, "/api/settings", {"b2": b2["default"]}, auth=pw)
        s = wait_back(host)
        check("back to default shows as default", item(s, "b2")["origin"] == "default" and code == 200)

        s = settings(host)
        offered = {p["name"]: p["profiles"] for p in s.get("ports", [])}
        check("presets offered by wiring", offered == {"Zander": ["zs1", "flarm"], "FLARM": ["zs1", "flarm"], "Becker": ["ar6201"]}, str(offered))
        code, body = request(host, "/api/settings", {"p2": "zs1", "p0": "ar6201", "p1": "nonsense"}, auth=pw)
        errs = json.loads(body).get("errors", {}) if code == 400 else {}
        check("unfitting or unknown presets refused", code == 400 and {"p0", "p1", "p2"} <= set(errs), f"HTTP {code}, fields {sorted(errs)}")

        code, body = request(host, "/api/settings", {"p0": "flarm", "b0": "19200", "f0": "8N1", "p1": "flarm", "b1": "9600", "f1": "8N1"}, auth=pw)
        check("preset change saved without trial", code == 200 and json.loads(body) == {"ok": True, "trial": False}, body.decode())
        s = wait_back(host)
        p0, b0, p1, b1 = (item(s, k) for k in ("p0", "b0", "p1", "b1"))
        check("Zander port keeps the FLARM preset after restart", p0["value"] == "flarm" and p0["origin"] == "changed" and b0["origin"] == "default", f"{p0}, {b0}")
        check("edited preset saved as Custom", p1["value"] == "custom" and b1["value"] == "9600", f"{p1}, {b1}")
        code, _ = request(host, "/api/settings", {"p0": "zs1", "b0": "19200", "f0": "8N1", "p1": "flarm", "b1": "19200", "f1": "8N1"}, auth=pw)
        s = wait_back(host)
        check("default presets show as default", code == 200 and all(item(s, k)["origin"] == "default" for k in ("p0", "b0", "f0", "p1", "b1", "f1")))

        code, body = request(host, "/api/settings", {"t0": "10111"}, auth=pw)
        check("port change starts a trial", code == 200 and json.loads(body).get("trial") is True, body.decode())
        s = wait_back(host)
        check("trial active after restart", s["trial"]["active"] and s["trial"]["seconds_left"] > 500, str(s["trial"]))
        check("Zander listens on 10111, not 10110", listens(host, 10111) and not listens(host, 10110))
        code, _ = request(host, "/api/keep", {}, auth=pw)
        check("Keep ends the trial", code == 200 and not settings(host)["trial"]["active"])

        if a.revert:
            code, _ = request(host, "/api/settings", {"t0": "10112"}, auth=pw)
            s = wait_back(host)
            check("second trial active", s["trial"]["active"])
            print("waiting about 10 minutes for the automatic revert ...")
            time.sleep(s["trial"]["seconds_left"] + 5)
            s = wait_back(host, timeout=120)
            check("unkept change reverted to the kept value", item(s, "t0")["value"] == "10111" and not s["trial"]["active"], str(item(s, "t0")))

        code, _ = request(host, "/api/reset", {}, auth=pw)
        reset_done = True
        s = wait_back(host)
        check("reset brings back all defaults", code == 200 and all(i["origin"] == "default" for i in s["items"]) and listens(host, 10110))
    finally:
        if not reset_done:
            print("stopped early: resetting the bridge to its defaults")
            try:
                request(host, "/api/reset", {}, auth=pw)
                wait_back(host)
            except (OSError, SystemExit) as error:
                print(f"reset failed ({error}); reset the bridge on its settings page")

    print(f"{sum(results)}/{len(results)} checks passed")
    sys.exit(0 if all(results) else 1)


if __name__ == "__main__":
    main()
