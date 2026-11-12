# Fit test 2

Second PETG fit-test print: the XT30 and DE-9 pieces redesigned on the measured connectors, and the first trial of separate PCB clips. Described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-2.3mf` | Print-ready mesh (high refinement), eleven separate pieces |
| `fit-test-2.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres; Z is the print direction. Print every piece as placed, without supports; about 10 cm³ in total.
- The clips look alike: mark each with a pen as it comes off the plate (layout below, left to right in the clip row).

| Piece | Size (mm) | Tests | Variants |
| --- | --- | --- | --- |
| G XT30 | 44 × 8 × 16 block | Press fit on the measured connector (10.55 × 5.25) | Pockets 0.15 / 0.10 / 0.05 / 0.00 mm larger per side than measured, left to right; chamfered end up with 0.9 × 1.1 chamfers (connector 1.3 × 1.6), so the chamfers have clearance; 8 mm deep: insert front first from the back face, the raised markings stop at the back face, the front stands 0.5 mm proud |
| H DE-9 | 44 × 3 wall on a 10 × 2 foot, 20 high | Front mount | Flange recess 31.6 × 13.2 × 1.0 deep on the outside face; rear-body cutout 19.9 × 11.5; holes Ø3.2 (left) and Ø3.4 (right) at 25.0 spacing; hex nut pockets 6.65 across flats, 1.0 deep in the inside face (wall left at the nut 1.0 mm); cutout top is a 20 mm bridge |
| J clip base, 1.6 mm PCB | 20 × 36 floor, 1.6 thick | Clip sockets for plate F (20 mm span) | Two dovetail sockets; any clip drops in from above |
| K clip base, 1.2 mm PCB | 20 × 25 floor | Clip sockets for an RS-232 board across its 9.2 mm width | Same sockets, 9.2 mm span |
| C1 | 8 long | Fixed lip, 1.6 mm PCB | Slot 1.7, lip 1.0 |
| C2 | 16 long | Sideways arm 1.2 mm, 1.6 mm PCB | Arm 11 long, catch lip 1.0 with 45° ramp |
| C3, C4 | 16 long | Sideways arm 1.5 mm, 1.6 mm PCB | Two pieces, to test a pair |
| C5 | 8 long | Fixed lip, 1.2 mm PCB | Slot 1.3 |
| C6 | 16 long | Sideways arm 1.2 mm, 1.2 mm PCB | — |
| C7 | 16 long | Sideways arm 1.5 mm, 1.2 mm PCB | — |

Common to all clips: PCB underside 3.0 mm above the floor (trial value), ledge 1.5 mm under the PCB edge, dovetail tongue 4 mm high in a socket with 0.2 mm clearance per side. The sideways arm springs parallel to the floor, so its length does not set the PCB height.
