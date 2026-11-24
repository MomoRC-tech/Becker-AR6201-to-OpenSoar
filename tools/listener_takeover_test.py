#!/usr/bin/env python3
"""Check that OpenSoar's "TCP port" device replaces a stale session.

Plays the bridge after a power cut, without any firmware change:

1. Connection A sends GPRMC/GPGGA once a second (speed 10 kn), then
   stays open and silent, like a session whose bridge lost power.
2. Connection B connects and sends fixes with a different speed (50 kn).

The phone is watched by the owner (device list: *GPS fix* / *No data*,
speed InfoBox). This script prints a timestamped line for every step and
reports whether and when the app closed connection A.

Synthetic position only. Example:

    python tools/listener_takeover_test.py <phone address> 4353
    python tools/listener_takeover_test.py <phone address> 4353 --bind <PC address on the hotspot>
"""

import argparse
import datetime as dt
import socket
import sys
import threading
import time

T0 = time.monotonic()


def log(msg):
    print(f"[{time.monotonic() - T0:7.1f} s {dt.datetime.now():%H:%M:%S}] {msg}", flush=True)


def nmea(body):
    cs = 0
    for ch in body:
        cs ^= ord(ch)
    return f"${body}*{cs:02X}\r\n"


def fix(speed_kn):
    now = dt.datetime.now(dt.timezone.utc)
    hms = now.strftime("%H%M%S") + ".00"
    dmy = now.strftime("%d%m%y")
    lat, lon = "4800.000", "01100.000"  # made-up position
    rmc = nmea(f"GPRMC,{hms},A,{lat},N,{lon},E,{speed_kn:.1f},90.0,{dmy},,,A")
    gga = nmea(f"GPGGA,{hms},{lat},N,{lon},E,1,08,1.0,500.0,M,47.0,M,,")
    return (rmc + gga).encode("ascii")


def connect(host, port, bind, name):
    t = time.monotonic()
    s = socket.create_connection((host, port), timeout=10,
                                 source_address=(bind, 0) if bind else None)
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    log(f"{name}: connected in {(time.monotonic() - t) * 1000:.0f} ms "
        f"(local {s.getsockname()[0]}:{s.getsockname()[1]})")
    return s


def watch_close(s, name, closed):
    """Report when the app closes the connection (EOF or reset)."""
    closed.at = None
    s.settimeout(None)
    try:
        while True:
            data = s.recv(4096)
            if not data:
                log(f"{name}: closed by the app (EOF)")
                break
            log(f"{name}: app sent {len(data)} bytes")
    except OSError as e:
        log(f"{name}: closed by the app ({e.__class__.__name__}: {e})")
    closed.at = time.monotonic()
    closed.set()


def send_for(s, name, seconds, speed):
    end = time.monotonic() + seconds
    n = 0
    while time.monotonic() < end:
        try:
            s.sendall(fix(speed))
        except OSError as e:
            log(f"{name}: send failed after {n} fixes ({e})")
            return False
        n += 1
        time.sleep(1.0)
    log(f"{name}: sent {n} fixes at {speed:.0f} kn")
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host", help="phone address")
    ap.add_argument("port", type=int, help="OpenSoar TCP port, e.g. 4353")
    ap.add_argument("--bind", help="local address to send from (PC address on the hotspot)")
    ap.add_argument("--feed", type=float, default=20, help="seconds A sends (default 20)")
    ap.add_argument("--silent", type=float, default=30,
                    help="seconds A stays open and silent before B (default 30)")
    ap.add_argument("--after", type=float, default=30, help="seconds B sends (default 30)")
    args = ap.parse_args()

    a_closed = threading.Event()
    a_closed.at = None
    a = connect(args.host, args.port, args.bind, "A")
    threading.Thread(target=watch_close, args=(a, "A", a_closed), daemon=True).start()
    log("A: sending 10 kn fixes - phone should show GPS fix")
    if not send_for(a, "A", args.feed, 10) or a_closed.is_set():
        log("Stop: the app closed A before B - is the TCP port device enabled and listening?")
        return 2
    log(f"A: silent for {args.silent:.0f} s, socket left open - phone should show No data")
    time.sleep(args.silent)
    if a_closed.is_set():
        log("Stop: A was closed while silent, before B - no stale session to replace")
        return 2

    b_closed = threading.Event()
    t_b = time.monotonic()
    b = connect(args.host, args.port, args.bind, "B")
    threading.Thread(target=watch_close, args=(b, "B", b_closed), daemon=True).start()
    log("B: sending 50 kn fixes - note when the phone shows GPS fix / 50 kn again")
    ok = send_for(b, "B", args.after, 50)

    print()
    if a_closed.is_set():
        log(f"Result: the app closed A {a_closed.at - t_b:.1f} s after B connected "
            "(stale session replaced)")
    else:
        log("Result: A is still open - the app did NOT replace the stale session")
    if b_closed.is_set() or not ok:
        log("Result: B was closed or failed - see the lines above")
    else:
        log(f"Result: B stayed connected for {time.monotonic() - t_b:.0f} s")
    passed = a_closed.is_set() and ok and not b_closed.is_set()
    a.close()
    b.close()
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
