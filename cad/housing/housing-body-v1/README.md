# Housing body v1 (prototype)

First housing prototype on [layout v4](../layout-v4/README.md): base tray with connector wall and corner posts, and the hood. Rules: [housing § 2](../../../docs/hardware/housing.md#rules). Built in Autodesk Fusion through MCP, 2026-11-13, in the document `Housing body - prototype v1` (a copy of `Housing layout - compact v4` with the XT30 moved 4.2 mm inward, the DE-9 4 mm toward the XT30 and the ferrite-ring placeholder 2 mm inward; the holder v3 pieces are local copies of its version 7, link broken). Not printed yet.

![Closed housing: hood with flush screw counterbores and side vents, connector wall with XT30 and DE-9, cable-tie ears](preview-closed.png)

<details>
<summary><b>🖼️ Show the base with the hood off</b> (click to expand)</summary>

![Hood top with the dark grey MomoRC logo insert](preview-top.png)

![Base tray with the ESP32 holder, RS-232 clamp, MAX490, UBEC, ferrite ring placeholder and the four corner posts](preview-open.png)

</details>

| File | Use |
| --- | --- |
| `housing-base.f3d`, `housing-hood.f3d` | Fusion archives of the two parts; open with *File › Open › Open from my computer* |
| `housing-base.3mf`, `housing-hood.3mf`, `housing-logo-insert.3mf` | Print-ready meshes, Z up: base floor down; hood and logo insert top down in the same coordinates. Load hood and logo insert together as one object with two parts (dark grey filament for the logo) |
| `housing-logo-insert.f3d`, `housing-logo-insert.step` | Logo insert as its own part |
| `housing-base.step`, `housing-hood.step` | Solid models in the layout frame |

| Part | Main dimensions (mm) |
| --- | --- |
| Base tray | Outside 92 × 64 (104 long with the ears), floor 2.5, side walls to 10 above the floor top plus a 1.75 mm seam lip (inner 0.9); connector wall 3.0 and its corners full height to 19.75. Set 0.25 back from the hood wherever it is not a seat: lip top, connector-wall top and the two corner ends where the hood side walls butt on; the hood sits only on the post tops and on the wall shoulder at Y 10 |
| Hood | Outside 92 × 64, top 22.5 above the floor top, top 2.5 thick with four thick spots 4.6 thick (down to 17.9) over the posts, top edges R3, vertical corners R6 (inner corners R4); skirt 0.9 over the lip with 0.2 gap |
| Corner posts | 8 × 8 (far posts rounded R3.8 to follow the inner corner, 2.0 mm around each insert), tops 17.9 above the floor, inserts Ø4.0 × 6.7 deep; at the connector end the side and connector walls stay full height (19.75) around the post top. Insert centres (X, Z): (7, −24.5), (7, 28.5), (85.8, −23.3), (85.8, 28.3). Posts hang from the walls: free faces end 8 above the floor, the underside rises 45° from the walls to there (printable without support); free vertical edges R2. The XT30 corner post sits on the XT30 collar |
| Cover screws | DIN 912 M3 × 6 (head Ø5.4 × 2.9), counterbore Ø6.0 × 3.1 in the hood thick spots, heads flush, 1.5 under the head, Ø3.4 clearance; screw reaches 4.5 into the insert, tip 2.2 above the hole bottom |
| Connector wall | XT30 pocket 10.55 × 5.25 through wall and collar (centre Z −15.3, Y 6.5), its two corners at the chamfered side filled 45° with 1.3 legs (0.2 clear of the chamfers); collar 5 mm deep, from the floor to 2 above the pocket, 2 mm wall toward the corner post, top free edge R2; DE-9 (centre Z 8.5, Y 10) recess 31.6 × 13.2 × 1.0, D-shaped cutout 11.5 high, 19.9 wide for the top 2.5 and narrowing to 16.7 at the bottom (sides 10° like the rear body, 0.25 clear), holes Ø3.4 at 25.0, nut pockets 5.6 across flats × 1.0 deep from the inner face |
| Labels | Raised 0.6: "7..20V" (4 mm) above the XT30, "INSTR" (4 mm) below the DE-9 |
| Logo | MomoRC logo Ø56, a 0.6 mm dark grey insert flush with the hood top (two-colour print, the hood has the matching 0.6 mm recess), centred, upright seen from the connector end; traced from the owner's bitmap with white lines widened and black slivers removed so every feature is at least 0.8 wide |
| Vents | 2 rows × 5 slots per long side, 10 × 1.5, 2 mm ribs, 12.5 and 16.5 above the floor top |
| Ears | 4 × (6 out × 10 wide × 2.5, flush with the floor underside and the side faces) off both short ends, slots 5.4 × 1.9 for 4.8 mm ties, 2 mm from the wall face |

- Colours in Fusion and STEP: base dark green, hood a lighter green. Logo insert dark grey.
- Units: millimetres. Layout frame: X from the outer face of the connector wall, Y up from the floor top, Z across.
- Clash check in Fusion: no intersection with the holders, boards or connectors, except 0.3 mm³ with the XT30 reference model at its top face (model tolerance) and 0.3 mm³ with a small part of the oversized UBEC reference model at the far post on the MAX490 side.
- Placeholders: inside height 20 mm and the UBEC, MAX490 and ring positions come from layout v4 before the parts were measured. MAX490 posts, UBEC bridge and wire routing are not modelled; the hood is flat (no lower ring area or cut-off corner yet).
- Print checks: printed floor down, the DE-9 cutout top is a 19.9 mm bridge and the XT30 pocket top a 10.55 mm bridge (both were fit-tested printed flat, and the rule limit is 10 mm). The connector face has 2.7 mm of wall between the XT30 pocket and the DE-9 recess; the XT30 collar stays 0.8 mm clear of the nearer DE-9 nut pocket.
