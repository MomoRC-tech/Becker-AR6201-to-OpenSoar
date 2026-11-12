# RS-232 holder

Holds one RS-232 module: the board slides in flat between two grooved rails and is held by friction, only on its long edges beside the chip feet. Rules: [housing § 2](../../../docs/hardware/housing.md#rules); fit result: [§ 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12; tested and selected.

| File | Use |
| --- | --- |
| `rs232-holder.f3d` | Editable Fusion archive with the full timeline; open with *File › Open › Open from my computer* |
| `rs232-holder.step` | Solid model for checking dimensions or other CAD tools |
| `rs232-holder.3mf` | Print-ready mesh (high refinement), one holder in print orientation |

- Units: millimetres; Z is the print direction. Print **standing** as placed (on one rail end, grooves vertical), without supports; about 0.35 cm³ each, two needed. The slot height then comes from X/Y accuracy, not from the 0.2 mm layer steps. Use a brim if the 12.55 × 6.2 footprint does not stick.
- Size 12.55 × 6.2 × 9.0. The two notches on one outer rail face identify the 1.25 slot.

Board 9.2 × 15.6 measured, PCB 1.15 (S38):

| Feature | Value | Why |
| --- | --- | --- |
| Slot height | 1.25 (0.05 per side) | Holds the board by friction, slides in by hand; 1.20 was the tighter alternative |
| Grip length | 9.0, centred on the board | Each board end overhangs 3.3: both pad rows (up to 1.5 from the end) sit in free air, at least 1.8 from any PETG |
| Groove floor to groove floor | 9.35 (0.075 per side) | Lateral location |
| Lip and ledge overlap | 0.625 per side, lips 7.95 apart | Stays 0.5 clear of the chip feet and the underside capacitors (S41 model, placed in the measured outline) |
| PCB underside above the floor | 2.5 | Underside parts 0.45 + 2 mm rule |
| Lip above the PCB | 1.0 | |
| Lead-in | 0.4 × 45° at both groove ends | Board can go in from either end |
