"""Host regressions using local sockets and simulated RS-232 feeds."""

import io
import socket
import threading
import time
import unittest
from argparse import Namespace
from contextlib import ExitStack, redirect_stderr, redirect_stdout
from types import SimpleNamespace
from unittest.mock import patch

import bridge_acceptance_test as acceptance


class RadioVerdictTest(unittest.TestCase):
    def check_loop(self, drop_second_echo):
        with ExitStack() as stack:
            sender, reader = socket.socketpair()
            radio, peer = socket.socketpair()
            for sock in (sender, reader, radio, peer):
                stack.enter_context(sock)

            def echo():
                with peer.makefile("rb") as stream:
                    count = 0
                    while frame := stream.read(4):
                        count += 1
                        if drop_second_echo and count == 2:
                            peer.sendall(frame[:2])
                            peer.shutdown(socket.SHUT_WR)
                            return
                        peer.sendall(frame)

            responder = threading.Thread(target=echo, daemon=True)
            responder.start()
            stack.enter_context(patch.object(acceptance, "tcp", side_effect=[sender, reader, radio]))
            stack.enter_context(patch.object(acceptance, "counters", return_value="local sockets"))
            output = stack.enter_context(redirect_stdout(io.StringIO()))
            args = Namespace(host="local", seconds=0.8, max_delay=2, baseline=True, radio_loop=True)
            result = acceptance.run(args)
            responder.join(timeout=2)
            self.assertFalse(responder.is_alive())
            return result, output.getvalue()

    def test_radio_echo_through_end_passes(self):
        result, output = self.check_loop(drop_second_echo=False)
        self.assertTrue(result, output)

    def test_partial_echo_after_success_fails(self):
        result, output = self.check_loop(drop_second_echo=True)
        self.assertFalse(result, output)
        self.assertIn("FAIL radio loop during the test: 1 frames, 1 lost", output)


class WiredAcceptanceTest(unittest.TestCase):
    def run_wired(self, feeds, fault=None, radio=False, slow=False):
        # Non-default TCP ports and different baud rates make it possible to
        # catch tests that use firmware defaults or the other input's settings.
        ports = {
            "Zander": dict(uart=0, baud=9600, tcp_port=21010),
            "FLARM": dict(uart=1, baud=19200, tcp_port=22000),
            "Becker": dict(uart=2, baud=4800, tcp_port=24352),
        }
        for fields in ports.values():
            fields.update(tcp_clients=0, short_writes=0, uart_errors=0)
        argv = ["local", "--seconds", "0.25", "--max-delay", "0.25"]
        if len(feeds) == 1:
            name, device = next(iter(feeds.items()))
            argv.extend(["--feed", device, "--input", name.lower()])
        else:
            argv.extend(["--zander-feed", feeds["Zander"], "--flarm-feed", feeds["FLARM"]])
        if not slow:
            argv.append("--baseline")
        if radio:
            argv.append("--radio-loop")
        written = {"COM4": bytearray(), "COM5": bytearray()}
        sockets, connections, opened, closed, write_order = {}, [], [], [], []
        output = io.StringIO()
        assert_equal = self.assertEqual
        started = time.monotonic()

        def tcp(host, port, rcvbuf=0):
            self.assertEqual(host, "local")
            key = (port, bool(rcvbuf))
            self.assertNotIn(key, sockets)
            connections.append(key)
            client, peer = socket.socketpair()
            stack.enter_context(peer)
            # Keep a fallback so a failed cleanup assertion closes the socket.
            stack.callback(client.close)
            sockets[key] = (client, peer)
            if port == 24352 and fault == "radio-eof":
                peer.shutdown(socket.SHUT_WR)
            return client

        class Feed:
            def __init__(self, device, baudrate, **kwargs):
                if fault == "open" and device == "COM5":
                    raise OSError("simulated adapter open failure")
                frame = kwargs["bytesize"], kwargs["parity"], kwargs["stopbits"]
                assert_equal(frame, (7, "E", 1) if device == "COM4" else (8, "N", 1))
                opened.append((device, baudrate))
                self.device = device

            def __enter__(self):
                return self

            def __exit__(self, *exc):
                self.close()

            def close(self):
                closed.append(self.device)

            def reset_input_buffer(self):
                pass

            def flush(self):
                pass

            def write(self, data):
                port, other = (21010, 22000) if self.device == "COM4" else (22000, 21010)
                size = len(data)
                first = not written[self.device]
                write_order.append(self.device)
                written[self.device].extend(data)
                if fault == "swap":
                    port = other
                elif fault == "missing" and self.device == "COM5" and first:
                    data = data[1:]
                elif fault == "cross" and self.device == "COM4" and first:
                    sockets[other, False][1].sendall(data[:1])
                sockets[port, False][1].sendall(data)
                return size

        def http_json(host, path):
            if path == "/api/settings":
                return {"items": [{"key": f"f{uart}", "value": frame}
                                  for uart, frame in enumerate(("7E1", "8N1", "8N1"))]}
            self.assertTrue(path.startswith("/api/log"), path)
            now = 60000 + int((time.monotonic() - started) * 1000)
            # A real log poll can return an ordinary connection event even if no client stalled.
            return {"seq": 1, "boot": 1, "lost": False, "now": now,
                    "lines": [{"q": 1, "t": now, "m": "FLARM: 2 apps connected"}]}

        def status(host):
            connected = [port for (port, _), (client, _) in sockets.items() if client.fileno() != -1]
            active = {}
            for name, fields in ports.items():
                port = fields["tcp_port"]
                clients = connected.count(port)
                short_writes = 5 if clients == 2 and fault != "no-pressure" else 0
                active[name] = {**fields, "tcp_clients": clients, "short_writes": short_writes}
            return active

        with ExitStack() as stack:
            stack.enter_context(patch.object(acceptance, "tcp", side_effect=tcp))
            stack.enter_context(patch.object(acceptance, "status", side_effect=status))
            stack.enter_context(patch.object(acceptance, "get_json", side_effect=http_json))
            stack.enter_context(patch.dict("sys.modules", {"serial": SimpleNamespace(Serial=Feed)}))
            stack.enter_context(redirect_stdout(output))
            stack.enter_context(redirect_stderr(output))
            result = acceptance.main(argv) == 0
            self.assertCountEqual(closed, [device for device, _ in opened])
            for client, _ in sockets.values():
                self.assertEqual(client.fileno(), -1)
            if result and len(feeds) == 2:
                latest_start = max(write_order.index(device) for device in feeds.values())
                earliest_finish = min(len(write_order) - 1 - write_order[::-1].index(device)
                                      for device in feeds.values())
                self.assertLess(latest_start, earliest_finish, write_order)
        return result, output.getvalue(), connections, opened, written

    def test_single_feed_uses_its_active_tcp_port_and_baud(self):
        for name, device, baud, other_device in (
            ("Zander", "COM4", 9600, "COM5"),
            ("FLARM", "COM5", 19200, "COM4"),
        ):
            with self.subTest(input=name):
                result, output, connections, opened, written = self.run_wired({name: device})
                self.assertTrue(result, output)
                self.assertCountEqual(connections, [(21010, False), (22000, False)])
                self.assertEqual(opened, [(device, baud)])
                self.assertTrue(written[device])
                self.assertFalse(written[other_device])

    def test_single_feed_rejects_traffic_on_unfed_port(self):
        result, output, _, _, _ = self.run_wired({"Zander": "COM4"}, "cross")
        self.assertFalse(result, output)
        self.assertIn("FAIL FLARM:", output)
        self.assertIn("unexpected data at byte 0", output)

    def test_dual_feed_is_byte_exact_and_rejects_stream_faults(self):
        feeds = {"Zander": "COM4", "FLARM": "COM5"}
        for fault in (None, "swap", "missing", "cross"):
            with self.subTest(fault=fault):
                result, output, _, opened, written = self.run_wired(feeds, fault)
                self.assertEqual(result, fault is None, output)
                self.assertCountEqual(opened, [("COM4", 9600), ("COM5", 19200)])
                if fault is None:
                    self.assertTrue(written["COM4"] and written["COM5"], output)
                    self.assertNotEqual(written["COM4"][:32], written["COM5"][:32])
                else:
                    self.assertIn("at byte", output)

    def test_slow_app_uses_selected_port_and_requires_short_writes(self):
        for fault in (None, "no-pressure"):
            with self.subTest(fault=fault):
                result, output, connections, _, _ = self.run_wired({"FLARM": "COM5"}, fault, slow=True)
                self.assertEqual(result, fault is None, output)
                self.assertCountEqual(connections, [(21010, False), (22000, False), (22000, True)])
                verdict = "PASS" if fault is None else "INCONCLUSIVE"
                self.assertIn(f"{verdict} FLARM slow app", output)

    def test_slow_app_closure_needs_complete_log_and_minimum_elapsed_time(self):
        before = {"short_writes": 7}
        for elapsed, clients, closures, log_ok, verdict in (
            (90, 1, [60.1], True, "PASS"),
            (90, 1, [59.9], True, "FAIL"),
            (90, 1, [60.1], False, "FAIL"),
            (90, 2, [], True, "INCONCLUSIVE"),
        ):
            with self.subTest(closures=closures, clients=clients, log_ok=log_ok):
                after = {"short_writes": 12, "tcp_clients": clients}
                with redirect_stdout(io.StringIO()) as output:
                    result = acceptance.slow_app_result("FLARM", elapsed, before, after, closures, log_ok)
                self.assertEqual(result, verdict == "PASS", output.getvalue())
                self.assertIn(f"{verdict} FLARM slow app", output.getvalue())

    def test_wired_radio_eof_fails_on_active_radio_port(self):
        result, output, connections, _, _ = self.run_wired(
            {"Zander": "COM4", "FLARM": "COM5"}, "radio-eof", radio=True,
        )
        self.assertFalse(result, output)
        self.assertIn((24352, False), connections)
        self.assertIn("radio connection closed", output)

    def test_failed_second_adapter_open_closes_first_adapter(self):
        result, output, _, opened, _ = self.run_wired({"Zander": "COM4", "FLARM": "COM5"}, "open")
        self.assertFalse(result, output)
        self.assertEqual(opened, [("COM4", 9600)])
        self.assertIn("simulated adapter open failure", output)


if __name__ == "__main__":
    unittest.main()
