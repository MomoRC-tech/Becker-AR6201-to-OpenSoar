# Fit-test coupon

Project model of the PETG fit-test print described in [housing § 2.3](../../../docs/hardware/housing.md#fit-test). Built in Autodesk Fusion through MCP, 2026-11-12. The parametric source stays in the owner's Fusion account; these exports are the repository copies.

| File | Use |
| --- | --- |
| `fit-test-coupon.3mf` | Print-ready mesh (high refinement), six separate pieces A–F |
| `fit-test-coupon.step` | Solid model for checking dimensions or re-slicing |

- Units: millimetres; Z is the print direction. Print every piece as placed, without supports.
- Only the material each test needs: about 14.5 cm³ in total.

| Piece | Size (mm) | Tests | Variants |
| --- | --- | --- | --- |
| A DE-9 wall | 56 × 9.5 × 20, 3.5 mm wall on a 2 mm foot | DE-9 cutout, mating, jackposts embossed text | Cutout 0.4 mm clearance on the shell; holes Ø3.1 (left) and Ø3.3 (right) at 25.0 mm spacing; right hole in a Ø9 recess leaving 1.6 mm wall; "12V" 5 mm high, 0.6 mm raised on the outside face |
| B XT30 pockets | 32 × 9 × 17 | XT30 press fit | Interference 0.05 / 0.10 / 0.15 mm per side, left to right; insert from the back, the rib stops at the 9 mm depth |
| C Screw and insert holes | 68 × 10 × 2.5 bar | M3 clearance holes, heat-set inserts | Holes Ø3.2 / 3.3 / 3.4 / 3.5 / 3.6; Ø8 bosses with insert holes Ø3.9 / 4.0 / 4.1, 6.7 mm deep |
| D PCB slots | 34 × 10 × 7 | PCB edge slots, 5 mm deep | 1.7 / 1.8 / 1.9 / 2.0 (1.6 mm PCBs), 1.3 / 1.4 / 1.5 / 1.6 (1.2 mm PCBs) |
| E Snap hooks | 86 × 8 × 11.8 strip | Snap hooks | Pairs 1.2 / 1.5 / 1.8 mm thick, 0.6 mm lip, 8.2 mm flexing length, no root fillet; plate rests on two ribs per pair |
| F Test plate | 20 × 20 × 1.6 | Stand-in PCB for the hooks | — |
