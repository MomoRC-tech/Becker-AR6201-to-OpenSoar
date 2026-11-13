# Connector wall print test

Two short slices of the [housing body v1](../housing-body-v1/README.md) base, cut unchanged from `housing-base.step` (commit d7a259c): the connector wall with its floor strip, collar, nut pockets, engraved labels and the corner posts, 11.5 mm deep from the outer face, tie loops cut off. They check the XT30 and DE-9 openings as the base prints them — standing, with the 10.55 and 19.9 mm bridges — before printing the whole base. Rules: [housing § 2](../../../docs/hardware/housing.md#rules).

| File | Slice (layout Z, mm) | Checks |
| --- | --- | --- |
| `xt30-print-test.3mf` / `.step` | −29.5 to −8.6 (side wall to between the openings) | XT30 pocket 10.55 × 5.25 with chamfer fill, collar, "7-20V" label, XT30 corner post with insert hole |
| `de9-print-test.3mf` / `.step` | −8.6 to 34.5 (between the openings to the other side wall) | DE-9 flange recess 31.6 × 13.2 × 1.0, D-shaped cutout, Ø3.4 holes, nut pockets, "INSTR" label, far corner post |

- **Print:** like the base: floor down, same PETG and settings, no supports. The 3MF files are already in that orientation (Z up); the STEP files are rotated the same way.
- **Material:** about 2.9 and 3.7 cm³.

Checklist after printing:

- [ ] XT30 goes in front first from the inside, raised markings stop at the inner face, press fit holds, plug mates and unplugs.
- [ ] Top bridge of the XT30 pocket clean, no sag into the opening.
- [ ] DE-9 flange sits flush in the recess, rear body passes the D-cutout without forcing, the 19.9 mm bridge clean.
- [ ] Jackposts through the holes, nuts drop into the pockets, mated cable plug locks.
- [ ] Engraved labels readable.
