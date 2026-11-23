"""Radio port robustness checks on the wired bench bridge (WP6 step 5).

    python tools/radio_robustness_test.py <bridge address>
    python tools/radio_robustness_test.py <bridge address> --refused 300

Needs the assembled bridge with the DE-9 radio loop (links 4-8 and 5-9,
docs/hardware/wiring.md#bus-test), instruments disconnected and no other app
on the radio port. Checks:

 1. single owner: a second client is refused while the first one works
 2. stalled owner: an owner that sends but never reads is closed by the
    bridge, and the next owner receives no stale bytes
 3. owner leaves mid-echo: an owner that sends a burst and closes at once;
    the next owner receives none of that echo
 4. refused clients: many refused connection attempts while an owner
    works; the owner keeps its echo, the bridge does not restart, and the
    lowest free memory from the bridge log is reported

The dead-owner takeover (Wi-Fi cut, 45 s) stays a manual check: see
firmware/bridge/README.md#slow-app-check.
"""

import argparse
import random
import socket
import sys
import time

from bridge_acceptance_test import HEAP_LINE, get_json, status, tcp

results = []


def check(name, ok, detail=""):
    results.append(ok)
    print(f"{'PASS' if ok else 'FAIL'} {name}{': ' + detail if detail else ''}")


def radio_port(host):
    return status(host)["Becker"]["tcp_port"]


def accepted(sock, wait=1.0):
    """A refused client is closed by the bridge at once; an owner stays open."""
    sock.settimeout(wait)
    try:
        return sock.recv(1, socket.MSG_PEEK) != b""
    except socket.timeout:
        return True
    except OSError:
        return False


def owner(host, port, attempts=40, **kw):
    """Connects until the bridge accepts this client as the radio owner."""
    for _ in range(attempts):
        sock = tcp(host, port, **kw)
        if accepted(sock):
            return sock
        sock.close()
        time.sleep(0.5)
    raise ConnectionError("the radio port did not accept a new owner")


def echo(sock, data, timeout=5.0):
    sock.sendall(data)
    got = bytearray()
    end = time.monotonic() + timeout
    while len(got) < len(data) and time.monotonic() < end:
        sock.settimeout(max(0.05, end - time.monotonic()))
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            break
        if not chunk:
            break
        got += chunk
    return bytes(got) == data


def stray_bytes(sock, seconds=2.0):
    """Bytes a fresh owner receives without sending anything."""
    sock.settimeout(0.2)
    count, end = 0, time.monotonic() + seconds
    while time.monotonic() < end:
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            continue
        if not chunk:
            break
        count += len(chunk)
    return count


def closed_by_bridge(sock, seconds):
    """Reads (and discards) until the bridge closes the connection or time runs out."""
    end = time.monotonic() + seconds
    while time.monotonic() < end:
        sock.settimeout(0.5)
        try:
            if not sock.recv(65536):
                return True
        except socket.timeout:
            continue
        except OSError:
            return True
    return False


def boot(host):
    return get_json(host, "/api/log")["boot"]


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("host")
    ap.add_argument("--refused", type=int, default=200, help="refused connection attempts in check 4 (default 200)")
    a = ap.parse_args(argv)
    host = a.host
    rng = random.Random(11)
    port = radio_port(host)
    if status(host)["Becker"]["tcp_clients"]:
        raise SystemExit("FAIL disconnect other apps from the radio port first")
    baud = status(host)["Becker"]["baud"]
    start_boot = boot(host)
    log_seq = get_json(host, "/api/log")["seq"]

    # 1. Single owner
    first = owner(host, port)
    with first:
        second = tcp(host, port)
        with second:
            check("second client refused while an owner is connected", not accepted(second, 2))
        check("owner keeps its echo", echo(first, bytes(range(256))))

    # 2. Stalled owner: send several times what the socket buffers and the app queue hold
    # (about 10 kB) at the radio line rate, never read, then expect the bridge to close it.
    stalled = owner(host, port, rcvbuf=1024)
    with stalled:
        payload = bytes(rng.randrange(256) for _ in range(20000))
        stalled.settimeout(1)
        sent, deadline = 0, time.monotonic() + len(payload) / (baud / 10) * 2 + 20
        closed = False
        while sent < len(payload) and time.monotonic() < deadline:
            try:
                sent += stalled.send(payload[sent:sent + 512])
            except socket.timeout:
                continue
            except OSError:
                closed = True
                break
            time.sleep(512 / (baud / 10))  # about the radio line rate
        # The echo still arrives for a few seconds after the last send; reading now would
        # empty the socket and rescue the owner before the bridge's queue overflows.
        if not closed:
            time.sleep(10)
        closed = closed or closed_by_bridge(stalled, 15)
        check("owner that never reads is closed by the bridge", closed, f"{sent} bytes sent")
    nxt = owner(host, port)
    with nxt:
        stray = stray_bytes(nxt)
        check("next owner receives no stale bytes after a stalled owner", stray == 0, f"{stray} bytes")

        # 3. Owner leaves mid-echo
        nxt.sendall(bytes(rng.randrange(256) for _ in range(600)))
    time.sleep(0.05)
    last = owner(host, port)
    with last:
        stray = stray_bytes(last)
        check("next owner receives none of the previous owner's echo", stray == 0, f"{stray} bytes")

        # 4. Refused clients while the owner works
        failed_echo, t0 = 0, time.monotonic()
        for n in range(a.refused):
            try:
                with tcp(host, port) as extra:
                    accepted(extra, 0.3)
            except OSError:
                pass
            if n % 20 == 0 and not echo(last, bytes(rng.randrange(256) for _ in range(rng.randint(1, 64)))):
                failed_echo += 1
        check(f"owner keeps its echo during {a.refused} refused connections", failed_echo == 0,
              f"{failed_echo} failed echoes in {time.monotonic() - t0:.0f} s")

    log = get_json(host, f"/api/log?since={log_seq}")
    lowest = [int(m.group(2)) for line in log["lines"] if (m := HEAP_LINE.search(line["m"]))]
    check("bridge did not restart", log["boot"] == start_boot,
          f"lowest free memory {min(lowest)} kB" if lowest else "no memory line in the log yet")
    print(f"{sum(results)}/{len(results)} checks passed")
    return 0 if all(results) else 1


if __name__ == "__main__":
    sys.exit(main())
