# Uno bench feed and power cut

Bench fixture for [issue #2](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/2) and [WP6 phone acceptance](../../docs/project/roadmap.md#firmware-plan). It forwards PC-selected bytes to one bridge RS-232 input at 19,200 baud, 8N1, and can cut 12 V for five seconds while the feed keeps running.

## Build

Use the Uno target (`arduino:avr:uno`), AltSoftSerial 1.4, and Python with `pyserial`. Install AltSoftSerial from Arduino Library Manager or use a local copy of the [1.4 release](https://github.com/PaulStoffregen/AltSoftSerial/tree/1.4).

```powershell
arduino-cli compile --fqbn arduino:avr:uno firmware/uno_bench
```

## Proposed bench wiring

Agree the relay module, trigger polarity, and wiring with the owner before connecting. The default sketch assumes an active-high input. Set `relayActive` to match the approved module and verify that the hardware biases its input inactive with D7 high-impedance, including while the Uno boots or is unplugged.

| Connection | Wiring |
| --- | --- |
| USB | PC to Uno. Remove the RESET-to-GND jumper before uploading the sketch |
| D9 | To the MAX3232 **TTL input**, moving the lead from D0. D8 is unused |
| D7 and Uno GND | To the relay module’s logic input and ground |
| Fused bench +12 V | To relay COM, then NC to XT30 +. Run the supply return straight through to XT30 − |

Use a relay module with a driver and flyback protection and suitable DC contacts. Keep the bridge USB unplugged. See the [existing RS-232 hookup](../../docs/hardware/wiring.md#bus-test) and [bridge power path](../../docs/hardware/power.md#supply).

## Run

Only one program opens the Uno COM port. For a byte-exact wired check with no cuts, select the corresponding DE-9 input and use:

```powershell
python tools/bridge_acceptance_test.py 10.10.10.10 --uno-feed COM4 --input flarm --baseline
```

For a phone reconnect check, use a synthetic file with valid checksummed NMEA sentences and CR/LF line endings. The tool repeats the file once per second, cuts power once after the specified delay, and keeps sending until Ctrl+C. The file limit is 1,536 bytes. Use a short clip for the phone check.

```powershell
python tools/uno_bench.py COM4 --replay clip.nmea --cycle-after 15
```

For a standalone power cut, use:

```powershell
python tools/uno_bench.py COM4 --cycle
```

The USB link runs at 115,200 baud. Requests start and end with `C0` and use SLIP escapes `DB DC` for `C0` and `DB DD` for `DB`. The host sends one request at a time. The device replies with CR/LF-delimited lines.

| Request body | Reply |
| --- | --- |
| `UNO/1` | `UNO/1 READY` |
| `D` followed by 1–64 bytes | `QUEUED` once all payload bytes are queued for RS-232 TX |
| `CYCLE` | `CUT` when the pin is driven, then `RESTORED` after five seconds. A second request during the cut returns `BUSY` |

Confirm the off and on states independently at the bridge’s 5 V rail with RS-232 connected. The replies report the Uno’s GPIO commands. Use the [phone acceptance checklist](../../docs/project/open-items.md#software-and-acceptance) to record socket reconnection and fresh data separately.
