import sys
from types import SimpleNamespace
import unittest
from unittest.mock import patch

from uno_bench import UnoBench


class UnoBenchTest(unittest.TestCase):
    def test_forwarding_and_cycle_share_one_serial_connection(self):
        payload = bytes(range(256)) + b"\xc0CYCLE\xc0\xdb"
        replies = [b"UNO/1 READY\r\n", b"CUT\r\n", b"QUEUED\r\n",
                   b"RESTORED\r\n"] + [b"QUEUED\r\n"] * 4
        packets = []

        class Link:
            def __init__(self):
                self.closed = False

            def write(self, data):
                packets.append(data)
                return len(data)

            def readline(self, size):
                return replies.pop(0)

            def close(self):
                self.closed = True

        link = Link()
        with patch.dict(sys.modules, {"serial": SimpleNamespace(Serial=lambda *a, **kw: link)}):
            with patch("uno_bench.time.sleep"), UnoBench("COM4") as bench:
                bench.cycle()
                self.assertEqual(bench.write(payload), len(payload))
                self.assertTrue(bench.poll_restored())

        self.assertTrue(link.closed)
        self.assertEqual(packets[:2], [b"\xc0UNO/1\xc0", b"\xc0CYCLE\xc0"])
        decoded = bytearray()
        for packet in packets[2:]:
            self.assertTrue(packet.startswith(b"\xc0D"))
            self.assertTrue(packet.endswith(b"\xc0"))
            self.assertNotIn(0xc0, packet[1:-1])
            decoded.extend(packet[2:-1].replace(b"\xdb\xdc", b"\xc0").replace(b"\xdb\xdd", b"\xdb"))
        self.assertEqual(decoded, payload)
        self.assertFalse(replies)
