"""Step 2 loop check for the bridge firmware.

    python tools/bridge_loop_test.py 10.10.10.10
    python tools/bridge_loop_test.py 192.168.1.50 --radio-loop
    python tools/bridge_loop_test.py 192.168.1.50 --radio-loop --radio-owner
    python tools/bridge_loop_test.py 192.168.1.50 --radio-only --radio-owner
    python tools/bridge_loop_test.py <bridge address> --feed COM5 --input zander

Needs the bench ESP32 running the bridge firmware and jumper wires
(firmware plan step 2, docs/project/roadmap.md#firmware-plan):

    GPIO33 (Zander TX) -> GPIO26 (FLARM RX)
    GPIO27 (FLARM TX)  -> GPIO32 (Zander RX)
    GPIO17 (Becker TX) -> GPIO16 (Becker RX), only with --radio-loop

Sends every byte value plus a pseudo-random block into one port and checks
that exactly the same bytes, in order, come out of the looped port, over
TCP in both directions for Zander and FLARM. With --radio-loop it also loops
the radio port over TCP. Last it checks that UDP sent to the former upstream
UDP ports reaches no device: the firmware has no UDP.

With --radio-owner, also check that a second client is refused, its bytes do
not reach the UART, navigation keeps flowing, and a clean TCP close releases
the port for the next client. Use only on a looped bench bridge.

Zander and FLARM must use the same baud rate in config.h for the loop.

With --radio-only, use the wired bridge instead: the DE-9 links 4-8 and 5-9
loop the radio port (bench step 10, docs/hardware/wiring.md#bus-test). The
Zander and FLARM loops and the navigation check are left out, since the
jumper loops are not allowed once the modules are soldered.

With --feed, use the wired bridge and a PC serial port that drives one
RS-232 input at RS-232 levels (bench steps 11 and 12,
docs/hardware/wiring.md#bus-test). The bytes must arrive unchanged on the
selected input's TCP port and nothing on the other one. The baud rate is
read from the bridge's /api/status. Needs pyserial.
"""

import argparse
import json
import random
import socket
import sys
import time
import urllib.request

ZANDER_TCP, FLARM_TCP, RADIO_TCP = 10110, 2000, 4352
FORMER_UDP = (14550, 14551, 14552)  # upstream UDP ports; the firmware has no UDP
BAUD = 19200


def payload(seed: int, size: int = 2048) -> bytes:
    rng = random.Random(seed)
    return bytes(range(256)) + bytes(rng.randrange(256) for _ in range(size))


def wait_seconds(size: int) -> float:
    # 10 bits per byte at BAUD, plus margin for Wi-Fi.
    return size * 10 / BAUD * 1.5 + 3.0


def read_tcp(sock: socket.socket, size: int, timeout: float) -> bytes:
    data = bytearray()
    deadline = time.monotonic() + timeout
    while len(data) < size and time.monotonic() < deadline:
        sock.settimeout(max(0.05, deadline - time.monotonic()))
        try:
            chunk = sock.recv(4096)
        except socket.timeout:
            break
        if not chunk:
            break
        data += chunk
    return bytes(data)


def drain(sock: socket.socket) -> None:
    sock.settimeout(0.3)
    try:
        while sock.recv(4096):
            pass
    except (socket.timeout, BlockingIOError):
        pass


def compare(name: str, sent: bytes, got: bytes) -> bool:
    if got == sent:
        print(f"PASS {name}: {len(sent)} bytes identical")
        return True
    first = next((i for i, (a, b) in enumerate(zip(sent, got)) if a != b), min(len(sent), len(got)))
    print(f"FAIL {name}: sent {len(sent)}, received {len(got)}, first difference at byte {first}")
    return False


def tcp(host: str, port: int) -> socket.socket:
    sock = socket.create_connection((host, port), timeout=5)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    return sock


def send_tcp(sock: socket.socket, data: bytes, split: int = 97) -> None:
    # Uneven writes, so the bridge sees split and merged reads.
    for i in range(0, len(data), split):
        sock.sendall(data[i : i + split])


def radio_owner_checks(host: str, zander: socket.socket | None, flarm: socket.socket | None) -> list[bool]:
    with tcp(host, RADIO_TCP) as owner:
        data = payload(8, size=128)
        send_tcp(owner, data)
        established = compare("radio owner established", data, read_tcp(owner, len(data), 3))
        if not established:
            return [False]

        with tcp(host, RADIO_TCP) as second:
            second.settimeout(2)
            try:
                second.sendall(b"\x00\xa5second writer")
                refused = second.recv(1) == b""
            except (ConnectionResetError, BrokenPipeError):
                refused = True
            except socket.timeout:
                refused = False
        print(f"{'PASS' if refused else 'FAIL'} second radio client refused")

        send_tcp(owner, data)
        intact = compare("radio owner receives only its own bytes", data, read_tcp(owner, len(data), 3))
        navigation = None
        if zander is not None and flarm is not None:
            nav_data = payload(9, size=128)
            send_tcp(zander, nav_data)
            navigation = compare("navigation with a radio owner", nav_data, read_tcp(flarm, len(nav_data), 3))

        # Wait for the server to acknowledge our FIN before attempting a new connection.
        owner.settimeout(2)
        owner.shutdown(socket.SHUT_WR)
        try:
            released = owner.recv(1) == b""
        except socket.timeout:
            released = False
        print(f"{'PASS' if released else 'FAIL'} radio owner released after TCP FIN")

    with tcp(host, RADIO_TCP) as replacement:
        try:
            send_tcp(replacement, data)
            got = read_tcp(replacement, len(data), 3)
        except (ConnectionResetError, BrokenPipeError):
            got = b""
        reopened = compare("radio port opens for the next client", data, got)

    return [r for r in (established, refused, intact, navigation, released, reopened) if r is not None]


def feed_checks(host: str, com_port: str, target: str) -> list[bool]:
    import serial  # pyserial, only needed for the feed test

    with urllib.request.urlopen(f"http://{host}/api/status", timeout=5) as reply:
        ports = {p["name"].lower(): p for p in json.load(reply)["ports"]}
    other = "flarm" if target == "zander" else "zander"
    baud = ports[target]["baud"]
    print(f"Feeding {com_port} at {baud} baud into the {target} input")

    want, quiet = tcp(host, ports[target]["tcp_port"]), tcp(host, ports[other]["tcp_port"])
    for sock in (want, quiet):
        drain(sock)
    data = payload(11 if target == "zander" else 12)
    with serial.Serial(com_port, baud, timeout=1) as feed:
        time.sleep(0.5)  # let the adapter settle after opening
        feed.reset_input_buffer()
        feed.write(data)
        feed.flush()
        got = read_tcp(want, len(data), len(data) * 10 / baud * 1.5 + 3.0)
    stray = read_tcp(quiet, 1, 1.0)
    want.close()
    quiet.close()
    results = [compare(f"RS-232 feed -> {target} TCP {ports[target]['tcp_port']}", data, got)]
    ok = stray == b""
    print(f"{'PASS' if ok else 'FAIL'} nothing on {other} TCP {ports[other]['tcp_port']}: {len(stray)} bytes")
    return results + [ok]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("host", help="bridge address, 10.10.10.10 in hotspot mode")
    parser.add_argument("--radio-loop", action="store_true", help="GPIO17 is looped to GPIO16")
    parser.add_argument("--radio-owner", action="store_true", help="test step 4 TCP ownership on the looped board")
    parser.add_argument("--radio-only", action="store_true", help="wired bridge with DE-9 links 4-8 and 5-9: radio port only")
    parser.add_argument("--feed", metavar="COM", help="PC serial port that drives one RS-232 input (bench steps 11, 12)")
    parser.add_argument("--input", choices=("zander", "flarm"), help="input fed by --feed")
    args = parser.parse_args()
    if args.feed:
        if not args.input:
            parser.error("--feed requires --input")
        results = feed_checks(args.host, args.feed, args.input)
        print(f"{sum(results)}/{len(results)} checks passed")
        return 0 if all(results) else 1
    if args.radio_only:
        args.radio_loop = True
    if args.radio_owner and not args.radio_loop:
        parser.error("--radio-owner requires --radio-loop")

    results = []
    zander = flarm = None
    if not args.radio_only:
        zander, flarm = tcp(args.host, ZANDER_TCP), tcp(args.host, FLARM_TCP)
        for sock in (zander, flarm):
            drain(sock)

        for name, src, dst, seed in (("TCP Zander -> FLARM", zander, flarm, 1), ("TCP FLARM -> Zander", flarm, zander, 2)):
            data = payload(seed)
            send_tcp(src, data)
            results.append(compare(name, data, read_tcp(dst, len(data), wait_seconds(len(data)))))

    radio = None
    if args.radio_loop:
        radio = tcp(args.host, RADIO_TCP)
        drain(radio)
        data = payload(7)
        send_tcp(radio, data)
        results.append(compare("TCP radio loop", data, read_tcp(radio, len(data), wait_seconds(len(data)))))

    out = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    for port in FORMER_UDP:
        out.sendto(b"must not reach any device", (args.host, port))
    stray = b"" if args.radio_only else read_tcp(zander, 1, 3.0) + read_tcp(flarm, 1, 0.5)
    if radio is not None:
        stray += read_tcp(radio, 1, 3.0 if args.radio_only else 0.5)  # with the loop, UDP into the radio UART would come back here
        radio.close()
    ok = stray == b""
    print(f"{'PASS' if ok else 'FAIL'} UDP to ports {', '.join(map(str, FORMER_UDP))}: {len(stray)} bytes reached a device")
    results.append(ok)

    if args.radio_owner:
        results.extend(radio_owner_checks(args.host, zander, flarm))

    for sock in (zander, flarm):
        if sock is not None:
            sock.close()
    print(f"{sum(results)}/{len(results)} checks passed")
    return 0 if all(results) else 1


if __name__ == "__main__":
    sys.exit(main())
