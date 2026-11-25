"""Feed the Uno bench fixture and request a timed bridge power cut."""

import argparse
from pathlib import Path
import sys
import time


FEED_BAUD = 19200
FEED_FORMAT = "8N1"
CONTROL_BAUD = 115200
MAX_PAYLOAD = 64
MAX_REPLAY = 1536  # 80% of the bytes available in one second at 19200 8N1
END = b"\xc0"
ESC = b"\xdb"


def encode_frame(payload: bytes) -> bytes:
    return END + payload.replace(ESC, b"\xdb\xdd").replace(END, b"\xdb\xdc") + END


class UnoBench:
    """One synchronous owner of the fixture's USB serial port."""

    def __init__(self, port: str, timeout: float = 2.0):
        self.port = port
        self.timeout = timeout
        self.restore_deadline = None
        self.restored_at = None

    def __enter__(self):
        import serial

        self.serial = serial.Serial(self.port, CONTROL_BAUD, timeout=self.timeout,
                                    write_timeout=self.timeout)
        try:
            # Opening a Uno serial port can reset the AVR into its bootloader.
            time.sleep(2)
            self._request(b"UNO/1", b"UNO/1 READY")
        except BaseException:
            self.serial.close()
            raise
        return self

    def __exit__(self, *exc):
        self.serial.close()

    def _reply(self) -> bytes:
        reply = self.serial.readline(32)
        if not reply.endswith(b"\n"):
            raise TimeoutError(f"{self.port}: no complete reply from the Uno bench sketch")
        return reply.rstrip(b"\r\n")

    def _record_restore(self) -> None:
        if self.restore_deadline is None:
            raise OSError(f"{self.port}: unexpected RESTORED reply")
        self.restored_at = time.monotonic()
        self.restore_deadline = None

    def _request(self, payload: bytes, expected: bytes) -> None:
        packet = encode_frame(payload)
        if self.serial.write(packet) != len(packet):
            raise OSError(f"{self.port}: short write to Uno")
        reply = self._reply()
        if reply == b"RESTORED":
            self._record_restore()
            reply = self._reply()
        if reply != expected:
            raise OSError(f"{self.port}: expected {expected!r}, received {reply!r}")

    def write(self, payload: bytes) -> int:
        for offset in range(0, len(payload), MAX_PAYLOAD):
            self._request(b"D" + payload[offset:offset + MAX_PAYLOAD], b"QUEUED")
        return len(payload)

    def cycle(self) -> None:
        if self.restore_deadline is not None:
            raise ValueError("a power cycle is already in progress")
        self._request(b"CYCLE", b"CUT")
        self.restored_at = None
        self.restore_deadline = time.monotonic() + 5 + self.timeout

    def poll_restored(self) -> bool:
        if self.restore_deadline is not None:
            if self.serial.in_waiting:
                reply = self._reply()
                if reply != b"RESTORED":
                    raise OSError(f"{self.port}: expected RESTORED, received {reply!r}")
                self._record_restore()
            elif time.monotonic() >= self.restore_deadline:
                raise TimeoutError(f"{self.port}: timed out waiting for relay restore acknowledgment")
        return self.restored_at is not None


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("port", help="Uno USB serial port, for example COM4")
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--cycle", action="store_true", help="request one five-second cut")
    mode.add_argument("--replay", type=Path, help="repeat the bytes of a short file each second")
    parser.add_argument("--cycle-after", type=int, metavar="SECONDS",
                        help="request one cut after this many seconds of replay")
    args = parser.parse_args(argv)
    if args.cycle_after is not None and (args.replay is None or args.cycle_after < 1):
        parser.error("--cycle-after requires --replay and a positive delay")
    try:
        payload = b""
        if args.replay is not None:
            with args.replay.open("rb") as clip:
                payload = clip.read(MAX_REPLAY + 1)
            if not 1 <= len(payload) <= MAX_REPLAY:
                parser.error(f"replay file must contain 1–{MAX_REPLAY} bytes")

        with UnoBench(args.port) as uno:
            started = next_replay = time.monotonic()
            cycle_at = started if args.cycle else (
                started + args.cycle_after if args.cycle_after is not None else None)
            cut_at = None
            while True:
                now = time.monotonic()
                if cycle_at is not None and now >= cycle_at:
                    uno.cycle()
                    cut_at = time.monotonic()
                    cycle_at = None
                    print("Uno acknowledged relay cut", flush=True)
                if payload and now >= next_replay:
                    uno.write(payload)
                    next_replay = now + 1
                if cut_at is not None and uno.poll_restored():
                    print(f"Uno acknowledged relay restoration after {uno.restored_at - cut_at:.3f} s",
                          flush=True)
                    cut_at = None
                    if args.cycle:
                        return 0
                time.sleep(0.02)
    except KeyboardInterrupt:
        return 130
    except ModuleNotFoundError as error:
        if error.name != "serial":
            raise
        print("FAIL Uno bench requires pyserial (python -m pip install pyserial)", file=sys.stderr)
    except (OSError, ValueError) as error:
        print(f"FAIL {error}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
