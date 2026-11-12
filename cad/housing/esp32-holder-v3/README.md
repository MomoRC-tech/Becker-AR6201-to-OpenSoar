# ESP32 holder v3 (RS-232 clamp)

ESP32 holder variant for [layout v4](../layout-v4/README.md): the tested USB block, the antenna cap without foot ears, and a USB clamp that also carries both RS-232 boards above the ESP32. One M2.5 screw removes the clamp with both boards. Rules: [housing § 5](../../../docs/hardware/housing.md#layout). Built in Autodesk Fusion through MCP, 2026-11-12, from [esp32-holder](../esp32-holder/README.md) and [rs232-holder](../rs232-holder/README.md); not printed yet.

| File | Use |
| --- | --- |
| `esp32-holder-v3.f3d` | Fusion archive, standalone (RS-232 slot links broken into local copies); open with *File › Open › Open from my computer* |
| `esp32-holder-v3.3mf` | Print-ready mesh (high refinement) |
| `esp32-holder-v3.step` | Solid model for checking dimensions or re-slicing |

| Piece | Change from the tested holder |
| --- | --- |
| A antenna cap | Foot ears outside the side walls removed: 31.9 wide instead of 38.5; end corners rounded (R1.5). U-channel on top of the spring bridge for the clamp arm: 8.8 wide, walls 1.2, 6.4 inner width, closed at the antenna end |
| B USB screw block | Unchanged |
| UR USB clamp with RS-232 slots | USB cap with two RS-232 slots (1.25 mm, 9.0 grip) on top, one behind the other, rails running across the ESP32; slot bottoms 9.85 above the housing floor, boards at 13.8; first slot starts just behind the screw head. Arm (6 × 2 mm, 26 mm long, 11.4–13.4 above the floor) from the clamp's free end into the antenna-cap channel |
| J gluing jig | Unchanged |

- Units: millimetres. The clamp pieces are still in the U cap's print orientation (top face down); with the slots on top it must print top up instead: check bridging over the USB cavity and the slot height (now set by layers) in a fit test. The arm needs support underneath, like the slot plate.
- Slide the pre-soldered RS-232 boards into the clamp before screwing it on; the ESP32 header pins block them afterwards.
