"""Exercise navigation detection on a bench bridge and one wired RS-232 test source.

    python tools/nav_scan_test.py <bridge address> --feed COM4 --input flarm

Needs pyserial. Sends synthetic sentences only through the named PC serial adapter.
Leaves saved bridge settings alone. The two navigation inputs must be free of instruments.
"""
import argparse
from functools import reduce
import json
from operator import xor
import socket
import time

import serial

from settings_test import password, request, settings


def sentence(body):
    data = body.encode("ascii")
    return b"$%s*%02X\r\n" % (data, reduce(xor, data, 0))


def verify_forwarding(host, tcp_port, source):
    with socket.create_connection((host, tcp_port), timeout=4) as client:
        time.sleep(0.3)
        probe = bytes(range(256))
        source.write(probe)
        received = bytearray()
        while len(received) < len(probe):
            block = client.recv(len(probe) - len(received))
            assert block, "connection closed"
            received.extend(block)
        assert received == probe


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("host")
    ap.add_argument("--feed", required=True)
    ap.add_argument("--input", choices=["zander", "flarm"], required=True)
    args = ap.parse_args()
    host = socket.gethostbyname(args.host)
    port = int(args.input == "flarm")
    pw = password()
    url = "/api/nav-scan"

    def api(data=None, expected=200):
        code, body = request(host, url, data, pw)
        assert code == expected, (code, expected)
        return json.loads(body) if code == 200 else None

    before = {i["key"]: i["value"] for i in settings(host)["items"]}
    active_rate = int(before[f"b{port}"])
    api({"port": 2}, 400)
    current = api({"port": port})
    api({"port": 1 - port}, 409)
    api({"cancel": current["id"] + 1}, 409)
    api({"cancel": current["id"]})
    time.sleep(0.4)
    assert api()["state"] == "cancelled"
    print("PASS authentication, port restrictions, single scan and cancellation", flush=True)

    zander = sentence("PZAN1,00420,000001")
    flarm = sentence("PFLAU,0,1,1,1,0,,0,,,")
    gps = sentence("GPRMC,123456,V,,,,,,,010101,,,N")
    cases = [
        (4800, zander, "found", {"zander"}),
        (19200, flarm, "found", {"flarm"}),
        (38400, gps + zander + flarm, "found", {"generic", "zander", "flarm"}),
        (9600, gps, "found", {"generic"}),
        (active_rate, b"noise\x00" + b"$PZAN1,1,2*FF\r\n" + sentence("PFLAU,1,2"), "undecoded", set()),
        (active_rate, b"", "quiet", set()),
    ]
    with serial.Serial(args.feed, baudrate=active_rate, write_timeout=2) as source:
        # Leave the unplugged input scanning without browser polls while the wired input carries data.
        scan = api({"port": 1 - port})
        verify_forwarding(host, int(before[f"t{port}"]), source)
        time.sleep(26)
        completed = api()
        assert completed["id"] == scan["id"] and completed["state"] in ("quiet", "undecoded"), completed
        print("PASS other input forwards during scan and unattended scan expires", flush=True)
        for rate, payload, state, protocols in cases:
            source.baudrate = rate
            start = time.monotonic()
            scan = api({"port": port})
            while time.monotonic() - start < 60:
                if payload:
                    # Split a line across reads on the ESP32.
                    source.write(payload[:4])
                    time.sleep(0.025)
                    source.write(payload[4:])
                time.sleep(0.24)
                result = api()
                assert result["id"] == scan["id"]
                if result["state"] not in ("listening", "measuring"):
                    break
            assert result["state"] == state, result
            if state == "found":
                assert result["baud"] == rate and result["frame"] == "8N1", result
                assert {key for key in ("generic", "flarm", "zander") if result[key] >= 3} == protocols, result
            print(f"PASS {rate} baud {state} {sorted(protocols)} in {time.monotonic() - start:.1f}s", flush=True)
        # Byte-exact restoration, before offering a result to the settings form.
        source.baudrate = active_rate
        verify_forwarding(host, int(before[f"t{port}"]), source)
    assert before == {i["key"]: i["value"] for i in settings(host)["items"]}
    print("PASS forwarding restored and saved settings preserved")


if __name__ == "__main__":
    main()
