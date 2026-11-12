# Fit test 3

ESP32 holder, second version, after the holder test in fit test 2. Described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-3.3mf` | Print-ready mesh (high refinement), three pieces |
| `fit-test-3.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres; Z is the print direction. Print all pieces as placed, without supports; about 3.6 cm³.

| Piece | Size (mm) | What changed |
| --- | --- | --- |
| L3 ESP32 holder | 59.5 × 45 frame, 13.8 high | Corner lips 1.7 thick (was 1.2); EN-side lip 1.9 wide, BOOT-side lip 1.5 wide (was 1.4 both), 0.25 clear of header strip and module; the board fits one way round, EN side at the wide lip. Corner posts end 0.5 mm above the shield top with a Ø2.0 peg each for the shield clamp. USB end: no tab bosses; a slot post on the BOOT side and one boss with a Ø2.2 pilot hole on the EN side; two small end stops for the board edge |
| S antenna shield clamp | 11.5 × 29.5 × 2.3 | Separate plate glued onto the two post pegs (Ø2.3 holes); reaches 9 mm onto the module, clear of both header rows; its pad (7.8 × 12.5) presses 0.3 mm onto the shield. Printed pad up; turn it over to fit |
| K USB keeper bar | 39 × 9 × 2.3 | One bar across the USB end: its tongue goes into the slot, the M2.5 × 8 screw goes through the other end; its arm presses a pad 0.3 mm onto the top of the USB socket. Printed pad up; turn it over to fit |

Values from the CAD model: shield top 3.2 mm and USB socket top 3.74 mm above the PCB, shield starting at the antenna-end PCB edge.
