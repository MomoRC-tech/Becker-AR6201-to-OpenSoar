"""Check the bridge's supply voltage output (firmware plan step 3d).

    python tools/supply_test.py <bridge address> [--seconds 10] [--expect-off]

Checks, against a running bridge:
- /api/status has a "supply" block; with the sensor on it shows a reading
- the OpenVario port (from /api/status) sends one "$POV,V,<volts>*hh" line per
  second with a valid checksum and nothing else
- data sent to the port, as OpenSoar's OpenVario driver does, is dropped and the
  connection stays open
- /api/supply has the chart data: points of [lowest, mean, highest] in
  centivolts, oldest first, lowest <= mean <= highest
- with --expect-off: the sensor is off and the port sends nothing

Reads only; changes no setting. Prints one line per check and exits 1 if one fails.
"""

import argparse
import json
import socket
import sys
import time
import urllib.request

failures = 0


def check(name: str, ok: bool, detail: str = "") -> None:
    global failures
    print(f"{'PASS' if ok else 'FAIL'}  {name}{'  (' + detail + ')' if detail else ''}")
    if not ok:
        failures += 1


def get_json(host: str, path: str) -> dict:
    with urllib.request.urlopen(f"http://{host}{path}", timeout=10) as r:
        return json.loads(r.read())


def checksum_ok(line: str) -> bool:
    if not line.startswith("$") or "*" not in line:
        return False
    body, _, given = line[1:].partition("*")
    sum_ = 0
    for c in body:
        sum_ ^= ord(c)
    return given.upper() == f"{sum_:02X}"


def read_lines(host: str, port: int, seconds: float, poke: bool) -> tuple[list[str], bool]:
    """Lines received in `seconds`; True if the connection stayed open."""
    lines, buf, open_ = [], b"", True
    with socket.create_connection((host, port), timeout=5) as s:
        s.settimeout(0.5)
        if poke:  # what OpenSoar's OpenVario driver sends when MacCready changes
            s.sendall(b"$POV,C,MC,1.50*30\r\n")
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            try:
                chunk = s.recv(1024)
            except socket.timeout:
                continue
            if not chunk:
                open_ = False
                break
            buf += chunk
            while b"\n" in buf:
                raw, buf = buf.split(b"\n", 1)
                lines.append(raw.decode("ascii", "replace").rstrip("\r"))
    return lines, open_


def main() -> None:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host")
    ap.add_argument("--seconds", type=float, default=10)
    ap.add_argument("--expect-off", action="store_true", help="the sensor is switched off in the settings")
    args = ap.parse_args()

    status = get_json(args.host, "/api/status")
    sup = status.get("supply")
    check("status has a supply block", isinstance(sup, dict))
    if not isinstance(sup, dict):
        sys.exit(1)
    print(f"      firmware {status['firmware']}, sensor {'on' if sup['sensor'] else 'off'}, "
          f"{sup['volts']} V (raw {sup['raw_volts']} V), factor {sup['calibration']}, TCP {sup['tcp_port']}")

    if args.expect_off:
        check("sensor is off", sup["sensor"] is False)
        check("no reading while off", sup["volts"] is None)
        try:
            lines, _ = read_lines(args.host, sup["tcp_port"], 3, False)
            check("port sends nothing while off", not lines, f"{len(lines)} lines")
        except OSError:
            check("port closed while off", True)
    else:
        check("sensor is on", sup["sensor"] is True)
        check("reading in the plausible range", sup["volts"] is not None and 5 <= sup["volts"] <= 30, str(sup["volts"]))
        lines, open_ = read_lines(args.host, sup["tcp_port"], args.seconds, True)
        good = [l for l in lines if l.startswith("$POV,V,") and checksum_ok(l)]
        check("only $POV,V lines with a valid checksum", lines and len(good) == len(lines),
              f"{len(good)} of {len(lines)}" + (f", first other: {next(l for l in lines if l not in good)!r}" if len(good) != len(lines) else ""))
        expected = args.seconds
        check("about one line per second", abs(len(lines) - expected) <= 2, f"{len(lines)} in {args.seconds:g} s")
        check("data from the app dropped, connection stays open", open_)
        if good:
            values = [float(l[7:].split("*")[0]) for l in good]
            print(f"      values {min(values):.2f} to {max(values):.2f} V")

    hist = get_json(args.host, "/api/supply")
    pts = hist.get("points")
    check("chart data present", isinstance(pts, list) and hist.get("interval_s") == 60)
    if isinstance(pts, list):
        check("at most 720 points (12 h)", len(pts) <= 720, f"{len(pts)} points")
        bad = [p for p in pts if p is not None and not (len(p) == 3 and p[0] <= p[1] <= p[2])]
        check("each point lowest <= mean <= highest", not bad, f"{len(bad)} bad")
        cur = hist.get("current")
        check("current minute well formed", cur is None or (len(cur) == 3 and cur[0] <= cur[1] <= cur[2]), str(cur))
        print(f"      {len(pts)} full minutes, newest closed {hist['age_s']} s ago")

    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
