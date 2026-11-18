"""Bench checks for the bridge web log (firmware plan step 3c).

    python tools/log_test.py <bridge address>          # after any start
    python tools/log_test.py <bridge address> --kept   # right after a restart (OTA upload)

Needs the bridge in network mode and no app connected to it. No jumpers needed.

Checks:
 1. the page and /api/log are served; the start line names firmware and reset
 2. the settings were logged at start-up
 3. an app connecting to and leaving the Zander port is logged (Ports, Info)
 4. the radio port: owner taken and released (Info), second app refused (Warning)
 5. ?since= returns only newer lines, oldest first
 6. every line has a known level and source
With --kept:
 7. lines from before the restart are still there, with a lower start number
"""

import argparse
import json
import socket
import sys
import time
import urllib.request

PORTS = (10110, 2000, 4352)
LEVELS = set("EWID")
SOURCES = {"wifi", "ports", "radio", "settings", "system"}
results = []


def check(name, ok, detail=""):
    results.append(ok)
    print(("PASS " if ok else "FAIL ") + name + (f": {detail}" if detail else ""))


def log(host, since=0):
    with urllib.request.urlopen(f"http://{host}/api/log?since={since}", timeout=10) as r:
        return json.loads(r.read())


def find(lines, source, level, text):
    return [l for l in lines if l["s"] == source and l["l"] == level and text in l["m"]]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host")
    ap.add_argument("--kept", action="store_true", help="also check that lines from before a restart are kept")
    args = ap.parse_args()
    host = args.host

    with urllib.request.urlopen(f"http://{host}/log", timeout=10) as r:
        page = r.read()
    check("log page served", r.status == 200 and b"WIFI Bridge log" in page, f"{len(page)} bytes")

    d = log(host)
    now = [l for l in d["lines"] if l["b"] == d["boot"]]
    start = [l for l in now if l["s"] == "system" and l["m"].startswith("Start, firmware")]
    check("start line with firmware and reset reason", bool(start) and d["firmware"] in start[0]["m"] and "reset:" in start[0]["m"],
          start[0]["m"] if start else "missing")
    check("settings logged at start-up", bool([l for l in now if l["s"] == "settings" and "Settings" in l["m"]]))

    seq = d["seq"]
    app = socket.create_connection((host, PORTS[0]), timeout=5)
    time.sleep(2.5)
    app.close()
    time.sleep(2.5)
    d = log(host, seq)
    check("Zander app connect logged", bool(find(d["lines"], "ports", "I", "Zander: 1 app connected")))
    check("Zander app disconnect logged", bool(find(d["lines"], "ports", "I", "Zander: 0 apps connected")))

    seq = d["seq"]
    owner = socket.create_connection((host, PORTS[2]), timeout=5)
    time.sleep(1)
    second = socket.create_connection((host, PORTS[2]), timeout=5)
    time.sleep(1)
    second.close()
    owner.close()
    time.sleep(2)
    d = log(host, seq)
    check("radio owner logged", bool(find(d["lines"], "radio", "I", "took the radio port")))
    refused = find(d["lines"], "radio", "W", "refused: another app owns the radio port")
    check("second radio app refused as a warning", bool(refused), refused[0]["m"] if refused else "missing (one line per 30 s)")
    check("radio owner release logged", bool(find(d["lines"], "radio", "I", "released the radio port")))

    q = [l["q"] for l in d["lines"]]
    check("?since= returns newer lines only, oldest first", all(x > seq for x in q) and q == sorted(q), f"{len(q)} lines")

    every = log(host)["lines"]
    bad = [l for l in every if l["l"] not in LEVELS or l["s"] not in SOURCES]
    check("every line has a known level and source", not bad, f"{len(every)} lines")

    if args.kept:
        boot = log(host)["boot"]
        old = [l for l in every if l["b"] < boot]
        check("lines from before the restart kept", bool(old), f"{len(old)} older lines, start number now {boot}")

    print(f"{sum(results)}/{len(results)} checks passed")
    sys.exit(0 if all(results) else 1)


if __name__ == "__main__":
    main()
