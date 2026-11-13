# Housing body v1 (prototype)

First housing prototype on [layout v4](../layout-v4/README.md): base tray with connector wall and corner posts, and the hood. Rules: [housing § 2](../../../docs/hardware/housing.md#rules). Built in Autodesk Fusion through MCP, 2026-11-13, in the document `Housing body - prototype v1` (a copy of `Housing layout - compact v4` with the XT30 moved 4.2 mm inward, the DE-9 4 mm toward the XT30, the ferrite-ring placeholder 2 mm inward, and the housing 3 mm wider on the MAX490 side with the MAX490 board and UBEC 3 mm further out; the holder v3 pieces are local copies of its version 7, link broken). Not printed yet.

![Closed housing: hood with flush screw counterbores and side vents, connector wall with XT30 and DE-9, tie loops on the base sides](preview-closed.png)

<details>
<summary><b>🖼️ Show the base with the hood off</b> (click to expand)</summary>

![Hood top with the dark grey MomoRC logo and "7-20V" inserts](preview-top.png)

![Base tray with the ESP32 holder, RS-232 clamp, MAX490, UBEC, ferrite ring placeholder and the four corner posts](preview-open.png)

</details>

| File | Use |
| --- | --- |
| [housing-assembly.f3d](housing-assembly.f3d) | Full editable Fusion assembly with the [measured UBEC](../../reference/ubec-hobbywing-3a/README.md), [measured ferrite ring](../../../docs/hardware/housing.md#contents) and a local copy of the MAX490 board. Open with *File › Open › Open from my computer*. |
| `housing-base.f3d`, `housing-hood.f3d` | Fusion archives of the two parts; open with *File › Open › Open from my computer* |
| `housing-base.3mf`, `housing-hood.3mf`, `housing-logo-insert.3mf` | Print-ready meshes, Z up: base floor down; hood and logo insert top down in the same coordinates. Load hood and logo insert together as one object with two parts (dark grey filament for the logo) |
| `housing-logo-insert.f3d`, `housing-logo-insert.step` | Logo and "7-20V" insert as its own part |
| `housing-base.step`, `housing-hood.step` | Solid models in the layout frame |

| Part | Main dimensions (mm) |
| --- | --- |
| Base tray | Outside 92 × 67 (75 wide over the tie loops), floor 2.5, side walls to 10 above the floor top plus a 1.75 mm seam lip (inner 0.9); connector wall 3.0 and its corners full height to 20.0, flat against the hood top underside. Set 0.25 back from the hood wherever it is not a seat: lip top and the two corner ends where the hood side walls butt on; the hood sits on the post tops, the wall shoulder at Y 10 and the connector-wall and corner tops at Y 20.0 |
| Hood | Outside 92 × 67, top 22.5 above the floor top, top 2.5 thick with four thick spots 4.6 thick (down to 17.9) over the posts, top edges R3, vertical corners R6 (inner corners R4); skirt 0.9 over the lip with 0.2 gap |
| Corner posts | 8 × 8 (far posts rounded R3.8 to follow the inner corner, 2.0 mm around each insert), tops 17.9 above the floor, inserts Ø4.0 × 6.7 deep, entries chamfered 0.2; at the connector end the side and connector walls stay full height (20.0) around the post top. Insert centres (X, Z): (7, −24.5), (7, 31.5), (85.8, −23.3), (85.8, 31.3). Posts hang from the walls: free faces end 8 above the floor, the underside rises 45° from the walls to there (printable without support); free vertical edges R2. The XT30 corner post sits on the XT30 collar |
| Cover screws | DIN 912 M3 × 6 (head Ø5.4 × 2.9), counterbore Ø6.0 × 3.1 in the hood thick spots (at the XT30 corner the thick spot reaches 1.0 further out and down to 19.35 into a 1.0 × 0.9 rebate (floor 19.1) in the base side wall top at X 3–11, so the counterbore keeps 0.8 of wall), heads flush, 1.5 under the head, Ø3.4 clearance; screw reaches 4.5 into the insert, tip 2.2 above the hole bottom |
| Connector wall | XT30 pocket 10.55 × 5.25 through wall and collar (centre Z −15.3, Y 6.5), its two corners at the chamfered side filled 45° with 1.3 legs (0.2 clear of the chamfers); collar 5 mm deep, from the floor to 2 above the pocket, 2 mm wall toward the corner post, top free edge R2; DE-9 (centre Z 8.5, Y 10) recess 31.6 × 13.2 × 1.0, D-shaped cutout 11.5 high, 19.9 wide for the top 2.5 and narrowing to 16.7 at the bottom (sides 10° like the rear body, 0.25 clear), holes Ø3.4 at 25.0, nut pockets 5.6 across flats × 1.0 deep from the inner face |
| Labels | Engraved 0.6 deep in Arial Black: "7-20V" (3.6 mm) above the XT30, "INSTR" (4 mm) below the DE-9. On the hood top, "7-20V" (3 mm, Arial Black) as a 0.6 mm dark grey insert flush with the top, above the XT30 close to the connector-end edge beside the corner screw, readable from the connector end |
| Logo | MomoRC logo Ø56, a 0.6 mm dark grey insert flush with the hood top (two-colour print, the hood has the matching 0.6 mm recess), centred, upright seen from the connector end; traced from the owner's bitmap with white lines widened and black slivers removed so every feature is at least 0.8 wide |
| Vents | 2 rows × 5 slots per long side, 10 × 1.5 with R0.5 corners (Fusion feature "Vent slot corners R0.5"), 2 mm ribs, 12.5 and 16.5 above the floor top |
| Tie loops | 4 × rounded pads on the long side walls, centres X 14 and 78: 12 long × 4 out, floor underside to Y 8.5 (half-round top R6), outer edges R1.5, wall blend R1.5; vertical tunnel 5.4 × 1.9 against the wall face, open top and bottom, corners R0.5, entries rounded R0.6 (top) and R0.5 (bed side), for 4.8 mm ties |

- Colours in Fusion and STEP: base dark green, hood a lighter green. Logo insert dark grey.
- Units: millimetres. Layout frame: X from the outer face of the connector wall, Y up from the floor top, Z across.
- Fusion archive check: reopened with no external references. All 65 occurrences matched the source assembly, including the MAX490 board’s 150 bodies and placement after unlinking. Physical fit has not been tested.
- Mating plug: the [XT30PB-F](../../reference/amass-xt30-family/README.md) STEP model sits plugged onto the XT30 on the outside of the connector wall (reference component `XT30 female (reference)` in the Connector side group): mating face against the male front face (0.5 mm outside the wall), centred on the pocket, turned 180° about the plug axis from the first placement so the polarity matches the male, solder tails pointing away from the box; no intersection with the base, hood, connector plate, DE-9 or XT30 male (Fusion check). Included in the full Fusion assembly.
- Layout: inside height 20 mm. The UBEC and ring use measured dimensions. MAX490 posts, UBEC bridge and wire routing are not modelled; the hood is flat (no lower ring area or cut-off corner yet).
- Print checks: printed floor down, the DE-9 cutout top is a 19.9 mm bridge and the XT30 pocket top a 10.55 mm bridge (both were fit-tested printed flat, and the rule limit is 10 mm). The connector face has 2.7 mm of wall between the XT30 pocket and the DE-9 recess; the XT30 collar stays 0.8 mm clear of the nearer DE-9 nut pocket.
