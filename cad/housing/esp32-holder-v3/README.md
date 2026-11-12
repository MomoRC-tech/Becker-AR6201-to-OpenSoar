# ESP32 holder v3 (RS-232 clamp)

ESP32 holder variant for [layout v4](../layout-v4/README.md): the tested USB block, the antenna cap without foot ears, and a USB clamp that also carries both RS-232 boards above the ESP32. One M2.5 screw removes the clamp with both boards. Rules: [housing § 5](../../../docs/hardware/housing.md#layout). Built in Autodesk Fusion through MCP, 2026-11-12, from [esp32-holder](../esp32-holder/README.md) and [rs232-holder](../rs232-holder/README.md); not printed yet.

| File | Use |
| --- | --- |
| `esp32-holder-v3.f3d` | Fusion archive, standalone (RS-232 slot links broken into local copies); open with *File › Open › Open from my computer* |
| `esp32-holder-v3.3mf` | Print-ready mesh (high refinement) |
| `esp32-holder-v3.step` | Solid model for checking dimensions or re-slicing |

| Piece | Colour in Fusion | Change from the tested holder |
| --- | --- | --- |
| A antenna cap | 🔵 Blue | Foot ears outside the side walls removed: 31.9 wide instead of 38.5; end corners rounded (R1.5). Tunnel on top of the spring bridge for the clamp arm, open only towards the clamp: 6.7 inner width, 1.2 walls and roof, 4.3 long (1.5 of it beyond the bridge, without floor); top 15.4 above the housing floor |
| B USB screw block | 🟠 Orange | Unchanged |
| UR USB clamp with RS-232 slots | 🟢 Green | USB cap with two RS-232 slots (1.25 mm, 9.0 grip) on top, one behind the other, rails running across the ESP32; slot bottoms 9.85 above the housing floor, boards at 13.8; first slot starts just behind the screw head. Arm (6 × 2 mm, 25.7 mm long, 11.9–13.9 above the floor) from the clamp's free end into the antenna-cap tunnel |
| J gluing jig | 🟡 Yellow | Unchanged |

- Units: millimetres. The 3MF has every piece in print orientation; the clamp is flipped there to print top up (slots and arm up). The Fusion archive and STEP keep the clamp in the U cap's orientation (top face down), because layout v4 places the pieces from it. Check bridging over the USB cavity and the slot height (now set by layers) in a fit test. Support only under two flat faces: the slot plate (3.6 mm above the bed) and the arm (5.6 mm above the bed, 26 × 6 mm). The antenna cap needs no support.
- Slide the pre-soldered RS-232 boards into the clamp before screwing it on; the ESP32 header pins block them afterwards.
