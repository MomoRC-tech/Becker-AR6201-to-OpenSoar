# ESP32 holder v2 (RS-232 integrated)

ESP32 holder variant used by the selected [layout v3](../layout-v3/README.md): the antenna cap without foot ears, and a USB block with both RS-232 slot holders side by side ahead of it, joined by a 1.5 mm base strip. Rules and fit notes: [housing § 5](../../../docs/hardware/housing.md#layout). Built in Autodesk Fusion through MCP, 2026-11-12, from the tested [esp32-holder](../esp32-holder/README.md) and [rs232-holder](../rs232-holder/README.md); not printed yet.

| File | Use |
| --- | --- |
| `esp32-holder-v2.f3d` | Fusion archive, standalone (RS-232 slot links broken into local copies); open with *File › Open › Open from my computer* |
| `esp32-holder-v2.3mf` | Print-ready mesh (high refinement), pieces in print orientation |
| `esp32-holder-v2.step` | Solid model for checking dimensions or re-slicing |

| Piece | Change from the tested holder |
| --- | --- |
| A antenna cap | Foot ears outside the side walls removed: 31.9 wide instead of 38.5; foot ends now square |
| BR USB block with RS-232 slots | Two RS-232 slots (1.25 mm, 9.0 grip) side by side, centred ahead of the USB block; boards end 3.3 mm in front of the boss |
| U USB cap, J gluing jig | Unchanged |

- Units: millimetres; Z is the print direction. The RS-232 slots now print lying down with the USB block, so the slot height comes from the 0.2 mm layers: fit-test before use.
