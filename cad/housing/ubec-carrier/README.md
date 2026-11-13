# UBEC carrier

Four-screw carrier above the MAX490 board, with two cable ties holding the UBEC. Dimensions, fasteners and assembly instructions: [housing § 2.4](../../../docs/hardware/housing.md#ubec-carrier).

![Carrier in print orientation, UBEC contact face down and four full-height mounting legs with vertical holes](preview-print.png)

| File | Use |
| --- | --- |
| [ubec-carrier.f3d](ubec-carrier.f3d) | Editable Fusion part, 27 timeline items |
| [ubec-carrier.step](ubec-carrier.step) | Solid model |
| [ubec-carrier.3mf](ubec-carrier.3mf) | Print mesh, millimetres, contact face on Z = 0; print as placed |
| [Housing assembly](../housing-body-v1/housing-assembly.f3d) | Installed carrier with flush, offset screw seats, raised UBEC, updated floor posts and tie fit envelopes |

![Carrier with flush screw seats, installed UBEC and cable-tie envelopes](preview-installed.png)

- Print in PETG with the housing settings, solid walls and **supports off**. The full-height legs start on the bed in the supplied orientation ([print dimensions](../../../docs/hardware/housing.md#ubec-carrier)). Support-free printing is intended; the part has not been sliced or printed.
- The assembly omits the four screw reference solids; the shared screws are still required for assembly. Tie heads are assumed 4 × 4 × 3 mm envelopes, not a selected manufacturer's model.
- **Verification, 2026-11-13:** Fusion recompute and all affected features passed. The carrier reopened as one solid with 27 timeline items; the base reopened as one solid with 65. The assembly reopened with 67 occurrences and 279 timeline items; all 482 body names, volumes and bounds match the live model. All 480 bodies outside the carrier/base retain their pre-edit geometry. The four Ø4.4 mm head envelopes and all added carrier/post material clear other bodies; the offset axes, floor thickness and screw stack match the dimensions in housing § 2.4. The STEP imports each contain one solid with the correct offset holes; volumes agree with Fusion within 0.001 %. The carrier and base meshes have closed edges, no degenerate triangles and Z = 0 bed placement. Existing base-archive engraving warnings and detailed checks: S53 in the [source register](../../../docs/research/sources.md#source-register). Slicing, physical fit, thread holding, vibration and loaded temperature were not tested. Fit checks: [open items](../../../docs/project/open-items.md#housing).
