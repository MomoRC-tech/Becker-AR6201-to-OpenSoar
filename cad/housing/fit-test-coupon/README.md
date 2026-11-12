# Fit-test coupon

Project model of the PETG fit-test print described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12. The parametric source stays in the owner's Fusion account; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-coupon.3mf` | Print-ready mesh (high refinement), two bodies: coupon and the 20 × 20 × 1.6 mm test plate |
| `fit-test-coupon.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres. Model Z is the print direction: the 112 × 60 × 2.5 mm base plate lies on the bed, the 28 mm connector wall stands up along the long edge.
- No supports needed. All horizontal holes (XT30 pockets, DE-9 cutout, jackpost holes) bridge across 5–17 mm.
- Feature positions, in mm from the plate corner (X along the wall, Y across the plate):

| Feature | Where | Variants |
| --- | --- | --- |
| DE-9 cutout + jackpost holes | Wall, centre X 22, 15 above the bed | Cutout 0.4 mm clearance on the shell; holes Ø3.1 (left) and Ø3.3 (right); right hole in a Ø9 recess leaving 1.6 mm wall |
| XT30 pockets | Wall boss X 44–84, 9 mm deep | Interference 0.05 / 0.10 / 0.15 mm per side, left to right |
| M3 clearance holes | Plate, Y 56, X 8–32 | Ø3.2, 3.3, 3.4, 3.5, 3.6 |
| Heat-set insert bosses | Plate, Y 52, X 46 / 58 / 70 | Ø8 bosses, holes Ø3.9 / 4.0 / 4.1, 6.7 mm deep |
| PCB edge slots | Block X 8–40, Y 42–54 | Widths 1.7 / 1.8 / 1.9 / 2.0 (for 1.6 mm PCBs) and 1.3 / 1.4 / 1.5 / 1.6 (for 1.2 mm PCBs), 5 mm deep |
| Snap-hook pairs | Pedestals centred X 46 / 72 / 98, Y 30 | Hook thickness 1.2 / 1.5 / 1.8 mm, 0.6 mm lip, 8.2 mm flexing length, no root fillet |
| Embossed text | Plate, X 8–34, Y 10–25 | "12V" 5 mm, "FUSE" 7 mm, 0.6 mm high |
