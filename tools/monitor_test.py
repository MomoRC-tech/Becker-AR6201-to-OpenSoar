"""Bench checks for the bridge data monitor (firmware plan step 3b).

    python tools/monitor_test.py <bridge address>

Needs the step 2 jumpers (Zander out -> FLARM in, FLARM out -> Zander in,
radio out -> radio in) and a bridge that nobody watched since its last start.

Checks:
 1. nothing is collected before anyone opens the monitor
 2. a line sent to the Zander port shows on Zander as "to device" and on
    FLARM as "from device" (it arrives through the jumper)
 3. radio bytes, including 00 and A5, show both ways on the radio port
 4. collection stops 10 s after the last poll
 5. the page itself is served
"""

import argparse
import json
import socket
import sys
import time
import urllib.request

PORTS = (10110, 2000, 4352)
results = []


def check(name, ok, detail=""):
    results.append(ok)
    print(("PASS " if ok else "FAIL ") + name + (f": {detail}" if detail else ""))


def mon(host, port, since=0):
    with urllib.request.urlopen(f"http://{host}/api/monitor?port={port}&since={since}", timeout=10) as r:
        return json.loads(r.read())


def chunks(d, direction):
    return b"".join(bytes.fromhex(c["x"]) for c in d["chunks"] if c["d"] == direction)


def send(host, tcp, data, listen=None, wait=1.0):
    """Send data to one TCP port; optionally collect what arrives on another."""
    rx = None
    if listen is not None:
        rx = socket.create_connection((host, listen), timeout=5)
    tx = socket.create_connection((host, tcp), timeout=5)
    time.sleep(0.3)
    tx.sendall(data)
    time.sleep(wait)
    got = b""
    if rx is not None:
        rx.settimeout(0.5)
        try:
            while True:
                part = rx.recv(4096)
                if not part:
                    break
                got += part
        except socket.timeout:
            pass
        rx.close()
    tx.close()
    return got


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host")
    host = ap.parse_args().host

    send(host, PORTS[0], b"$PTEST,unwatched*00\r\n")
    d = mon(host, 0)
    check("nothing collected before the monitor opens", d["seq"] == 0, f"seq {d['seq']}")

    line = b"$PTEST,monitor,check*00\r\n"
    z0, f0 = mon(host, 0)["seq"], mon(host, 1)["seq"]
    send(host, PORTS[0], line)
    z, f = mon(host, 0, z0), mon(host, 1, f0)
    check("Zander shows the line as sent to the device", line in chunks(z, 1))
    check("FLARM shows the line as received from the device", line in chunks(f, 0))

    frame = bytes([0xA5, 0x14, 0x05, 0x16, 0x00, 0x00, 0x00, 0xA5, 0x12, 0x34])
    r0 = mon(host, 2)["seq"]
    send(host, PORTS[2], frame)
    r = mon(host, 2, r0)
    check("radio bytes shown sent to the radio", frame in chunks(r, 1), chunks(r, 1).hex(" "))
    check("radio bytes shown received from the radio", frame in chunks(r, 0), chunks(r, 0).hex(" "))

    print("waiting 12 s without polling ...")
    time.sleep(12)
    before = mon(host, 0)["seq"]  # this poll restarts collection, so read the count first ...
    time.sleep(12)                # ... and stay quiet again
    send(host, PORTS[0], b"$PTEST,idle*00\r\n")
    after = mon(host, 0)["seq"]
    check("collection stops 10 s after the last poll", after == before, f"seq {before} -> {after}")

    with urllib.request.urlopen(f"http://{host}/monitor", timeout=10) as r:
        page = r.read()
    check("monitor page served", r.status == 200 and b"WIFI Bridge data" in page, f"{len(page)} bytes")

    print(f"{sum(results)}/{len(results)} checks passed")
    sys.exit(0 if all(results) else 1)


if __name__ == "__main__":
    main()
