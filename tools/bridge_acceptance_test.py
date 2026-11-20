"""Step 5 bench acceptance tests for the bridge firmware: slow clients.

    python tools/bridge_acceptance_test.py 192.168.1.50 --baseline
    python tools/bridge_acceptance_test.py 192.168.1.50
    python tools/bridge_acceptance_test.py 192.168.1.50 --radio-loop --seconds 120

For the bare ESP32 only, before the converter boards are wired. Never fit
GPIO jumpers to the assembled bridge. Its bus-level procedure is in
docs/hardware/wiring.md#bus-test. This script needs the step 2 jumper
loops (GPIO33 -> GPIO26, GPIO27 -> GPIO32, and GPIO17 -> GPIO16 with
--radio-loop), see tools/bridge_loop_test.py and
docs/project/roadmap.md#firmware-plan.

Streams NMEA-like test data into the Zander port at about 80 % of the line
rate. The loop carries it to the FLARM UART, and the bridge forwards it to
the apps on the FLARM port. Two apps are connected there:

- a normal app that reads everything, and
- a slow app that connects with a tiny receive window and then never reads,
  like a phone with OpenSoar in the background.

With --baseline the slow app is left out, for comparison.

The test passes when the bridge keeps taking data on the Zander port, the
normal app gets every byte unchanged and no data is delayed by more than
--max-delay seconds. The slow app skips data but stays connected; only a
run longer than 60 s without reading closes it (firmware APP_STALL_CLOSE_MS),
so with --seconds 30 the check expects it open, with --seconds 90 closed. With --radio-loop it also
sends a short frame through the radio loop every half second and checks
that its round trip stays below --max-delay, so a stall on one port is
seen on the others too.

Prints the bridge's short-write and UART error counters before and after.
"""

import argparse
import json
import random
import socket
import sys
import threading
import time
import urllib.request

ZANDER_TCP, FLARM_TCP, RADIO_TCP = 10110, 2000, 4352
BAUD = 19200
CHUNK = 64  # bytes per write into the Zander port
STALL_CLOSE_S = 60  # firmware APP_STALL_CLOSE_MS: an app that takes no data for this long is closed


def tcp(host: str, port: int, rcvbuf: int = 0) -> socket.socket:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    if rcvbuf:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, rcvbuf)  # before connect, so the window stays small
    sock.settimeout(5)
    sock.connect((host, port))
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    return sock


def drain(sock: socket.socket) -> None:
    sock.settimeout(0.3)
    try:
        while sock.recv(4096):
            pass
    except (socket.timeout, BlockingIOError):
        pass


def status(host: str) -> dict:
    with urllib.request.urlopen(f"http://{host}/api/status", timeout=5) as reply:
        return {p["name"]: p for p in json.load(reply)["ports"]}


def counters(host: str) -> str:
    try:
        ports = status(host)
    except OSError as error:
        return f"status page not reachable ({error})"
    return ", ".join(
        f"{name}: {p['tcp_clients']} apps, {p['short_writes']} short writes, {p['uart_errors']} UART errors"
        for name, p in ports.items()
    )


def test_data(size: int) -> bytes:
    # NMEA-like lines with a running number, so a gap or a swap is visible in a dump.
    rng = random.Random(5)
    lines, n = [], 0
    while sum(map(len, lines)) < size:
        body = f"PTST,{n:06d},{rng.randrange(10**8):08d}"
        check = 0
        for c in body.encode():
            check ^= c
        lines.append(f"${body}*{check:02X}\r\n".encode())
        n += 1
    return b"".join(lines)[:size]


class Reader(threading.Thread):
    """Reads one socket and records when each byte offset arrived."""

    def __init__(self, sock: socket.socket):
        super().__init__(daemon=True)
        self.sock, self.data, self.arrivals, self.stop = sock, bytearray(), [], False

    def run(self) -> None:
        self.sock.settimeout(0.2)
        while not self.stop:
            try:
                chunk = self.sock.recv(4096)
            except socket.timeout:
                continue
            except OSError:
                break
            if not chunk:
                break
            self.data += chunk
            self.arrivals.append((len(self.data), time.monotonic()))


class RadioPinger(threading.Thread):
    """Sends a short frame through the radio loop every half second and records the round trip."""

    def __init__(self, sock: socket.socket):
        super().__init__(daemon=True)
        self.sock, self.round_trips, self.lost, self.stop = sock, [], 0, False
        self.completed = False
        self.error = ""

    def run(self) -> None:
        n = 0
        while True:
            last_probe = self.stop
            frame = bytes([0xA5, 0x00, n & 0xFF, 0x5A])
            start = time.monotonic()
            got = bytearray()
            try:
                self.sock.settimeout(10)
                self.sock.sendall(frame)
                while len(got) < len(frame):
                    remaining = 10 - (time.monotonic() - start)
                    if remaining <= 0:
                        raise TimeoutError("radio echo timed out")
                    self.sock.settimeout(remaining)
                    chunk = self.sock.recv(len(frame) - len(got))
                    if not chunk:
                        raise ConnectionError("radio connection closed before echo completed")
                    got += chunk
            except OSError as error:
                self.lost += 1
                self.error = str(error)
                return
            if bytes(got) != frame:
                self.lost += 1
            else:
                self.round_trips.append(time.monotonic() - start)
            if last_probe:
                self.completed = True
                return
            n += 1
            time.sleep(max(0.0, 0.5 - (time.monotonic() - start)))


def run(args: argparse.Namespace) -> bool:
    print(f"before: {counters(args.host)}")
    source = tcp(args.host, ZANDER_TCP)
    reader_sock = tcp(args.host, FLARM_TCP)
    for sock in (source, reader_sock):
        drain(sock)

    slow = None
    if not args.baseline:
        slow = tcp(args.host, FLARM_TCP, rcvbuf=1024)  # never read from here on
        print("slow app connected to the FLARM port; it never reads")

    reader = Reader(reader_sock)
    reader.start()
    pinger = None
    if args.radio_loop:
        radio = tcp(args.host, RADIO_TCP)
        drain(radio)
        pinger = RadioPinger(radio)
        pinger.start()

    rate = BAUD / 10 * 0.8  # bytes per second, 80 % of the line rate
    data = test_data(int(rate * args.seconds))
    sent_at, longest_send, stalled = [], 0.0, False
    source.settimeout(30)
    start = time.monotonic()
    for offset in range(0, len(data), CHUNK):
        due = start + offset / rate
        time.sleep(max(0.0, due - time.monotonic()))
        t0 = time.monotonic()
        try:
            source.sendall(data[offset : offset + CHUNK])
        except socket.timeout:
            stalled = True
            print(f"bridge took no data on the Zander port for 30 s, after {offset} bytes")
            data = data[:offset]
            break
        longest_send = max(longest_send, time.monotonic() - t0)
        sent_at.append((min(offset + CHUNK, len(data)), time.monotonic()))
    ok = not stalled and longest_send <= args.max_delay
    print(f"{'PASS' if ok else 'FAIL'} bridge takes data on the Zander port: longest blocked write {longest_send:.2f} s")
    send_ok = ok

    deadline = time.monotonic() + args.max_delay + 2
    while len(reader.data) < len(data) and time.monotonic() < deadline:
        time.sleep(0.1)
    reader.stop = True
    if pinger:
        pinger.stop = True
        # A probe already in flight and the final probe each have a 10 s deadline.
        pinger.join(timeout=22)
    reader.join(timeout=1)

    results = [send_ok]
    got = bytes(reader.data)
    if got == data:
        print(f"PASS normal app on the FLARM port: {len(data)} bytes identical")
        results.append(True)
    else:
        first = next((i for i, (a, b) in enumerate(zip(data, got)) if a != b), min(len(data), len(got)))
        print(f"FAIL normal app on the FLARM port: sent {len(data)}, received {len(got)}, first difference at byte {first}")
        results.append(False)

    # Delay of each written chunk until its last byte reached the normal app.
    delays, i = [], 0
    for end, t_sent in sent_at:
        while i < len(reader.arrivals) and reader.arrivals[i][0] < end:
            i += 1
        if i < len(reader.arrivals):
            delays.append(reader.arrivals[i][1] - t_sent)
    worst = max(delays) if delays else float("inf")
    ok = len(delays) == len(sent_at) and worst <= args.max_delay
    print(f"{'PASS' if ok else 'FAIL'} data delay on the FLARM port: worst {worst:.2f} s, limit {args.max_delay:.1f} s")
    results.append(ok)

    if pinger:
        worst_rt = max(pinger.round_trips) if pinger.round_trips else float("inf")
        ok = pinger.completed and not pinger.is_alive() and pinger.lost == 0 and worst_rt <= args.max_delay
        print(f"{'PASS' if ok else 'FAIL'} radio loop during the test: {len(pinger.round_trips)} frames, "
              f"{pinger.lost} lost, worst round trip {worst_rt:.2f} s")
        if not pinger.completed:
            print(f"radio check incomplete: {pinger.error or 'final echo was not confirmed'}")
        results.append(ok)

    if slow is not None:
        # The slow app's connection must stay open while it skips data, and close only
        # after it took no data for STALL_CLOSE_S (firmware APP_STALL_CLOSE_MS).
        expect_open = args.seconds < STALL_CLOSE_S - 5
        slow.settimeout(2)
        received, closed = 0, False
        try:
            while True:
                chunk = slow.recv(65536)
                if not chunk:
                    closed = True
                    break
                received += len(chunk)
        except socket.timeout:
            pass
        except OSError:
            closed = True
        ok = closed != expect_open
        state = "closed by the bridge" if closed else "open"
        print(f"{'PASS' if ok else 'FAIL'} slow app after {args.seconds:.0f} s without reading: {state}, "
              f"{received} bytes waiting (expected {'open' if expect_open else 'closed'})")
        results.append(ok)

    print(f"after:  {counters(args.host)}")
    for sock in (source, reader_sock, slow):
        if sock:
            sock.close()
    if pinger:
        pinger.sock.close()
    print(f"{sum(results)}/{len(results)} checks passed")
    return all(results)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("host", help="bridge address, 10.10.10.10 in hotspot mode")
    parser.add_argument("--baseline", action="store_true", help="leave out the slow app")
    parser.add_argument("--radio-loop", action="store_true", help="GPIO17 is looped to GPIO16; also time the radio port")
    parser.add_argument("--seconds", type=float, default=60, help="test length (default 60)")
    parser.add_argument("--max-delay", type=float, default=2.0, help="largest allowed delay in seconds (default 2)")
    return 0 if run(parser.parse_args()) else 1


if __name__ == "__main__":
    sys.exit(main())
