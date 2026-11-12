# Fit test 3

ESP32 holder, second version, after the holder test in fit test 2. Described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-3.3mf` | Print-ready mesh (high refinement), two pieces |
| `fit-test-3.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres; Z is the print direction. Print both pieces as placed, without supports; about 3.3 cm³.
- The bridge over the module spans 23 mm; check it for sag.

| Piece | Size (mm) | What changed |
| --- | --- | --- |
| L2 ESP32 holder | 59.5 × 45 frame, 13.8 high | Corner lips 1.7 thick (was 1.2); EN-side lip 1.9 wide, BOOT-side lip 1.5 wide (was 1.4 both), 0.25 clear of header strip and module; the board fits one way round, EN side at the wide lip. Bridge between the corner posts over the module shield with a pad pressing 0.3 mm onto the shield. USB end: no tab bosses; a slot post on the BOOT side and one boss with a Ø2.2 pilot hole on the EN side; two small end stops for the board edge |
| K USB keeper bar | 39 × 9 × 2.3 | One bar across the USB end: its tongue goes into the slot, the M2.5 × 8 screw goes through the other end; its arm presses a pad 0.3 mm onto the top of the USB socket. Printed pad up; turn it over to fit |

Values from the CAD model: shield top 3.2 mm and USB socket top 3.74 mm above the PCB, shield starting at the antenna-end PCB edge.
