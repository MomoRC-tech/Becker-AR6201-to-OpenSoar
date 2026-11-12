# Mechanical fit coupons

Three independent PETG prints modelled in Fusion. Start with the 3MF files in the supplied orientation (Z up, all parts flat on the bed). The STEP files contain the same solids. Use the housing's [print starting values](../../../docs/hardware/housing.md#print-values) and preview the front plate's recessed DE-9 flange bridge in the slicer.

| Coupon | Print | Check on the bench |
| --- | --- | --- |
| ESP32 | [3MF](esp32-holder.3mf) · [STEP](esp32-holder.step) | Slide the antenna-end PCB shoulders into the two fixed pockets, lower the USB end, then fit both keeper tabs with M2.5 × 6 screws. Check the pins and underside joints stay clear, the PCB stays flat, the USB plug mates and the buttons remain accessible. The screws should seat the tabs on the bosses without bending the PCB. Wiggle the board and insert and remove the USB plug with the hood absent. |
| RS-232 | [3MF](rs232-holder.3mf) · [STEP](rs232-holder.step) | Try each 9.2 × 15.6 mm board in the same saddle. Slip one long edge under the fixed lip, lower the board and fit the keeper with an M2.5 × 6 screw. Inspect the two narrow edge contacts and the end stops against the real board before tightening. Check both pad rows and the soldered leads remain accessible and the board cannot shift. |
| Front plate | [3MF](front-plate.3mf) · [STEP](front-plate.step) | Seat the DE-9 from the outside, check the rear nut pockets and full jackpost thread engagement, then mate both plugs to check backshell and tool clearance. Insert the XT30 front first from the back. Check the marking stop, face protrusion, mating and pull-out. The right end is reserved for the fuse holder. |

## Trial dimensions

- **ESP32:** 59 × 31 mm tray. The 1.70 mm capture gap gives 0.20 mm nominal clearance on the measured 1.50 mm board. Both screws stop on the tray bosses. Antenna fingers are 1.35 mm wide with 1.05 mm overlap. USB tabs overlap by 1.10 mm and leave 10.10 mm between them. PCB underside is 3.50 mm above the floor.
- **RS-232:** 17.1 × 20.8 mm saddle. A 1.35 mm capture gap gives 0.20 mm nominal clearance on the 1.15 mm PCB. Each keeper lip overlaps 0.50 mm in the middle of a long edge. The 0.80 mm wide end stops target the gap between the middle pads. Verify those contact patches with the board and soldered leads in hand.
- **Front plate:** 86 × 25 mm with a 3.0 mm wall, a 31.6 × 13.2 × 1.0 mm DE-9 flange recess, 19.9 × 11.5 mm rear opening, two Ø3.4 mm holes on 25 mm centres and 6.65 mm across-flats rear nut pockets. The XT30 collar is 8.0 mm deep from the front face with a 10.75 × 5.45 mm throat before chamfers. This coupon tests the friction fit. The housing's rear retention rib and the fuse cutout are separate features.

Fusion solid counts and the exported 3MF meshes were checked. PETG printing, physical fit and connector retention are bench checks.
