"""Step 5 bench acceptance tests for the bridge firmware: slow clients.

    python tools/bridge_acceptance_test.py 192.168.1.50 --baseline
    python tools/bridge_acceptance_test.py 192.168.1.50
    python tools/bridge_acceptance_test.py 192.168.1.50 --radio-loop --seconds 120

    python tools/bridge_acceptance_test.py 192.168.1.50 --feed COM4 --input flarm --seconds 30
    python tools/bridge_acceptance_test.py 192.168.1.50 --zander-feed COM4 --flarm-feed COM5 --radio-loop --seconds 3600

With serial feeds, use the assembled bridge at the DE-9 with instruments
disconnected, RS-232-level sources and (with --radio-loop) links 4-8 and
5-9. See docs/hardware/wiring.md#bus-test. Two feeds need independent PC
serial adapters. Wired mode needs pyserial and uses the live settings.
Run with the PC test clients alone. The slow client uses the first wired
input (Zander when both are fed).

Without serial feeds, use the bare ESP32 and the step 2 jumper loops
(GPIO33 -> GPIO26, GPIO27 -> GPIO32, and GPIO17 -> GPIO16 with
--radio-loop). Never fit GPIO jumpers to the assembled bridge.

In bare-board mode, streams NMEA-like data into the Zander port at about 80 % of the line
rate. The loop carries it to the FLARM UART, and the bridge forwards it to
the apps on the FLARM port. Two apps are connected there:

- a normal app that reads everything, and
- a slow app that connects with a tiny receive window and then never reads,
  like a phone with OpenSoar in the background.

With --baseline the slow app is left out, for comparison.

The test passes when the bridge keeps taking data on the Zander port, the
normal app gets every byte unchanged and no data is delayed by more than
--max-delay seconds. In wired mode, use --seconds 30 for the short test:
the slow app must skip data and stay connected. Use --seconds 120 or longer
to observe sustained backpressure: the firmware log must confirm closure
after 60 s without progress. A connected slow app in a sustained test
leaves the closure check inconclusive.

With --radio-loop, the test also sends a burst of 1-64 random bytes (every
byte value) every half second and checks that each comes back unchanged,
that a final echo is confirmed and that nothing follows it. All round trips
must stay within --max-delay.

In wired mode the test also reads the bridge log every minute: the bridge
must not restart, and the lowest free memory it reports is printed. With
--supply it reads the supply-voltage port and expects about one valid
$POV sentence per second.

Prints the bridge's short-write and UART error counters before and after.
"""

import argparse
from contextlib import ExitStack, closing
import json
import math
from queue import Empty, Full, Queue
import random
import re
import socket
import sys
import threading
import time
import urllib.request

ZANDER_TCP, FLARM_TCP, RADIO_TCP, SUPPLY_TCP = 10110, 2000, 4352, 4353
BAUD = 19200
CHUNK = 64  # bytes per write into the Zander port
STALL_CLOSE_S = 60  # firmware APP_STALL_CLOSE_MS: an app that takes no data for this long is closed
BURST_MAX = 64  # largest radio probe burst in bytes
HEAP_LINE = re.compile(r"Memory free (\d+) kB, lowest (\d+) kB")


def tcp(host: str, port: int, rcvbuf: int = 0) -> socket.socket:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        if rcvbuf:
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, rcvbuf)  # before connect
        sock.settimeout(5)
        sock.connect((host, port))
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    except OSError:
        sock.close()
        raise
    return sock


def drain(sock: socket.socket) -> None:
    sock.settimeout(0.3)
    try:
        while sock.recv(4096):
            pass
    except (socket.timeout, BlockingIOError):
        pass


def get_json(host: str, path: str) -> dict:
    with urllib.request.urlopen(f"http://{host}{path}", timeout=5) as reply:
        return json.load(reply)


def status(host: str) -> dict:
    return {p["name"]: p for p in get_json(host, "/api/status")["ports"]}


def counters(host: str) -> str:
    try:
        ports = status(host)
    except OSError as error:
        return f"status page not reachable ({error})"
    return ", ".join(
        f"{name}: {p['tcp_clients']} apps, {p['short_writes']} short writes, {p['uart_errors']} UART errors"
        for name, p in ports.items()
    )


def test_chunks(size: int, port_id: int = 0):
    # Include the input and a sequence number to detect gaps and swapped inputs.
    rng = random.Random(5 + port_id)
    pending, n = bytearray(), 0
    while size:
        body = f"PTST,{port_id},{n:07d},{rng.randrange(10**8):08d}"
        check = 0
        for c in body.encode():
            check ^= c
        pending.extend(f"${body}*{check:02X}\r\n".encode())
        n += 1
        count = min(size, CHUNK)
        if len(pending) >= count:
            yield bytes(pending[:count])
            del pending[:count]
            size -= count


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
    """Sends a random burst through the radio loop every half second and records the round trip.

    Bursts are 1-BURST_MAX bytes and cover every byte value, including 0x00 and 0xA5.
    After the final echo, any further byte is an error (duplicated or late data)."""

    def __init__(self, sock: socket.socket, seed: int = 7):
        super().__init__(daemon=True)
        self.sock, self.round_trips, self.lost, self.stop = sock, [], 0, False
        self.completed = False
        self.error = ""
        self.sent = 0
        self.rng = random.Random(seed)

    def burst(self) -> bytes:
        size = self.rng.randint(1, BURST_MAX)
        return bytes(self.rng.randrange(256) for _ in range(size))

    def run(self) -> None:
        while True:
            last_probe = self.stop
            frame = self.burst()
            start = time.monotonic()
            got = bytearray()
            try:
                self.sock.settimeout(10)
                self.sock.sendall(frame)
                self.sent += len(frame)
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
                if last_probe:
                    self.error = "final radio echo differed"
                    return
            else:
                self.round_trips.append(time.monotonic() - start)
            if last_probe:
                self.check_trailing()
                return
            time.sleep(max(0.0, 0.5 - (time.monotonic() - start)))

    def check_trailing(self) -> None:
        """After the final echo, nothing more may arrive."""
        self.sock.settimeout(0.5)
        try:
            extra = self.sock.recv(4096)
        except socket.timeout:
            extra = None
        except OSError as error:
            self.error = f"radio connection failed after the final echo: {error}"
            return
        if extra:
            self.lost += 1
            self.error = f"{len(extra)} unexpected bytes after the final echo"
            return
        self.completed = True


class SupplyReader(threading.Thread):
    """Reads the supply-voltage port and counts valid $POV,V sentences."""

    def __init__(self, sock: socket.socket):
        super().__init__(daemon=True)
        self.sock, self.valid, self.invalid, self.stop, self.error = sock, 0, 0, False, ""

    @staticmethod
    def valid_sentence(line: bytes) -> bool:
        match = re.fullmatch(rb"\$(POV,V,[0-9.]+)\*([0-9A-F]{2})", line.strip())
        if not match:
            return False
        check = 0
        for c in match.group(1):
            check ^= c
        return check == int(match.group(2), 16)

    def run(self) -> None:
        self.sock.settimeout(0.5)
        pending = b""
        while not self.stop:
            try:
                chunk = self.sock.recv(1024)
            except socket.timeout:
                continue
            except OSError as error:
                self.error = str(error)
                return
            if not chunk:
                self.error = "supply port closed"
                return
            pending += chunk
            *lines, pending = pending.split(b"\n")
            for line in lines:
                if self.valid_sentence(line):
                    self.valid += 1
                else:
                    self.invalid += 1


def bare_slow_app_result(seconds, slow_age, closed, received, before, after, restarted):
    """Verdict for the bare-board slow app: open and skipping before the stall limit,
    closed by the stall rule (not by a restart) after it."""
    skipped = after["short_writes"] - before["short_writes"]
    expect_open = slow_age < STALL_CLOSE_S - 5
    if restarted:
        ok, detail = False, "the bridge restarted"
    elif skipped <= 0:
        ok, detail = False, "the slow app never fell behind (no short writes)"
    elif expect_open:
        ok, detail = not closed, "open while skipping data" if not closed else "closed too early"
    else:
        ok, detail = closed, "closed after the stall limit" if closed else "still open after the stall limit"
    print(f"{'PASS' if ok else 'FAIL'} slow app after {seconds:.0f} s without reading: {detail}, "
          f"{skipped} short writes, {received} bytes waiting")
    return ok


def run(args: argparse.Namespace) -> bool:
    with ExitStack() as resources:
        return run_bare(args, resources)


def run_bare(args: argparse.Namespace, resources: ExitStack) -> bool:
    print(f"before: {counters(args.host)}")
    source = resources.enter_context(closing(tcp(args.host, ZANDER_TCP)))
    reader_sock = resources.enter_context(closing(tcp(args.host, FLARM_TCP)))
    for sock in (source, reader_sock):
        drain(sock)

    slow = slow_before = None
    if not args.baseline:
        slow_before = status(args.host)["FLARM"]
        boot_before = get_json(args.host, "/api/log")["boot"]
        slow = resources.enter_context(closing(tcp(args.host, FLARM_TCP, rcvbuf=1024)))  # never read from here on
        slow_start = time.monotonic()
        print("slow app connected to the FLARM port; it never reads")

    reader = Reader(reader_sock)
    reader.start()
    pinger = None
    if args.radio_loop:
        radio = resources.enter_context(closing(tcp(args.host, RADIO_TCP)))
        drain(radio)
        pinger = RadioPinger(radio)
        pinger.start()

    rate = BAUD / 10 * 0.8  # bytes per second, 80 % of the line rate
    data = b"".join(test_chunks(int(rate * args.seconds)))
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
        # after it took no data for STALL_CLOSE_S (firmware APP_STALL_CLOSE_MS). Reading
        # counts as progress, so for the close check wait well past the limit first.
        if args.seconds >= STALL_CLOSE_S - 5:
            time.sleep(max(0.0, slow_start + STALL_CLOSE_S + 10 - time.monotonic()))
        slow_age = time.monotonic() - slow_start
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
        restarted = get_json(args.host, "/api/log")["boot"] != boot_before
        results.append(bare_slow_app_result(args.seconds, slow_age, closed, received, slow_before,
                                            status(args.host)["FLARM"], restarted))

    print(f"after:  {counters(args.host)}")
    print(f"{sum(results)}/{len(results)} checks passed")
    return all(results)


class WiredInput:
    """A paced serial feed and an independent byte-for-byte TCP reader."""

    def __init__(self, name, sock, source, port, frame, args, halt):
        self.name, self.sock, self.source, self.halt = name, sock, source, halt
        frame_bits = 1 + int(frame[0]) + (frame[1] != "N") + int(frame[2])
        self.rate = port["baud"] / frame_bits * 0.8
        self.port_id, self.seconds, self.max_delay = port["uart"], args.seconds, args.max_delay
        self.pending = Queue(maxsize=max(1, math.ceil(self.rate * self.max_delay / CHUNK) + 1))
        self.sent = self.received = 0
        self.worst_delay = 0.0
        self.error = ""

    def fail(self, message):
        self.error = self.error or message
        self.halt.set()

    def send(self, start):
        for payload in test_chunks(int(self.rate * self.seconds), self.port_id):
            due = start + self.sent / self.rate
            if self.halt.wait(max(0.0, due - time.monotonic())):
                return
            try:
                self.pending.put_nowait((payload, time.monotonic()))
                if self.source.write(payload) != len(payload):
                    self.fail(f"short serial write at byte {self.sent}")
                    return
            except Full:
                self.fail(f"TCP reader fell behind at byte {self.received}")
                return
            except OSError as error:
                self.fail(f"serial write failed at byte {self.sent}: {error}")
                return
            self.sent += len(payload)

    def receive(self, finish):
        self.sock.settimeout(0.1)
        expected, sent_at = b"", 0.0
        while not finish.is_set() and not self.halt.is_set():
            try:
                block = self.sock.recv(4096)
                arrived = time.monotonic()
            except socket.timeout:
                continue
            except OSError as error:
                self.fail(f"TCP reader failed: {error}")
                return
            if not block:
                self.fail("TCP reader disconnected")
                return
            while block:
                if not expected:
                    try:
                        expected, sent_at = self.pending.get_nowait()
                    except Empty:
                        self.fail(f"unexpected data at byte {self.received}")
                        return
                count = min(len(expected), len(block))
                if expected[:count] != block[:count]:
                    first = next(i for i in range(count) if expected[i] != block[i])
                    self.fail(f"data differs at byte {self.received + first}")
                    return
                self.received += count
                block, expected = block[count:], expected[count:]
                if not expected:
                    self.worst_delay = max(self.worst_delay, arrived - sent_at)
                    if self.worst_delay > self.max_delay:
                        self.fail(f"delay {self.worst_delay:.3f} s exceeds {self.max_delay:g} s")
                        return


def slow_app_result(name, elapsed, before, after, closes, log_ok):
    skipped = after["short_writes"] - before["short_writes"]
    clients = after["tcp_clients"]
    if not log_ok:
        verdict, detail = "FAIL", "bridge log evidence incomplete"
    elif clients == 1 and len(closes) == 1 and closes[0] >= STALL_CLOSE_S and skipped > 0:
        verdict, detail = "PASS", "firmware closed after 60 s without progress"
    elif clients != 2 or closes:
        verdict, detail = "FAIL", "premature or unexpected disconnect"
    elif skipped <= 0:
        verdict, detail = "INCONCLUSIVE", "the slow client did not fall behind"
    elif elapsed > STALL_CLOSE_S:
        verdict, detail = "INCONCLUSIVE", "still connected; firmware did not report sustained no-progress closure"
    else:
        verdict, detail = "PASS", "still connected while skipping data"
    print(f"{verdict} {name} slow app: {detail} after {elapsed:.1f} s, "
          f"{skipped} short writes, {clients} apps, {len(closes)} stall closures")
    return verdict == "PASS"


def run_wired(args, feeds):
    import serial

    before = status(args.host)
    frames = {item["key"]: item["value"] for item in get_json(args.host, "/api/settings")["items"]}
    names = ("Zander", "FLARM", "Becker") if args.radio_loop else ("Zander", "FLARM")
    if any(before[name]["tcp_clients"] for name in names):
        raise ValueError("disconnect other test clients before starting")
    slow_name = None if args.baseline else next(iter(feeds))
    finish, halt = threading.Event(), threading.Event()
    closes, log_ok = [], True
    heap_lowest = []  # lowest free memory in kB, from the bridge's minute Debug lines
    with ExitStack() as resources:
        inputs = {}
        for name in ("Zander", "FLARM"):
            port = before[name]
            frame = frames[f"f{port['uart']}"]
            sock = resources.enter_context(tcp(args.host, port["tcp_port"]))
            source = None
            if name in feeds:
                if int(frame[0]) < 7:
                    raise ValueError(f"{name}: synthetic NMEA requires at least 7 data bits")
                source = resources.enter_context(serial.Serial(
                    feeds[name], baudrate=port["baud"], bytesize=int(frame[0]),
                    parity=frame[1], stopbits=int(frame[2]), write_timeout=args.max_delay))
                print(f"{name}: {feeds[name]}, {port['baud']} {frame}, TCP {port['tcp_port']}")
            inputs[name] = WiredInput(name, sock, source, port, frame, args, halt)
            if source is not None and int(inputs[name].rate * args.seconds) < CHUNK:
                raise ValueError(f"{name}: increase --seconds to send a complete {CHUNK}-byte test chunk")
        log = get_json(args.host, "/api/log")
        boot, seq, bridge_start = log["boot"], log["seq"], log["now"]
        if slow_name:
            slow_start = time.monotonic()
            resources.enter_context(tcp(args.host, before[slow_name]["tcp_port"], rcvbuf=1024))
            print(f"slow app connected to {slow_name}; it never reads")

        pinger = None
        if args.radio_loop:
            radio_port = before["Becker"]
            radio_frame = frames[f"f{radio_port['uart']}"]
            if int(radio_frame[0]) != 8:
                raise ValueError("Becker: binary loop test requires 8 data bits")
            radio = resources.enter_context(tcp(args.host, radio_port["tcp_port"]))
            print(f"Becker: DE-9 loop, {radio_port['baud']} {radio_frame}, TCP {radio_port['tcp_port']}")
            pinger = RadioPinger(radio)

        supply = None
        if args.supply:
            supply = SupplyReader(resources.enter_context(closing(tcp(args.host, SUPPLY_TCP))))

        start = time.monotonic()
        readers, writers = {}, []
        for stream in inputs.values():
            reader = threading.Thread(target=stream.receive, args=(finish,), daemon=True)
            reader.start()
            readers[stream.name] = reader
        for name in feeds:
            writer = threading.Thread(target=inputs[name].send, args=(start,), daemon=True)
            writer.start()
            writers.append(writer)
        if pinger:
            pinger.start()
        if supply:
            supply.start()

        def update_log():
            nonlocal log_ok, seq
            seq_before = seq
            log = get_json(args.host, f"/api/log?since={seq}")
            log_ok = log_ok and log["boot"] == boot and not log["lost"] and log["seq"] >= seq
            seq = log["seq"]
            for line in log["lines"]:
                if seq_before < line["q"] <= log["seq"] and (match := HEAP_LINE.search(line["m"])):
                    heap_lowest.append(int(match.group(2)))
            closes.extend(
                (line["t"] - bridge_start) / 1000 for line in log["lines"]
                if seq_before < line["q"] <= log["seq"] and line["m"].startswith(f"{slow_name}: app ")
                and line["m"].endswith(f" took no data for {STALL_CLOSE_S} s, closed")
            )

        try:
            deadline = start + args.seconds
            log_due = start + 60
            while not halt.wait(max(0, min(2, deadline - time.monotonic()))):
                if slow_name or time.monotonic() >= log_due:
                    update_log()
                    log_due = time.monotonic() + 60
                if time.monotonic() >= deadline:
                    break
                if pinger and (not pinger.is_alive() or pinger.lost
                               or (pinger.round_trips and pinger.round_trips[-1] > args.max_delay)):
                    halt.set()
                    break
            for writer in writers:
                writer.join(timeout=args.max_delay + 1)

            # Keep observing both receivers after the final write to catch trailing bytes.
            halt.wait(args.max_delay)
            if pinger:
                pinger.stop = True
                pinger.join(timeout=22)
            after = status(args.host)
            if slow_name:
                slow_elapsed = time.monotonic() - slow_start
            for name, reader in readers.items():
                if not reader.is_alive() and not halt.is_set():
                    inputs[name].fail("TCP reader stopped early")
            update_log()
        finally:
            if supply:
                supply.stop = True
                supply.join(timeout=2)
            if pinger and not pinger.stop:
                pinger.stop = True
                pinger.join(timeout=22)
            finish.set()
            halt.set()
            for thread in writers + list(readers.values()):
                thread.join(timeout=1)

        results = []
        print(f"{'PASS' if log_ok else 'FAIL'} bridge log: {'no restart, no lost lines' if log_ok else 'restart or lost log lines'}"
              f"{f', lowest free memory {min(heap_lowest)} kB' if heap_lowest else ''}")
        results.append(log_ok)
        if supply:
            ran = time.monotonic() - start
            ok = not supply.error and supply.invalid == 0 and supply.valid >= 0.9 * ran - 2
            print(f"{'PASS' if ok else 'FAIL'} supply port: {supply.valid} valid, {supply.invalid} invalid sentences "
                  f"in {ran:.0f} s{', ' + supply.error if supply.error else ''}")
            results.append(ok)
        for name, stream in inputs.items():
            expected = int(stream.rate * stream.seconds) if name in feeds else 0
            errors = after[name]["uart_errors"] - before[name]["uart_errors"]
            ok = not stream.error and errors == 0 and stream.sent == expected == stream.received
            details = f"{stream.received}/{expected} bytes, worst delay {stream.worst_delay:.3f} s, {errors} UART errors"
            if stream.error:
                details += f", {stream.error}"
            print(f"{'PASS' if ok else 'FAIL'} {name}: {details}")
            results.append(ok)
        if slow_name:
            results.append(slow_app_result(slow_name, slow_elapsed, before[slow_name], after[slow_name], closes, log_ok))
        if pinger:
            worst = max(pinger.round_trips, default=float("inf"))
            ok = pinger.completed and not pinger.is_alive() and not pinger.lost and worst <= args.max_delay
            ok = ok and before["Becker"]["uart_errors"] == after["Becker"]["uart_errors"]
            print(f"{'PASS' if ok else 'FAIL'} Becker radio loop: {len(pinger.round_trips)} bursts, {pinger.sent} bytes, "
                  f"{pinger.lost} lost, worst round trip {worst:.3f} s, "
                  f"{'final echo confirmed' if pinger.completed and not pinger.error else pinger.error or 'final echo missing'}")
            results.append(ok)
    print(f"{sum(results)}/{len(results)} checks passed")
    return all(results)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("host", help="bridge address, 10.10.10.10 in hotspot mode")
    parser.add_argument("--baseline", action="store_true", help="leave out the slow app")
    parser.add_argument("--radio-loop", action="store_true", help="time the radio port with the appropriate physical loop fitted")
    parser.add_argument("--feed", help="PC serial port for one assembled-bridge RS-232 input")
    parser.add_argument("--input", choices=("zander", "flarm"), help="navigation input driven by --feed")
    parser.add_argument("--zander-feed", help="PC serial port for the Zander input of the assembled bridge")
    parser.add_argument("--flarm-feed", help="PC serial port for the FLARM input of the assembled bridge")
    parser.add_argument("--seconds", type=float, help="test length (wired default 30, bare-board default 60)")
    parser.add_argument("--supply", action="store_true", help="wired mode: also read the supply-voltage port 4353")
    parser.add_argument("--max-delay", type=float, default=2.0, help="largest allowed delay in seconds (default 2)")
    args = parser.parse_args(argv)
    if args.seconds is None:
        args.seconds = 30 if args.feed or args.zander_feed or args.flarm_feed else 60
    if not all(math.isfinite(value) and value > 0 for value in (args.seconds, args.max_delay)):
        parser.error("--seconds and --max-delay must be positive and finite")
    if bool(args.feed) != bool(args.input):
        parser.error("--feed and --input must be supplied together")
    feeds = {name: device for name, device in (("Zander", args.zander_feed), ("FLARM", args.flarm_feed)) if device}
    if args.feed:
        if feeds:
            parser.error("--feed/--input cannot be combined with --zander-feed or --flarm-feed")
        feeds = {"Zander" if args.input == "zander" else "FLARM": args.feed}
    if len({device.casefold() for device in feeds.values()}) != len(feeds):
        parser.error("each input requires an independent PC serial port")
    if not feeds and int(BAUD / 10 * 0.8 * args.seconds) == 0:
        parser.error("--seconds must be long enough to send at least one byte")
    try:
        return 0 if (run_wired(args, feeds) if feeds else run(args)) else 1
    except ModuleNotFoundError as error:
        if error.name != "serial":
            raise
        print("FAIL wired mode requires pyserial (python -m pip install pyserial)", file=sys.stderr)
    except (OSError, ValueError, KeyError) as error:
        print(f"FAIL {error}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
