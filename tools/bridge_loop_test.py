"""Step 2 loop check for the bridge firmware.

    python tools/bridge_loop_test.py 10.10.10.10
    python tools/bridge_loop_test.py 192.168.1.50 --radio-loop

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

Zander and FLARM must use the same baud rate in config.h for the loop.
"""

import argparse
import random
import socket
import sys
import time

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


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("host", help="bridge address, 10.10.10.10 in hotspot mode")
    parser.add_argument("--radio-loop", action="store_true", help="GPIO17 is looped to GPIO16")
    args = parser.parse_args()

    results = []
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
    stray = read_tcp(zander, 1, 3.0) + read_tcp(flarm, 1, 0.5)
    if radio is not None:
        stray += read_tcp(radio, 1, 0.5)  # with the loop, UDP into the radio UART would come back here
        radio.close()
    ok = stray == b""
    print(f"{'PASS' if ok else 'FAIL'} UDP to ports {', '.join(map(str, FORMER_UDP))}: {len(stray)} bytes reached a device")
    results.append(ok)

    zander.close()
    flarm.close()
    print(f"{sum(results)}/{len(results)} checks passed")
    return 0 if all(results) else 1


if __name__ == "__main__":
    sys.exit(main())
