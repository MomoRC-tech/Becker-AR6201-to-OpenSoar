"""Bench checks for the lifetime record (firmware plan step 3f).

    python tools/records_test.py <bridge address>
    python tools/records_test.py <bridge address> --write-load 120

Needs the bridge firmware in network mode, so the address stays the same
across the restart. Uses the password from firmware/bridge/credentials.h.
Resets the lifetime record of the board it runs against.

Checks:
- /api/status has the lifetime block with storage available;
- a reset with a wrong password changes nothing;
- a reset with the password starts a new record (1 start, little operating time);
- after a restart by the firmware the record is kept, with one more start and
  the operating time carried over.

--write-load SECONDS: resets the record every 2 s, each one a flash write, far
more often than the firmware writes on its own, while
tools/bridge_acceptance_test.py --radio-loop --baseline streams data through
the step 2 jumper loops. Passes when that test passes and no UART receive
errors were counted, so flash writes do not lose serial data.
"""

import argparse
import base64
import json
import re
import subprocess
import sys
import threading
import time
import urllib.error
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
USER = "bridge"
results = []


def check(name, ok, detail=""):
    results.append(ok)
    print(("PASS " if ok else "FAIL ") + name + (f": {detail}" if detail else ""))


def password() -> str:
    text = (ROOT / "firmware" / "bridge" / "credentials.h").read_text()
    return re.search(r'#define\s+OTA_PASSWORD\s+"([^"]*)"', text).group(1)


def request(host, path, post=False, auth=None):
    req = urllib.request.Request(f"http://{host}{path}", data=b"" if post else None)
    if auth:
        req.add_header("Authorization", "Basic " + base64.b64encode(f"{USER}:{auth}".encode()).decode())
    try:
        with urllib.request.urlopen(req, timeout=10) as r:
            return r.status, r.read()
    except urllib.error.HTTPError as e:
        return e.code, e.read()


def status(host):
    return json.loads(request(host, "/api/status")[1])


def uart_errors(host):
    return sum(p["uart_errors"] for p in status(host)["ports"])


def wait_back(host, timeout=60):
    time.sleep(3)
    end = time.time() + timeout
    while time.time() < end:
        try:
            s = status(host)
            if s["uptime_s"] < 60:
                return s
        except Exception:
            pass
        time.sleep(2)
    raise SystemExit("bridge did not come back")


def record_checks(host, pw):
    s = status(host)
    life = s.get("lifetime")
    check("status has the lifetime block", life is not None and life.get("storage") is True, json.dumps(life))
    if life is None:
        return

    code, _ = request(host, "/api/lifetime/reset", post=True, auth="wrong-password")
    after = status(host)["lifetime"]
    check("wrong password refused, record unchanged", code == 401 and after["starts"] == life["starts"],
          f"HTTP {code}, starts {life['starts']} -> {after['starts']}")

    code, _ = request(host, "/api/lifetime/reset", post=True, auth=pw)
    fresh = status(host)["lifetime"]
    check("reset starts a new record", code == 200 and fresh["starts"] == 1 and fresh["up_s"] < 10
          and fresh["max_chip"] is None and fresh["min_supply"] is None, json.dumps(fresh))

    time.sleep(15)
    before = status(host)["lifetime"]
    code, _ = request(host, "/api/settings", post=True, auth=pw)  # nothing changed: save and restart
    print(f"restart through the settings page: HTTP {code}")
    wait_back(host)
    kept = status(host)["lifetime"]
    check("kept through a restart, one more start", kept["starts"] == before["starts"] + 1, f"{before['starts']} -> {kept['starts']}")
    check("operating time carried over", kept["up_s"] >= before["up_s"], f"{before['up_s']} s -> {kept['up_s']} s")
    check("extremes kept", kept["max_chip"] == before["max_chip"] and kept["min_supply"] == before["min_supply"],
          json.dumps([kept["max_chip"], kept["min_supply"]]))


def write_load(host, pw, seconds):
    errors_before = uart_errors(host)
    stop = threading.Event()
    writes = [0, 0]  # done, failed

    def hammer():
        while not stop.is_set():
            code, _ = request(host, "/api/lifetime/reset", post=True, auth=pw)
            writes[0 if code == 200 else 1] += 1
            stop.wait(2)

    t = threading.Thread(target=hammer, daemon=True)
    t.start()
    test = subprocess.run([sys.executable, str(ROOT / "tools" / "bridge_acceptance_test.py"), host, "--radio-loop",
                           "--baseline", "--seconds", str(seconds)], capture_output=True, text=True)
    stop.set()
    t.join()
    print(test.stdout.strip())
    errors_after = uart_errors(host)
    check(f"acceptance test under {writes[0]} flash writes", test.returncode == 0 and writes[0] > 0,
          f"exit {test.returncode}, {writes[1]} failed requests")
    check("no UART receive errors during the writes", errors_after == errors_before, f"{errors_before} -> {errors_after}")


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host")
    ap.add_argument("--write-load", type=float, metavar="SECONDS", help="acceptance test while the record is written every 2 s")
    a = ap.parse_args()
    pw = password()
    if a.write_load:
        write_load(a.host, pw, a.write_load)
    else:
        record_checks(a.host, pw)
    print(f"{sum(results)}/{len(results)} checks passed")
    sys.exit(0 if all(results) else 1)


if __name__ == "__main__":
    main()
