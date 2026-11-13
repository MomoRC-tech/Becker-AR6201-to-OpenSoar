# UBEC carrier

Four-screw carrier above the MAX490 board, with two cable ties holding the UBEC. Dimensions, fasteners and assembly instructions: [housing § 2.4](../../../docs/hardware/housing.md#ubec-carrier).

![Carrier in print orientation, UBEC contact face down and the four mounting seats upwards](preview-print.png)

| File | Use |
| --- | --- |
| [ubec-carrier.f3d](ubec-carrier.f3d) | Editable Fusion part, 24 timeline items |
| [ubec-carrier.step](ubec-carrier.step) | Solid model |
| [ubec-carrier.3mf](ubec-carrier.3mf) | Print mesh, millimetres, contact face on Z = 0; print as placed |
| [Housing assembly](../housing-body-v1/housing-assembly.f3d) | Installed carrier with open screw seats, raised UBEC, updated floor posts and tie fit envelopes |

![Carrier with open screw seats, installed UBEC and cable-tie envelopes](preview-installed.png)

- Print in PETG with the housing settings, solid walls and **support beneath all four projecting screw-seat pads** ([print dimensions](../../../docs/hardware/housing.md#ubec-carrier)). The part has not been sliced or printed.
- The assembly omits the four screw reference solids; the shared screws are still required for assembly. Tie heads are assumed 4 × 4 × 3 mm envelopes, not a selected manufacturer's model.
- **Verification, 2026-11-13:** Fusion recompute and all seven added features passed. The carrier reopened as one solid with 24 timeline items; the base reopened with both R1 post-root fillets. The assembly reopened with 67 occurrences and 276 timeline items; all 482 body names, volumes and bounds match the live model. The carrier and base meshes have closed edges after welding coincident vertices and sit at Z = 0. Screw holes and bearing-pad thickness are unchanged; the new fillet envelopes clear other bodies. Existing base-archive engraving warnings and detailed checks: S53 in the [source register](../../../docs/research/sources.md#source-register). Slicing, physical fit, thread holding, vibration and loaded temperature were not tested. Fit checks: [open items](../../../docs/project/open-items.md#housing).
