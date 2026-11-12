# Fit test 4

RS-232 slot holder: the board slides in flat between two grooved rails and is held by friction, only on its long edges beside the chip feet. Described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-4.3mf` | Print-ready mesh (high refinement), two pieces in print orientation |
| `fit-test-4.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres; Z is the print direction. Print both pieces **standing** as placed (on one rail end, grooves vertical), without supports; about 0.7 cm³. The slot height then comes from X/Y accuracy, not from the 0.2 mm layer steps. Use a brim if the 12.55 × 6.2 footprint does not stick.
- Tell the pieces apart by the notches on one outer rail face: one notch = A, two notches = B.

| Piece | Slot height (PCB 1.15) | Size (mm) |
| --- | --- | --- |
| A | 1.20 (0.025 per side) | 12.55 × 6.2 × 9.0 |
| B | 1.25 (0.05 per side) | 12.55 × 6.2 × 9.0 |

Common to both (board 9.2 × 15.6 measured, S38):

| Feature | Value | Why |
| --- | --- | --- |
| Grip length | 9.0, centred on the board | Each board end overhangs 3.3: both pad rows (up to 1.5 from the end) sit in free air, at least 1.8 from any PETG |
| Groove floor to groove floor | 9.35 (0.075 per side) | Lateral location |
| Lip and ledge overlap | 0.625 per side, lips 7.95 apart | Stays 0.5 clear of the chip feet and the underside capacitors (S41 model, placed in the measured outline) |
| PCB underside above the floor | 2.5 | Underside parts 0.45 + 2 mm rule |
| Lip above the PCB | 1.0 | |
| Lead-in | 0.4 × 45° at both groove ends | Board can go in from either end |
