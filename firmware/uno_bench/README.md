# Uno bench feed and power cut

Bench fixture for [issue #2](https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/issues/2) and [WP6 phone acceptance](../../docs/project/roadmap.md#firmware-plan). The current setup forwards PC-selected bytes to one bridge RS-232 input at 19,200 baud, 8N1.

## Bench equipment

| Source | PC test option |
| --- | --- |
| USB–RS-232 adapter | Raw serial via `--feed` |
| Arduino Uno and MAX3232 | Framed serial via `--uno-feed`. The verified D9 feed is wired to Zander on DE-9 pin 1 ([S214](../../docs/research/sources.md#source-register)) |

The harness accepts two raw feeds or one framed Uno feed paired with a raw feed on the other input. The relay is prepared for later use and remains uninstalled.

## Feed-only setup

With the Uno unplugged, remove the RESET-to-GND jumper and move the MAX3232 **TTL input** lead from **D0 to D9**. Leave D8 and D7 open. Keep the existing [MAX3232 power and DE-9 connections](../../docs/hardware/wiring.md#bus-test).

Use the Uno target (`arduino:avr:uno`), AltSoftSerial 1.4, and Python with `pyserial`. Install AltSoftSerial from Arduino Library Manager or use a local copy of the [1.4 release](https://github.com/PaulStoffregen/AltSoftSerial/tree/1.4). Reconnect the Uno and use its USB port in the upload command.

```powershell
arduino-cli compile --upload --port COM4 --fqbn arduino:avr:uno firmware/uno_bench
```

Only one program opens the Uno COM port. For a byte-exact wired check, select the corresponding DE-9 input. This example uses the verified Zander connection and the bridge’s hotspot address.

```powershell
python tools/bridge_acceptance_test.py 10.10.10.10 --uno-feed COM4 --input zander --baseline
```

The replay command sends a synthetic NMEA clip once per second until Ctrl+C. Use a file of up to 1,536 bytes with valid checksums and CR/LF line endings.

```powershell
python tools/uno_bench.py COM4 --replay clip.nmea
```

## Optional relay

The sketch also supports a five-second power cut. Leave the relay disconnected in the feed-only setup. Before adding it, confirm the module, trigger polarity, and wiring. The sketch defaults to active high. Verify the relay input stays inactive when D7 is high-impedance, including while the Uno boots or is unplugged.

| Connection | Wiring |
| --- | --- |
| D7 and Uno GND | To the relay module’s logic input and ground |
| Fused bench +12 V | To relay COM, then NC to XT30 +. Run the supply return straight through to XT30 − |

Use a relay module with a driver and flyback protection and suitable DC contacts. Keep the bridge USB unplugged. See the [bridge power path](../../docs/hardware/power.md#supply).

```powershell
python tools/uno_bench.py COM4 --replay clip.nmea --cycle-after 15
python tools/uno_bench.py COM4 --cycle
```

Confirm the off and on states independently at the bridge’s 5 V rail with RS-232 connected. The replies report the Uno’s GPIO commands. Use the [phone acceptance checklist](../../docs/project/open-items.md#software-and-acceptance) to record socket reconnection and fresh data separately.

## USB protocol

The USB link runs at 115,200 baud. Requests start and end with `C0` and use SLIP escapes `DB DC` for `C0` and `DB DD` for `DB`. The host sends one request at a time. The device replies with CR/LF-delimited lines.

| Request body | Reply |
| --- | --- |
| `UNO/1` | `UNO/1 READY` |
| `D` followed by 1–64 bytes | `QUEUED` once all payload bytes are queued for RS-232 TX |
| `CYCLE` | `CUT` when the pin is driven, then `RESTORED` after five seconds. A second request during the cut returns `BUSY` |
