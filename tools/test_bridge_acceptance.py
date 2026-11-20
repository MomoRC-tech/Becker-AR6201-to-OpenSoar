"""Host regression for the radio verdict, using local sockets in place of a bridge."""

import io
import socket
import threading
import unittest
from argparse import Namespace
from contextlib import ExitStack, redirect_stdout
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


if __name__ == "__main__":
    unittest.main()
