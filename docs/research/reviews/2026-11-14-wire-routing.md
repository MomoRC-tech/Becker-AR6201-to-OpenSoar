# Internal wire routing: review and improvement plan

2026-11-14 · WP3 hardware · [routing study](../../hardware/housing.md#internal-routing), S70–S74, [route data](../../../cad/housing/housing-body-v1/routing-study.json)

Reviews the modelled harness in `housing-power-routing.f3d` v6 against four goals: clear, tidy routing; the base plate and the centre axis used as routing buses; short wires; buildable and solderable. Proposes a lane-based re-route.

> [!NOTE]
> Analysis of the route data only: power revision (6 routes), signal revision (15) and the 8 remaining baseline routes, 29 in total. Nothing was changed in CAD. The proposals need owner decisions ([section 4](#decisions)).

Frame and heights used below: X from the connector-wall outer face, Z across (ESP32 side negative, MAX490 side positive), Y up. Floor top Y −3, hood underside Y 20, PCB undersides Y 4.5, ESP32 pin tops Y 15. Lengths are CAD centrelines, not cutting lengths.

---

<a id="findings"></a>

## 1. Findings

| # | Severity | Finding | Evidence |
| :-: | :-: | --- | --- |
| 1 | 🔴 | **Zander receive wire ends on the wrong ESP32 pin.** `Mod1 R2 to ESP32 GPIO32` ends at X 43.5 on the J2 row: that is J2-13 (IO12), a boot strapping pin the [pin plan](../../hardware/modules.md#esp32-pinout) avoids. GPIO32 is J2-7, at X 58.8. The pin was probably counted from the USB end | J2-1 at X 74.0, J2-14 at 41.0 and J2-19 at 28.3 in the same data give a 2.54 mm pitch from the antenna end. GPIO26 (J2-10, X 51.1) and all J3 endpoints are correct |
| 2 | 🔴 | **Too close to the hood.** The five resistor sleeves (Ø3.2) reach Y 19.0–19.9, leaving 0.1–1.0 mm. Seven wire routes come within 1.5 mm: R-F to mod2 R3 0.25, RS-232 ground link 0.5, R-D to J2-1 0.8 mm. Real wire with slack would be pressed by the hood and rub under vibration | Route maximum height + half diameter |
| 3 | 🟠 | **Wires under the boards at solder-tail height.** Both ESP32 power branches cross under the ESP32 at Y 1.5–1.9, at the height of the header solder tails (about 3 mm below the PCB, estimate). Both MAX490 power branches run under the MAX490 at Y 1.1. They must be laid before the boards, cannot be inspected and can chafe | Route segments inside the PCB outlines below Y 4.5 |
| 4 | 🟠 | **Power doubles back.** 5 V leaves the ring (X 78) for 5V-J (X 37) and returns to MAX490 VCC (X 74): 56 + 65 mm for a pin about 10 mm from the ring. The UBEC-to-ring leads drop to the floor and back: 32 / 36 mm for 5 / 7 mm direct | Route lengths |
| 5 | 🟠 | **Long local wires across the ESP32.** R-Z and R-F float above the modules with 13–37 mm leads, although [signal wiring](../../hardware/wiring.md#zander) puts them on the pads R1 → R3 (4.8 mm apart). R-D runs 40 mm from J3-11 across the module to J2-1. The RS-232 ground link runs 38 mm from J3-7, while J2-14 (GND) is about 9 mm from both R4 pads | Route lengths, pad and pin positions |
| 6 | 🟡 | **Paths a real wire cannot follow.** Several routes change direction 8–10 times over 15–80 mm (R-Z to R3: 8 bends in 17 mm; XT30 positive: 10 bends). They dodge neighbours instead of following a lane; a real wire takes the short way and lands elsewhere | Direction changes > 20° per route |
| 7 | 🟡 | **Missing routes and a crowded junction.** DE-9 pins 4/5/8/9 → MAX490 terminals and pins 3/6/7 → GND-J are not modelled. GND-J would join six conductors at floor level between two boards 7.45 mm apart, which is hard to solder in place | Route list; [DE-9 pinout](../../hardware/wiring.md#de9-pinout) |
| 8 | 🟡 | **Fragmented route data.** The current harness is spread over the v76 baseline, v77 shift rules and two revisions. The baseline table in the housing page still quotes a 0.25 mm hood gap for a 12 V route that has been replaced | `routing-study.json`, [routing study](../../hardware/housing.md#internal-routing) |

Joints with more than one conductor: mod2 R3 takes three (3.3 V feed, link to mod1, R-F lead); J2-1, J3-7, J3-11, J3-12, mod2 R4 and the MAX490 RO/DI ends take two each.

---

<a id="concept"></a>

## 2. Proposed routing concept

Three levels and three lanes. Every wire belongs to one lane and leaves it at a station opposite its pin.

| Level or lane | Where | Carries | Rule |
| --- | --- | --- | --- |
| **L0 floor bus** | Centre channel between the ESP32 and MAX490 PCB edges (Z 4–10.6), on the floor, X 12–70 | XT30 pair (X 12–33), instrument ground, ESP32 5V/GND pair, 5V-J and GND-J | On the floor in a channel or under fingers. Power on the MAX490 half, the ESP32 pair on the ESP32 half |
| **L1 centre-axis signal bus** | Same channel, mid height (Y about 9–12) | DE-9 pins 1/2 to the module L3 pads; MAX490 RO/DI to J3-12/J3-11 | Stay on the ESP32 half (Z 4.5–6.5, clear of the J3 pins at Z 2.3); rise only at the station |
| **L2 pin-top level** | Y 15–18 | Terminations, resistors, hops up to about 15 mm | Nothing above Y 18 (at least 2 mm under the hood) |
| Connector-wall lane | X 11–17 | XT30 pair down to the floor; Becker pairs 4/5 and 8/9 to the terminal entries | Pairs stay twisted to the terminals |
| J2-side lane | Between the module R pads (Z −17.3) and the J2 header (Z −23.3) | R2 → GPIO32 and GPIO26, 3.3 V feed from J2-1, module ground | Pin-top level |
| Junction station | Channel floor, X about 58–67 | 5V-J and GND-J | Made on the bench as part of the power harness, laid in afterwards |

Each wire crosses the channel at most once: signals to the ESP32 stay on its half, power for the UBEC and MAX490 on the other. The only floor crossing under a board is the ESP32 power pair, lying on the floor (about 3 mm below the solder tails, estimate) at one station, X about 36.

---

<a id="changes"></a>

## 3. Proposed changes and target lengths

Targets are estimates from the layout; the re-route in CAD sets the real values.

| Item | Now (mm) | Proposal | Target (mm) |
| --- | ---: | --- | ---: |
| UBEC output → ring | 32 / 36 | Cut the servo plug, move the ring along the factory output lead next to the UBEC output end, rewind the same turns | 10–15 |
| Ring → 5V-J / GND-J | 56 / 26 | Junction station at the far end of the channel | 15–20 |
| Junctions → MAX490 VCC / GND | 65 / 42, under the board | Floor run around the MAX490 header end, rise at the pins | 25–30 |
| Junctions → ESP32 5V / GND | 52 / 53, at tail height | Paired floor run, one floor crossing under the ESP32, up beside the PCB edge to J2-19 / J2-14 | 65–75 |
| XT30 → UBEC input | 79 / 65 | Twisted pair; factory UBEC input leads straight into the XT30 cups (no splice); connector-wall lane, floor bus, riser at X 33, straight crossing | 65–70 |
| Instrument grounds | not modelled | Bridge DE-9 cups 3-6-7, one wire sized for the [fault case](../../hardware/power.md#wire-breaks) to GND-J | about 55 |
| Becker pairs 4/5, 8/9 | not modelled | Two twisted pairs in the connector-wall lane | 10–25 |
| R-Z, R-F | 88 in total | On the pads R1 → R3, leads bent over R2, DIN0204-size body | leads only |
| R-D | 47 | J3-11 → mod2 L2 (same 3.3 V net as R3) | 8–10 |
| RS-232 ground | 38 + 31 | J2-14 → mod2 R4 → mod1 R4 | 10 + 13 |
| 3.3 V feed | 38 + 23 | J2-1 → mod2 R3 in the J2-side lane, then mod1 | 32 + 13 |
| Zander receive | 23, wrong pin | Mod1 R2 → J2-7 in the J2-side lane | about 30 |

Power distribution from the UBEC output drops from about 360 to about 250 mm; local signal and supply wiring from about 480 to about 310 mm.

<a id="build-order"></a>

### 3.1 Build order the routing should support

Every joint is either made on the bench or reachable from above with the hood off. No joint at floor level between boards.

- [ ] **Bench:** XT30 with the factory UBEC input leads; DE-9 pigtail with the 3-6-7 bridge, fitted from the front through the cutout; ring moved and rewound; power harness with 5V-J and GND-J splices; RS-232 modules with pull-ups, links and flying leads, then slid into the clamp.
- [ ] **Floor:** lay the floor bus and the ESP32 power pair; fix them in the channel.
- [ ] **Boards:** fit the ESP32; fit the MAX490 and solder its header ends (VCC, GND, RO, DI) before the carrier.
- [ ] **Signals:** Becker pairs into the terminals; RO/DI with R-B1, R-B2, R-D at J3; fit the clamp; solder the module leads to J2.
- [ ] **Top:** UBEC carrier, ring in the cradle, ties; check the hood clearance and the service slack for lifting the carrier and the clamp.

---

<a id="decisions"></a>

## 4. Decisions for the owner

One decision at a time; the recommended option first.

| # | Decision | Options | Recommendation |
| :-: | --- | --- | --- |
| D1 | Routing concept | Levels and lanes ([section 2](#concept)); keep free routing | Levels and lanes |
| D2 | Junction form and place | Bench-made inline splices at the far-end station; printed junction block; small two-rail strip board in a floor pocket; current mid-channel position | Bench-made splices at the far end |
| D3 | Instrument grounds | Bridge DE-9 cups 3-6-7 with one wire to GND-J; three separate wires | Bridge |
| D4 | Ferrite position | Move the ring along the factory output lead; keep the factory spacing and store the slack | Move the ring |
| D5 | Pull-ups R-Z, R-F | On the pads R1 → R3 with a DIN0204-size body; flying leads as modelled | On the pads, DIN0204 |
| D6 | Local 3.3 V and ground taps | RS-232 ground from J2-14 and R-D from mod2 L2; keep J3-7 and J2-1 | J2-14 and mod2 L2 |
| D7 | Wire types and gauges | Factory UBEC leads for power; fault-path ground checked for the 3.2 A case; 26–28 AWG PTFE-insulated wire for signals (insulation survives dense soldering) | As listed; selection in housing open item 14 |
| D8 | Wire retention | After the re-route, per housing open items 18–20 | Decide after D1–D7 |

D3 and D6 change the [supply path](../../hardware/power.md#supply) and D5 the resistor size in the [parts list](../../hardware/modules.md#parts-list); record each answer on its owning page.

---

<a id="plan"></a>

## 5. Work plan

| Step | Work | Done when |
| :-: | --- | --- |
| 1 | Move the Zander receive endpoint to J2-7 and check every pin endpoint against the [pin table](../../hardware/modules.md#esp32-pinout) (J2/J3 position n at X = 74.0 − (n − 1) × 2.54) | All endpoints match; no decision needed |
| 2 | Owner decisions D1–D7 | Answers recorded on the owning pages |
| 3 | Model the lanes, junction station and exit stations as a hideable `Routing lanes` component | Lanes clear boards, posts, carrier screwdriver paths and floor; hood gap at least 2 mm |
| 4 | Re-route power on the lanes | No run under a board at tail height; lengths near section 3 |
| 5 | Re-route signals and resistors; add the DE-9 ground and Becker pairs | Every route in a lane; no joint with more than two wires unless made on the bench |
| 6 | One consolidated route list and a harness table: from → to, colour, gauge, lane, routed length, strip allowance, slack, cutting length, build step | Housing page and route data show only the current harness |
| 7 | Retention features (housing open items 16, 18–20); fit-print a base slice; build the harness on the bench from the table | Physical fit and slack checked (housing open items 14, 21–23) |

Fusion steps follow the `momorc-fusion360-workflow` skill and the [CAD rule](../../hardware/housing.md#rules).

---

## Verification

Route data parsed with a script: centreline lengths, direction changes, highest point against the hood underside, segments inside the PCB outlines below Y 4.5, shared endpoints. Pin positions were checked against the module pin table. The live Fusion model, actual wire stiffness and bend radii, solder-tail length and physical fit were not checked.
