# Fit test 3

ESP32 holder, third version: two end blocks glued onto the housing floor, with a gluing jig. Described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-3.3mf` | Print-ready mesh (high refinement), four pieces in print orientation |
| `fit-test-3.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres; Z is the print direction. Print every piece as placed, without supports; about 3.8 cm³.
- Heights below are above the housing floor; PCB underside 3.5 mm.

| Piece | Size (mm) | Description |
| --- | --- | --- |
| A antenna cap | 9 × 38.5 foot, 10.2 high | Printed standing on its antenna-facing side. Broad glue foot; side walls along both long board edges; corner end walls; lips 1.7 thick (EN side 1.9 wide, BOOT side 1.5 wide, 0.25 clear of header strip and module); chamfered ledge under the board; bridge with a pad reaching 2 mm onto the shield, 0.3 mm pre-load; open window for the antenna overhang |
| B USB screw block | 15.5 × 24.5 foot, 8.2 high | Broad glue foot under and in front of the USB end; ledge under the board, two end stops, boss directly in front of the USB socket with a Ø2.2 pilot hole for M2.5 |
| U USB cap | 13.3 × 11.3 × 5 | Covers top and sides of the USB socket, pad 0.2 mm onto it; Ø2.8 hole for the M2.5 × 8 screw. Printed top down |
| J gluing jig | 49 × 28.5 dummy board with a dummy USB socket | Holds A and B at the board's exact spacing while their feet are glued: fit it into A like the board, set B under its USB end, screw U on. Remove after the glue has cured |

Values from the CAD model: shield top 3.2 mm and USB socket top 3.74 mm above the PCB, shield starting at the antenna-end PCB edge.
