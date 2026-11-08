"""Reproduce the pinned AR62xx send-length regression with a host C++ compiler.

Usage: python3 tools/check_becker_send.py --compiler g++
Upstream GPL-2.0-or-later source is downloaded into a temporary directory.
Only selected original function bodies are compiled. Port I/O is captured in
memory; mutex/condition waits are host stubs. This is not a radio acceptance test.
"""
import argparse
import pathlib
import subprocess
import tempfile
import urllib.request

OLD = "cd818c90ebb6e803a4b7a804935d87a7575eaa6f"
NEW = "b2e745cfddfe0150f6ed7db467ac18c321b38d01"


def fetch(repo, revision, path):
    url = f"https://raw.githubusercontent.com/{repo}/{revision}/{path}"
    with urllib.request.urlopen(url, timeout=30) as response:
        return response.read().decode()


def function(source, signature):
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 1
    end = opening + 1
    while depth:
        depth += (source[end] == "{") - (source[end] == "}")
        end += 1
    return source[start:end]


SCAFFOLD = r'''
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <span>
#include <string_view>
#include <vector>
struct OperationEnvironment {};
using Mutex = std::mutex;
struct Cond {
  void wait_for(std::unique_lock<Mutex>&, std::chrono::milliseconds) {}
};
auto AsBytes(std::string_view s) { return std::as_bytes(std::span{s.data(), s.size()}); }
struct Port {
  std::vector<std::byte> captured;
  void FullWrite(std::span<const std::byte> src, OperationEnvironment&,
                 std::chrono::steady_clock::duration) {
    captured.assign(src.begin(), src.end());
  }
  void FullWrite(std::string_view s, OperationEnvironment& env,
                 std::chrono::steady_clock::duration timeout);
};
union IntConvertStruct { uint16_t int_val16; uint8_t int_val8[2]; };
constexpr uint8_t HEADER_ID = 0xA5;
#define PROTID 0x14
#define ACTIVE_STATION 1
#define PASSIVE_STATION 0
struct AR62xxDevice {
  Port port;
  struct Radio { double active_frequency, passive_frequency; } radio_para;
  IntConvertStruct crc;
  bool b_sending = false;
  uint8_t response = 0;
  Mutex response_mutex;
  Cond rx_cond;
  static constexpr auto CMD_TIMEOUT = std::chrono::milliseconds(250);
  static constexpr unsigned NR_RETRIES = 3;
  static constexpr uint8_t NO_RSP = 0, ACK = 6;
  bool Send(const uint8_t*, unsigned, OperationEnvironment&);
  bool SendFixed(const uint8_t*, unsigned, OperationEnvironment&);
  int Frq2Idx(double) noexcept;
  uint16_t Freq2Idx(double) noexcept;
  uint16_t CRCBitwise(uint8_t*, size_t) noexcept;
  int SetAR620xStation(uint8_t*, int, double, const char*) noexcept;
};
'''

MAIN = r'''
int main() {
  OperationEnvironment env;
  for (auto pair : {std::pair{118.0, 123.5}, std::pair{123.0, 118.0}, std::pair{123.0, 124.0},
                    std::pair{121.5, 123.45}}) {
    AR62xxDevice device;
    device.radio_para = {pair.first, 125.0}; // Explicit synthetic known state.
    uint8_t command[128] = {};
    const auto count = device.SetAR620xStation(command, PASSIVE_STATION,
                                              pair.second, "synthetic");
    assert(count == 10);
    const auto expected = std::as_bytes(std::span{command, size_t(count)});
    const auto zero = std::find(command, command + count, uint8_t{0});
    const size_t text_count = zero - command;
    const bool result = device.Send(command, count, env);
    assert(result); // Current Send reports success even when truncated.
    assert(device.port.captured.size() == text_count);
    printf("active %.3f standby %.3f frame=", pair.first, pair.second);
    for (int i = 0; i < count; ++i) printf("%02X ", command[i]);
    printf("original=%zu/10 return=%s", device.port.captured.size(), result ? "true" : "false");
    // Proposed single-call correction, using the pinned API's byte-span overload.
    assert(device.SendFixed(command, count, env));
    assert(std::equal(expected.begin(), expected.end(),
                      device.port.captured.begin(), device.port.captured.end()));
    printf(" corrected=%zu/10\n", device.port.captured.size());
  }
}
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", default="g++")
    args = parser.parse_args()
    old = fetch("eku60/XCSoar", OLD, "src/Device/Driver/AR62xx.cpp")
    new = fetch("OpenSoaring/OpenSoar", NEW, "src/Device/Driver/AR62xx.cpp")
    header = fetch("OpenSoaring/OpenSoar", NEW, "src/Device/Port/Port.hpp")
    port = fetch("OpenSoaring/OpenSoar", NEW, "src/Device/Port/Port.cpp")
    assert "port.FullWrite(msg, msg_size, env, CMD_TIMEOUT)" in old
    assert "void FullWrite(std::span<const std::byte> src," in header
    assert "void FullWrite(std::string_view s," in header
    signatures = [("bool", "Send"), ("int", "Frq2Idx"),
                  ("uint16_t", "Freq2Idx"), ("uint16_t", "CRCBitwise"),
                  ("int", "SetAR620xStation")]
    bodies = [kind + "\n" + function(new, f"AR62xxDevice::{name}(")
              for kind, name in signatures]
    original_call = "port.FullWrite(reinterpret_cast<const char*> (msg), env, CMD_TIMEOUT);"
    assert bodies[0].count(original_call) == 1
    fixed = bodies[0].replace("AR62xxDevice::Send(", "AR62xxDevice::SendFixed(")
    fixed = fixed.replace(original_call,
                          "port.FullWrite(std::as_bytes(std::span{msg, msg_size}), env, CMD_TIMEOUT);")
    bodies.append(fixed)
    forwarding = "void\n" + function(port, "Port::FullWrite(std::string_view")
    with tempfile.TemporaryDirectory(prefix="becker-send-") as folder:
        path = pathlib.Path(folder)
        source = path / "check.cpp"
        source.write_text(SCAFFOLD + forwarding + "\n" + "\n".join(bodies) + MAIN)
        binary = path / "check"
        subprocess.run([args.compiler, "--version"], check=True)
        subprocess.run([args.compiler, "-std=c++20", "-Wall", "-Wextra",
                        "-fsanitize=address,undefined", "-g", str(source),
                        "-o", str(binary)], check=True)
        subprocess.run([str(binary)], check=True)


if __name__ == "__main__":
    main()
