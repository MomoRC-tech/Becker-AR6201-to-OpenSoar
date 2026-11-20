"""Generate the bridge's hardware diagrams in one consistent style.

Usage (from the repository root):  python tools/diagrams.py
Writes the SVGs into docs/images. Edit the diagram sections below, run the
script, and check the result in a browser before committing.

Output: teaser.svg (README top), overview.svg (spare, not linked), power-supply.svg, ground-loops.svg, ground-distribution.svg, wiring-overview.svg,
wiring-zander.svg, wiring-flarm.svg, wiring-becker.svg, pinout-rs422-to-ttl.svg, rs422-to-ttl-board.svg.
"""
import base64
import io
import os
import sys
from xml.sax.saxutils import escape

OUT = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "docs", "images")

STYLE = {
    "v12":    dict(color="#d32f2f", dash=None, width=2.5, label="+12 V supply"),
    "v5":     dict(color="#ef6c00", dash=None, width=2.5, label="5 V"),
    "v33":    dict(color="#8e24aa", dash=None, width=2.5, label="3.3 V"),
    "data":   dict(color="#37474f", dash=None, width=2.5, label="Data (arrow = direction)"),
    "gnd":    dict(color="#171717", dash=None, width=2.5, label="Bridge ground"),
    "sgnd":   dict(color="#171717", dash="7 5", width=2.5, label="Signal ground"),
    "shield": dict(color="#9e9e9e", dash="2 4", width=2.5, label="Cable shield"),
    "wifi":   dict(color="#00897b", dash="8 5", width=2.5, label="Wi-Fi"),
    "sense":  dict(color="#607d8b", dash=None, width=2.0, label="Supply voltage sense"),
}
INK, SUB = "#172b4d", "#475569"


class Svg:
    def __init__(self, w, h, title, desc):
        self.w, self.h, self.title, self.desc = w, h, title, desc
        # Drawing order: group backgrounds, lines, arrows/dots, boxes, text.
        self.groups, self.lines, self.marks, self.boxes, self.texts = [], [], [], [], []

    def line(self, pts, kind, width=None):
        st = STYLE[kind]
        d = "M" + " L".join(f"{x} {y}" for x, y in pts)
        dash = f' stroke-dasharray="{st["dash"]}"' if st["dash"] else ""
        self.lines.append(f'<path d="{d}" stroke="{st["color"]}" stroke-width="{width or st["width"]}" fill="none"{dash}/>')

    def arrow(self, x, y, direction, kind):
        p = {"down": [(x-5, y-4), (x+5, y-4), (x, y+5)], "up": [(x-5, y+4), (x+5, y+4), (x, y-5)],
             "left": [(x+4, y-5), (x+4, y+5), (x-5, y)], "right": [(x-4, y-5), (x-4, y+5), (x+5, y)]}[direction]
        pts = " ".join(f"{a},{b}" for a, b in p)
        self.marks.append(f'<polygon points="{pts}" fill="{STYLE[kind]["color"]}"/>')

    def resistor(self, cx, cy, kind, vertical=False):
        """IEC resistor symbol centred on a line; the white fill hides the line inside it."""
        w, h = (10, 26) if vertical else (26, 10)
        self.boxes.append(f'<rect x="{cx - w / 2}" y="{cy - h / 2}" width="{w}" height="{h}" fill="white" stroke="{STYLE[kind]["color"]}" stroke-width="2"/>')

    def ground(self, x, y):
        """Ground symbol hanging below (x, y)."""
        c = STYLE["gnd"]["color"]
        for dy, half in ((0, 9), (4, 6), (8, 3)):
            self.lines.append(f'<path d="M{x - half} {y + dy} H{x + half}" stroke="{c}" stroke-width="2"/>')

    def dot(self, x, y, kind, r=4):
        self.marks.append(f'<circle cx="{x}" cy="{y}" r="{r}" fill="{STYLE[kind]["color"]}"/>')

    def box(self, x, y, w, h, title, subs=(), group=False):
        if group:
            self.groups.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="8" fill="#f7f9fc" stroke="#9cb1cc" stroke-width="1.5" stroke-dasharray="6 4"/>')
            self.text(x + 12, y + 20, title, bold=True, anchor="start", size=13)
            return
        self.boxes.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="5" fill="#eaf1fb" stroke="#9cb1cc" stroke-width="1.5"/>')
        titles = [title] if isinstance(title, str) else list(title)  # several bold lines allowed
        n = len(titles) + len(subs)
        top = y + h / 2 - (n - 1) * 9 + 5
        for i, t in enumerate(titles):
            self.text(x + w / 2, top + 18 * i, t, bold=True, size=14)
        for i, s in enumerate(subs):
            self.text(x + w / 2, top + 18 * (len(titles) + i), s, color=SUB, size=12)

    def text(self, x, y, s, color=INK, size=12, bold=False, anchor="middle"):
        b = ' font-weight="700"' if bold else ""
        self.texts.append(f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" text-anchor="{anchor}"{b}>{escape(s)}</text>')

    def tag(self, x, y, s, kind, anchor="middle", bold=True, size=12):
        self.text(x, y, s, color=STYLE[kind]["color"], size=size, bold=bold, anchor=anchor)

    def legend(self, y, kinds, note=None):
        """kinds: style names, or (style name, label) to override the label."""
        x = 30
        for k in kinds:
            k, label = k if isinstance(k, tuple) else (k, STYLE[k]["label"])
            st = dict(STYLE[k], label=label)
            dash = f' stroke-dasharray="{st["dash"]}"' if st["dash"] else ""
            self.lines.append(f'<path d="M{x} {y} H{x+30}" stroke="{st["color"]}" stroke-width="2.5"{dash}/>')
            self.text(x + 36, y + 4, st["label"], anchor="start")
            x += 36 + 7 * len(st["label"]) + 34
        if note:
            self.text(30, y + 24, note, color=SUB, anchor="start")

    def save(self, name):
        body = "\n".join(self.groups + self.lines + self.marks + self.boxes + self.texts)
        svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{self.w}" height="{self.h}" viewBox="0 0 {self.w} {self.h}" role="img" aria-labelledby="title desc">\n'
               f'<title id="title">{escape(self.title)}</title>\n<desc id="desc">{escape(self.desc)}</desc>\n'
               f'<rect width="{self.w}" height="{self.h}" fill="white"/>\n<g font-family="Arial,sans-serif">\n{body}\n</g>\n</svg>\n')
        io.open(os.path.join(OUT, name), "w", encoding="utf-8", newline="\n").write(svg)


# ---------------------------------------------------------------- supply path
s = Svg(1180, 420, "Bridge supply distribution",
        "UBEC output through the existing ferrite to 5V-J, with separate branches to ESP32 and MAX490. ESP32 3.3 V feeds RS-232-mod2 then mod1, electrically in parallel. The 12 V input feeds the voltage divider R-V1 100 kOhm and R-V2 10 kOhm with C-V 100 nF to GPIO34. Grounds are shown separately.")
for x,w,title,subs in [(20,120,"F1 fuse",["harness, supply end"]),(175,125,"12 V harness",["about 22 AWG"]),(335,80,"XT30",[]),(450,110,"UBEC",["5 V, 3 A"])]:
    s.box(x,45,w,56,title,subs)
for x1,x2 in [(140,175),(300,335),(415,450)]:
    s.line([(x1,73),(x2,73)],"v12");s.arrow((x1+x2)/2,73,"right","v12")
s.box(660,35,170,90,"ESP32",["5V input J2-19","3V3 output J2-1"])
s.box(660,195,170,56,"MAX490",["VCC"])
s.line([(560,73),(660,73)],"v5");s.dot(610,73,"v5")
s.line([(610,73),(610,223),(660,223)],"v5")
s.tag(605,145,"5V-J","v5",anchor="end");s.tag(605,162,"separate branches","v5",anchor="end",bold=False,size=11)
s.text(515,120,"Output leads through ferrite",size=11,color=SUB)
s.line([(830,80),(850,80),(850,35),(960,35)],"v33")
# The physical chain terminates at mod2 R3, then continues to mod1 R3.
s.line([(960,35),(935,35),(935,165),(960,165)],"v33");s.dot(935,35,"v33")
s.tag(943,116,"R3 chain","v33",anchor="start",size=11)
for name,top in [("RS-232-mod2",20),("RS-232-mod1",150)]:
    r3=top+15
    s.box(960,top,140,60,name)
    s.text(966,r3+4,"R3",size=11,color=SUB,anchor="start")
# Supply voltage sense: R-V1 at the 12 V input, R-V2 and C-V at the ESP32 header (power.md, voltage sense).
s.dot(420,73,"v12");s.line([(420,73),(420,153)],"v12");s.resistor(420,140,"v12",vertical=True)
s.text(433,144,"R-V1 100 kΩ",anchor="start",size=11,color=SUB)
s.line([(420,153),(420,300)],"sense");s.dot(420,215,"sense")
s.resistor(420,255,"sense",vertical=True);s.text(433,259,"R-V2 10 kΩ",anchor="start",size=11,color=SUB)
s.line([(420,215),(358,215),(358,251)],"sense");s.line([(358,259),(358,300)],"sense")
for y in (251,259):
    s.lines.append(f'<path d="M346 {y} H370" stroke="{STYLE["sense"]["color"]}" stroke-width="3"/>')
s.text(341,259,"C-V 100 nF",anchor="end",size=11,color=SUB)
s.ground(358,300);s.ground(420,300);s.line([(358,300),(420,300)],"gnd")
s.text(389,330,"J2-14, GND-J",size=11,color=SUB)
s.line([(420,215),(545,215)],"sense");s.arrow(545,215,"right","sense")
s.tag(486,207,"GPIO34, J2-5","sense",size=11)
s.legend(370,["v12","v5","v33","sense"],"RS-232 boards: 3.3 V, electrically parallel. GND-J is separate from 5V-J; see the ground tree.")
s.save("power-supply.svg")

# ------------------------------------------------------ internal ground tree
s = Svg(1120,490,"Internal ground tree",
        "UBEC output ground through the ferrite to GND-J. Separate branches to ESP32 and MAX490, and one instrument-ground wire to the DE-9, where cups 3, 6 and 7 are bridged for the Becker, Zander and FLARM signal grounds. GND-J reaches the ESP32 at J2-14. ESP32 J3-1 is used only to feed RS-232-mod2 R4 then mod1 R4. R-B2 remains local. No additional module-to-junction or MAX490-to-ESP32 ground links.")
s.box(20,185,170,75,"UBEC output GND",["through existing ferrite"])
s.box(245,185,150,75,"GND-J",["bench-made splice"])
s.line([(190,222),(245,222)],"gnd")
s.line([(395,222),(450,222)],"gnd");s.line([(450,65),(450,305)],"gnd",width=4);s.dot(450,222,"gnd")
branches=[(65,"ESP32",["J2-14 main return","J3-1 module return"]),(145,"MAX490",["own GND branch"]),(305,"DE-9 ground bridge",["cups 3, 6, 7; one wire"])]
for y,title,subs in branches:
    s.line([(450,y),(510,y)],"gnd");s.dot(450,y,"gnd");s.box(510,y-27,190,54,title,subs)
s.line([(700,305),(730,305)],"gnd");s.line([(730,242),(730,382)],"gnd",width=4);s.dot(730,305,"gnd")
for y,title,subs in [(242,"Becker signal GND",["pin 3, J1-21"]),(312,"Zander signal GND",["pin 6"]),(382,"FLARM signal GND",["pin 7"])]:
    s.line([(730,y),(760,y)],"gnd");s.dot(730,y,"gnd");s.box(760,y-27,190,54,title,subs)
s.box(760,38,150,54,"RS-232-mod2",["R4"]);s.box(950,38,150,54,"RS-232-mod1",["R4"])
s.line([(700,65),(760,65)],"gnd");s.tag(730,30,"J3-1","gnd",size=11)
s.line([(910,65),(950,65)],"gnd");s.tag(930,30,"R4 link","gnd",size=11)
s.text(790,120,"R-B2 stays on ESP32 J3-7.",anchor="start",size=12)
s.text(790,145,"No extra RS-232 L1 ground wires.",anchor="start",size=12)
s.text(790,170,"5V-J is a separate electrical node.",anchor="start",size=12)
s.legend(450,["gnd"],"Internal wiring tree only. Instrument supply grounds still create external return paths; see ground-loops.svg.")
s.save("ground-distribution.svg")

# ------------------------------------------------------------- signal overview
s = Svg(1020, 380, "Bridge signal overview",
        "Zander ZS1 to RS-232-mod1 to ESP32 UART0 (receive only). Classic FLARM to RS-232-mod2 to ESP32 UART1 (receive only). "
        "Becker AR6201 to and from the MAX490 RS-422 board and ESP32 UART2 in both directions. The RS-232 transmitter inputs are unconnected. RO through R-B1 to GPIO16 with R-B2 to ground; GPIO17 straight to DI. The ESP32 talks to OpenSoar over Wi-Fi.")
s.box(20, 50, 150, 56, "Zander ZS1", ["RS-232"])
s.box(20, 145, 150, 56, "Classic FLARM", ["RS-232"])
s.box(20, 240, 150, 66, "Becker AR6201", ["RS-422, J1"])
s.box(300, 50, 170, 56, "RS-232-mod1", ["MAX3232, 3.3 V"])
s.box(300, 145, 170, 56, "RS-232-mod2", ["MAX3232, 3.3 V"])
s.box(300, 240, 170, 66, "RS-422 board", ["MAX490, 5 V"])
s.box(660, 40, 130, 276, "ESP32", ["Wi-Fi bridge"])
s.box(870, 148, 130, 56, "OpenSoar", ["Android"])
for y, a, b in ((78, "TX → L3", "R1 → GPIO32 · UART0"), (173, "pin 5 TX → L3", "R1 → GPIO26 · UART1")):
    s.line([(170, y), (300, y)], "data"); s.arrow(235, y, "right", "data")
    s.line([(470, y), (660, y)], "data"); s.arrow(565, y, "right", "data")
    s.tag(235, y - 8, a, "data", bold=False); s.tag(565, y - 8, b, "data", bold=False)
s.line([(170, 262), (300, 262)], "data"); s.arrow(235, 262, "right", "data")
s.line([(300, 286), (170, 286)], "data"); s.arrow(235, 286, "left", "data")
# GPIO17 straight to DI
s.line([(660, 262), (470, 262)], "data"); s.arrow(495, 262, "left", "data")
# RO through R-B1 to GPIO16, R-B2 to ground
s.line([(470, 286), (660, 286)], "data"); s.arrow(648, 286, "right", "data"); s.resistor(530, 286, "data")
s.dot(600, 286, "data"); s.line([(600, 286), (600, 326)], "data"); s.resistor(600, 306, "data", vertical=True); s.ground(600, 326)
s.tag(530, 302, "R-B1", "data", bold=False, size=11); s.tag(608, 310, "R-B2", "data", anchor="start", bold=False, size=11)
s.tag(235, 254, "TX2± → A/B", "data", bold=False); s.tag(235, 302, "RX2± ← Y/Z", "data", bold=False)
s.tag(550, 254, "DI ← GPIO17 · UART2", "data", bold=False, size=11)
s.tag(486, 280, "RO", "data", anchor="start", bold=False, size=11); s.tag(632, 280, "GPIO16", "data", bold=False, size=11)
s.line([(790, 176), (870, 176)], "wifi"); s.arrow(860, 176, "right", "wifi"); s.arrow(800, 176, "left", "wifi")
s.tag(830, 166, "one TCP path", "wifi", bold=False); s.tag(830, 196, "per device", "wifi", bold=False)
s.legend(345, ["data", "wifi"], "Zander and FLARM: receive only. Becker: both directions. Pins and resistor values: see each device section.")
s.save("wiring-overview.svg")

# -------------------------------------------------------------- RS-232 links
def rs232_device(name,dev,tx_pin,gpio,source,fname):
    s=Svg(930,365,f"{dev} through {name}",f"Instrument TX to L3, R1 to {gpio}; instrument ground to GND-J through the DE-9 ground bridge. Module R3/R4 supplied through {source}. L1 has no extra ground wire.")
    s.box(20,40,170,130,dev,["existing harness / DE-9"])
    s.box(330,40,240,130,name,["RS-232 / 3.3 V TTL"])
    s.box(720,40,190,90,"ESP32",[gpio])
    s.line([(190,65),(330,65)],"data");s.arrow(260,65,"right","data");s.tag(260,57,f"{tx_pin} → L3","data")
    s.line([(570,65),(720,65)],"data");s.arrow(690,65,"right","data");s.tag(645,57,f"R1 → {gpio}","data")
    s.box(330,210,240,65,"Central GND-J",["via DE-9 ground bridge"])
    s.line([(190,140),(250,140),(250,242),(330,242)],"sgnd");s.tag(245,132,"signal GND","sgnd",anchor="end",size=11)
    s.box(720,210,190,80,source,["3V3 / R3","GND / R4"])
    s.line([(720,235),(675,235),(675,115),(570,115)],"v33");s.tag(580,108,"R3","v33",anchor="start",size=11)
    s.line([(720,270),(650,270),(650,160),(570,160)],"gnd");s.tag(580,155,"R4","gnd",anchor="start",size=11)
    s.legend(320,["data","sgnd","gnd",("v33","3.3 V")],"Receive only. Supply/return chain: ESP32 → mod2 → mod1; boards remain electrically parallel.")
    s.save(fname)
rs232_device("RS-232-mod1","Zander ZS1","TX","GPIO32","RS-232-mod2","wiring-zander.svg")
rs232_device("RS-232-mod2","Classic FLARM","pin 5 TX","GPIO26","ESP32 J2-1 / J3-1","wiring-flarm.svg")

# --------------------------------------------------------------------- Becker
s = Svg(980, 300, "Becker to RS-422 board to ESP32",
        "Becker J1 TX2+ and TX2- (pins 2, 9) to MAX490 receiver inputs A and B; MAX490 driver outputs Y and Z to J1 RX2+ and RX2- (pins 3, 10). "
        "J1-21 D_GND to GND-J through the DE-9 ground bridge, without a series resistor. Cable shield to J1-11 at the radio end only. Receiver output RO through R-B1 (2.2 kΩ) to GPIO16, "
        "with R-B2 (3.3 kΩ) from GPIO16 to GND. GPIO17 straight to driver input DI; the board's own pull-up holds DI high in reset. A-B not terminated. Board VCC and GND have separate branches from 5V-J and GND-J.")
s.box(20, 30, 170, 190, "Becker J1", ["radio rear", "harness via DE-9"])
s.box(330, 30, 190, 190, "MAX490 board", ["A B Y Z | RXD TXD", "5 V, no termination"])
s.box(810, 30, 150, 190, "ESP32", ["UART2"])
s.line([(190, 55), (330, 55)], "data"); s.arrow(260, 55, "right", "data"); s.tag(260, 48, "2 TX2+ → A", "data", bold=False)
s.line([(190, 80), (330, 80)], "data"); s.arrow(260, 80, "right", "data"); s.tag(260, 73, "9 TX2− → B", "data", bold=False)
s.line([(330, 115), (190, 115)], "data"); s.arrow(260, 115, "left", "data"); s.tag(260, 108, "3 RX2+ ← Y", "data", bold=False)
s.line([(330, 140), (190, 140)], "data"); s.arrow(260, 140, "left", "data"); s.tag(260, 133, "10 RX2− ← Z", "data", bold=False)
s.line([(190, 175), (280, 175), (280, 235)], "sgnd"); s.tag(285, 232, "21 → GND-J", "sgnd", anchor="start", bold=False, size=11)
s.line([(190, 205), (290, 205)], "shield"); s.tag(198, 198, "11 shield, radio end only", "shield", bold=False, size=11, anchor="start")
# GPIO17 straight to DI, no resistor
s.line([(810, 85), (520, 85)], "data"); s.arrow(570, 85, "left", "data")
s.tag(729, 78, "TXD (DI) ← GPIO17 (J3-11)", "data")
# RO through R-B1, R-B2 to GND
s.line([(520, 135), (810, 135)], "data"); s.arrow(790, 135, "right", "data")
s.tag(527, 128, "RXD", "data", anchor="start")
s.resistor(600, 135, "data"); s.tag(600, 124, "R-B1 2.2 kΩ", "data", bold=False)
s.dot(720, 135, "data"); s.tag(765, 128, "GPIO16 (J3-12)", "data", size=11)
s.line([(720, 135), (720, 195)], "data"); s.resistor(720, 165, "data", vertical=True)
s.tag(730, 169, "R-B2 3.3 kΩ", "data", anchor="start", bold=False); s.dot(720, 195, "gnd")
s.line([(720, 195), (810, 195)], "gnd"); s.tag(765, 188, "local J3-7", "gnd", size=11)
s.line([(520, 195), (590, 195)], "gnd"); s.tag(555, 215, "to GND-J", "gnd", size=11)
s.legend(255, ["data", "sgnd", "gnd", "shield"], "Twisted pairs: TX2± and RX2±. VCC from 5V-J; GND from GND-J. No extra MAX490-to-ESP32 GND wire.")
s.save("wiring-becker.svg")
# ---------------------------------------- spare overview (no pins, not linked; kept as backup)
s = Svg(900, 345, "What the bridge does",
        "Glider computer Zander ZS1 sends flight data and collision warning Classic FLARM sends traffic to the Wi-Fi bridge. "
        "The radio Becker AR6201 exchanges standby frequency and status with the bridge in both directions. "
        "The bridge connects to the navigation app OpenSoar on Android over Wi-Fi.")
s.box(20, 20, 300, 74, ["Glider computer:", "Zander ZS1"], ["sends flight data"])
s.box(20, 113, 300, 74, ["Collision warning:", "Classic FLARM"], ["sends traffic"])
s.box(20, 206, 300, 74, ["Radio:", "Becker AR6201"], ["standby frequency in, status out"])
s.box(410, 80, 170, 140, ["Wi-Fi bridge:", "ESP32"])
s.box(680, 122, 200, 56, ["Navigation:", "OpenSoar (Android)"])
s.line([(320, 57), (365, 57), (365, 110), (410, 110)], "data"); s.arrow(390, 110, "right", "data")
s.line([(320, 150), (410, 150)], "data"); s.arrow(365, 150, "right", "data")
s.line([(320, 243), (365, 243), (365, 190), (410, 190)], "data"); s.arrow(390, 190, "right", "data"); s.arrow(340, 243, "left", "data")
s.line([(580, 150), (680, 150)], "wifi"); s.arrow(670, 150, "right", "wifi"); s.arrow(590, 150, "left", "wifi")
s.tag(630, 140, "Wi-Fi", "wifi")
s.legend(310, ["data", "wifi"], "Pins, levels and power: see the hardware pages.")
s.save("overview.svg")

# ----------------------------------------------------------------- ground loops
s = Svg(930, 530, "Bridge power and ground loops",
        "The battery feeds the bridge (UBEC, ESP32 with MAX490 and RS-232 modules) through fuse F1 in the harness, and each device with +12 V; arrows show the normal current direction. "
        "GND-J collects the module and instrument returns and connects through the UBEC output return; the bridge ground wire leaves from the UBEC input through the XT30. The battery minus, the bridge ground and each device's supply ground meet at the common ground point. Dashed signal grounds from the bridge to the "
        "Becker (J1-21), Zander (pin 5) and FLARM (pins 7/8) each close a ground loop: Loop B, Loop Z and Loop F.")
s.box(200, 126, 270, 244, "Bridge", group=True)
s.line([(95, 70), (95, 46)], "v12"); s.arrow(95, 58, "up", "v12"); s.tag(95, 40, "+12 V", "v12")
s.line([(335, 24), (335, 80)], "v12"); s.arrow(335, 56, "down", "v12"); s.tag(343, 34, "+12 V", "v12", anchor="start")
s.line([(335, 116), (335, 154)], "v12"); s.arrow(335, 135, "down", "v12")
s.line([(335, 190), (335, 220)], "v5"); s.arrow(335, 202, "down", "v5"); s.tag(343, 210, "5 V", "v5", anchor="start")
# Supply grounds back to the common ground point
s.line([(95, 126), (95, 440)], "gnd"); s.arrow(95, 300, "up", "gnd")
s.line([(270, 220), (270, 190)], "gnd"); s.arrow(270, 205, "up", "gnd"); s.tag(277, 209, "GND-J", "gnd", anchor="start", bold=False, size=11)
s.line([(245, 172), (215, 172), (215, 440)], "gnd"); s.arrow(215, 385, "down", "gnd")
s.line([(900, 112), (900, 440)], "gnd")
for y in (170, 285, 395):
    s.arrow(900, y, "down", "gnd")
s.line([(95, 440), (900, 440)], "gnd", width=4)
s.arrow(620, 440, "left", "gnd"); s.arrow(160, 440, "left", "gnd")
for x, y in ((95, 440), (215, 440), (900, 440)):
    s.dot(x, y, "gnd", r=5)
s.dot(900, 227, "gnd"); s.dot(900, 342, "gnd")
# Devices: +12 V stub, supply ground stub, signal ground (loop)
devices = ((70, "Becker AR6201", "P1-13/25", "Loop B", "signal GND J1-21", [(445, 240), (520, 240), (520, 98), (660, 98)]),
           (185, "Zander ZS1", "GND", "Loop Z", "signal GND pin 5", [(445, 275), (560, 275), (560, 213), (660, 213)]),
           (300, "FLARM", "GND", "Loop F", "signal GND pins 7/8", [(445, 310), (660, 310)]))
for y, name, gnd_label, loop, sig, path in devices:
    s.box(660, y, 150, 56, name)
    s.line([(810, y + 16), (835, y + 16)], "v12"); s.arrow(822, y + 16, "left", "v12"); s.tag(840, y + 20, "+12 V", "v12", anchor="start")
    s.line([(810, y + 42), (900, y + 42)], "gnd"); s.arrow(828, y + 42, "right", "gnd")
    s.tag(845, y + 36, gnd_label, "gnd", anchor="start", bold=False, size=11)
    s.line(path, "sgnd")
    label_y = path[-1][1]
    s.tag(652, label_y - 23, loop, "sgnd", anchor="end"); s.tag(652, label_y - 7, sig, "sgnd", anchor="end", bold=False)
s.box(30, 70, 130, 56, "Battery", ["12 V"])
s.box(245, 80, 180, 36, "F1 fuse (harness)")
s.box(245, 154, 180, 36, "UBEC 5 V")
s.box(225, 220, 220, 110, "GND-J / bridge loads", ["ESP32, MAX490", "RS-232 chain via ESP32"])
s.tag(103, 160, "Battery −", "gnd", anchor="start", bold=False, size=11)
s.tag(223, 405, "Bridge GND wire (XT30 −)", "gnd", anchor="start", bold=False, size=11)
s.text(497, 462, "Common ground point (battery −)", color="#424242", size=13, bold=True)
s.legend(492, ["v12", ("v5", "5 V in bridge"), ("gnd", "Supply ground"), ("sgnd", "Signal ground, closes a loop")],
         "Arrows show the normal current direction. Signal grounds carry small shares in either direction.")
s.save("ground-loops.svg")

print("ok")

# Rotate both component-side views clockwise, preserving the supplied bitmap.
photo_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "docs", "images", "rs422-to-ttl-supplied.png")
with open(photo_path, "rb") as photo_file:
    photo_data = base64.b64encode(photo_file.read()).decode("ascii")
photo_svg = ('<svg xmlns="http://www.w3.org/2000/svg" width="3245" height="1661" viewBox="0 0 3245 1661">'
             '<title>RS422 TO TTL · component side, header at right</title>'
             '<image width="1661" height="3245" transform="translate(3245 0) rotate(90)" '
             f'href="data:image/png;base64,{photo_data}"/></svg>\n')
with io.open(os.path.join(OUT, "rs422-to-ttl-board.svg"), "w", encoding="utf-8", newline="\n") as photo_file:
    photo_file.write(photo_svg)

s = Svg(560, 280, "RS422 TO TTL · component side",
        "Component-side view, rotated clockwise: screw terminals at left, header at right. "
        "Left top to bottom: Y output positive, Z output negative, B input negative, A input positive. "
        "Right top to bottom: TXD, RXD, GND, VCC. TXD is driver input DI; RXD is receiver output RO (S144).")
s.text(18, 29, "RS422 TO TTL · component side", size=21, bold=True, anchor="start")
s.text(18, 53, "Header at right · ESP32 signal names (S34, S144)", size=14, color=SUB, anchor="start")
s.text(100, 83, "RS-422 terminals", size=16, bold=True)
s.text(460, 83, "TTL header", size=16, bold=True)
s.groups.append('<rect x="180" y="97" width="200" height="166" rx="8" fill="#eaf1fb" stroke="#9cb1cc"/>')
s.text(280, 161, "MAX490", size=18, bold=True)
s.text(280, 187, "RO → divider → ESP32", size=12, color=SUB)
s.text(280, 207, "ESP32 → DI", size=12, color=SUB)
left = [("Y", "Output +"), ("Z", "Output −"), ("B", "Input −"), ("A", "Input +")]
right = [("TXD", "DI · from ESP32 TX", "data"), ("RXD", "RO · to RX divider", "data"),
         ("GND", "Ground", "gnd"), ("VCC", "5 V supply", "v5")]
for i, ((lname, ldetail), (rname, rdetail, kind)) in enumerate(zip(left, right)):
    y = 113 + i * 44
    for x, name, color in [(180, lname, STYLE["data"]["color"]), (380, rname, STYLE[kind]["color"])]:
        s.boxes.append(f'<rect x="{x-30}" y="{y-14}" width="60" height="28" rx="5" fill="{color}"/>')
        s.text(x, y+6, name, color="white", size=15, bold=True)
    s.text(138, y+5, ldetail, size=14, anchor="end")
    s.text(422, y+5, rdetail, size=14, anchor="start")
s.save("pinout-rs422-to-ttl.svg")

# ---------------------------------------------------------------- README teaser
# Product teaser at the top of the README: instruments -> bridge -> phone app,
# inside a glider, with the key features around it. The housing is cut out of
# the CAD preview (grid background removed by a flood fill from the edges).
from PIL import Image, ImageFilter  # noqa: E402  (only this section needs Pillow)

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")


def housing_cutout():
    """Return the closed-housing render with a transparent background, as WebP bytes."""
    im = Image.open(os.path.join(ROOT, "cad", "housing", "housing-body-v1", "preview-closed.png")).convert("RGB")
    im = im.crop((120, 70, 1200, 780)).resize((540, 355), Image.LANCZOS)
    w, h = im.size
    px = im.load()

    def background(p):  # blue-grey grid, its shadows and the red axis line
        r, g, b = p
        return (b >= g + 3 and b >= r + 6) or (r > g + 25 and abs(g - b) < 20)

    seen = bytearray(w * h)
    stack = [(x, y) for x in range(w) for y in (0, h - 1)] + [(x, y) for y in range(h) for x in (0, w - 1)]
    while stack:
        x, y = stack.pop()
        if seen[y * w + x] or not background(px[x, y]):
            continue
        seen[y * w + x] = 1
        stack += [(x2, y2) for x2, y2 in ((x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)) if 0 <= x2 < w and 0 <= y2 < h]
    mask = Image.frombytes("L", (w, h), bytes(0 if v else 255 for v in seen))
    im.putalpha(mask.filter(ImageFilter.MinFilter(3)).filter(ImageFilter.GaussianBlur(0.7)))
    im = im.crop(im.getbbox())
    buf = io.BytesIO()
    im.save(buf, "WEBP", quality=82, method=6)
    return im.size, buf.getvalue()


def esc(s):
    return escape(s)


def txt(x, y, s, size=13, color=INK, bold=False, anchor="start", extra=""):
    b = ' font-weight="700"' if bold else ""
    return f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" text-anchor="{anchor}"{b}{extra}>{esc(s)}</text>'


def icon(kind, cx, cy, color):
    """Small white line icon on a coloured disc, about 24 px wide."""
    st = 'fill="none" stroke="white" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"'
    o = [f'<circle cx="{cx}" cy="{cy}" r="21" fill="{color}"/>']
    if kind == "radar":
        o += [f'<circle cx="{cx}" cy="{cy}" r="11" {st}/>', f'<circle cx="{cx}" cy="{cy}" r="5" {st}/>',
              f'<path d="M{cx} {cy} L{cx + 8} {cy - 8}" {st}/>',
              f'<path d="M{cx - 9} {cy + 3} l3 -6 l3 6 z" fill="white"/>']
    elif kind == "gauge":
        o += [f'<path d="M{cx - 11} {cy + 5} A11 11 0 1 1 {cx + 11} {cy + 5}" {st}/>',
              f'<path d="M{cx} {cy + 1} L{cx + 7} {cy - 7}" {st}/>', f'<circle cx="{cx}" cy="{cy + 1}" r="2.2" fill="white"/>',
              f'<path d="M{cx - 7} {cy + 9} H{cx + 7}" {st}/>']
    elif kind == "radio":
        o += [f'<rect x="{cx - 12}" y="{cy - 8}" width="24" height="16" rx="2.5" {st}/>',
              f'<rect x="{cx - 8}" y="{cy - 4}" width="11" height="6" rx="1" fill="white"/>',
              f'<circle cx="{cx + 7}" cy="{cy + 1}" r="2.6" {st}/>', f'<path d="M{cx - 6} {cy - 8} L{cx + 2} {cy - 14}" {st}/>']
    elif kind == "wifi":
        o += [f'<path d="M{cx - 12} {cy - 3} A17 17 0 0 1 {cx + 12} {cy - 3}" {st}/>',
              f'<path d="M{cx - 7} {cy + 2} A10 10 0 0 1 {cx + 7} {cy + 2}" {st}/>',
              f'<circle cx="{cx}" cy="{cy + 7}" r="2.4" fill="white"/>']
    elif kind == "browser":
        o += [f'<rect x="{cx - 12}" y="{cy - 9}" width="24" height="18" rx="2.5" {st}/>', f'<path d="M{cx - 12} {cy - 4} H{cx + 12}" {st}/>',
              f'<path d="M{cx - 7} {cy + 1} H{cx + 2} M{cx - 7} {cy + 5} H{cx + 6}" {st}/>']
    elif kind == "monitor":
        o += [f'<path d="M{cx - 11} {cy - 7} H{cx + 9} M{cx - 11} {cy - 2} H{cx + 3} M{cx - 11} {cy + 3} H{cx + 11} M{cx - 11} {cy + 8} H{cx + 1}" {st}/>']
    elif kind == "update":
        o += [f'<path d="M{cx + 10} {cy - 1} A10 10 0 1 1 {cx + 5} {cy - 8.5}" {st}/>',
              f'<path d="M{cx + 6} {cy - 14} L{cx + 6} {cy - 8} L{cx} {cy - 8}" {st}/>']
    return o


W, H = 1060, 560
SKY_TOP, SKY_BOTTOM, CARD_LINE = "#cfe6fa", "#f5faff", "#b8d2ec"
WIFI = STYLE["wifi"]["color"]
o = []
o.append('<defs>'
         f'<linearGradient id="sky" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="{SKY_TOP}"/><stop offset="1" stop-color="{SKY_BOTTOM}"/></linearGradient>'
         '<filter id="shadow" x="-10%" y="-10%" width="120%" height="140%"><feDropShadow dx="0" dy="2" stdDeviation="3" flood-color="#1e3a5f" flood-opacity="0.18"/></filter>'
         '<clipPath id="screen"><rect x="686" y="164" width="112" height="222" rx="10"/></clipPath>'
         '</defs>')
o.append(f'<rect x="1" y="1" width="{W - 2}" height="{H - 2}" rx="22" fill="url(#sky)" stroke="{CARD_LINE}" stroke-width="2"/>')

# Glider in plan view, nose up, as a faint watermark: the bridge sits in its fuselage.
gx = 460
wing = [(gx - 10, 130), (gx - 300, 141), (gx - 452, 148), (gx - 455, 156), (gx - 300, 160), (gx - 10, 168)]
wing_r = [(2 * gx - x, y) for x, y in reversed(wing)]
stab = [(gx - 4, 468), (gx - 78, 474), (gx - 80, 482), (gx - 4, 484)]
stab_r = [(2 * gx - x, y) for x, y in reversed(stab)]
pts = lambda p: " ".join(f"{x},{y}" for x, y in p)
o.append('<g fill="white" opacity="0.6">'
         f'<polygon points="{pts(wing + wing_r)}"/><polygon points="{pts(stab + stab_r)}"/>'
         f'<path d="M{gx} 100 C{gx + 16} 104 {gx + 20} 130 {gx + 19} 175 C{gx + 16} 300 {gx + 7} 420 {gx + 4} 490 '
         f'L{gx - 4} 490 C{gx - 7} 420 {gx - 16} 300 {gx - 19} 175 C{gx - 20} 130 {gx - 16} 104 {gx} 100 Z"/></g>')
o.append(f'<ellipse cx="{gx}" cy="125" rx="9" ry="17" fill="#9cc3e6" opacity="0.7"/>')

# Header
o.append(txt(40, 60, "Wi-Fi Bridge for glider avionics", size=30, bold=True))
o.append(txt(40, 88, "Cockpit instruments to your phone or tablet, without a cable", size=16, color=SUB))
o.append(f'<rect x="838" y="36" width="196" height="30" rx="15" fill="#fff3e0" stroke="#ef6c00" stroke-width="1.5"/>')
o.append(txt(936, 56, "PROTOTYPE · IN DEVELOPMENT", size=12, color="#c25400", bold=True, anchor="middle"))

# Instruments (left)
instruments = [("radar", "#c62828", "Classic FLARM", "Traffic and collision", "warnings · NMEA", 192),
               ("gauge", "#1565c0", "Zander ZS1", "Flight and navigation", "data · NMEA", 282),
               ("radio", "#6a1b9a", "Becker AR6201 radio", "Airfield frequency from", "the app into standby", 372)]
for kind, color, name, l1, l2, cy in instruments:
    o.append(f'<rect x="28" y="{cy - 37}" width="250" height="74" rx="14" fill="white" filter="url(#shadow)"/>')
    o += icon(kind, 66, cy, color)
    o.append(txt(98, cy - 12, name, size=15, bold=True))
    o.append(txt(98, cy + 7, l1, color=SUB))
    o.append(txt(98, cy + 24, l2, color=SUB))
    # cable into the bridge
    end_y = 270 + (cy - 282) * 0.35
    o.append(f'<path d="M278 {cy} C318 {cy} 318 {end_y} 352 {end_y}" fill="none" stroke="{color}" stroke-width="2.5"/>')
    o.append(f'<polygon points="{352 - 9},{end_y - 5} {352 - 9},{end_y + 5} {352},{end_y}" fill="{color}"/>')
    if kind == "radio":
        o.append(f'<polygon points="{278 + 9},{cy - 5} {278 + 9},{cy + 5} {278},{cy}" fill="{color}"/>')

# The bridge (housing render)
(iw, ih), webp = housing_cutout()
hw = 255
hh = round(hw * ih / iw)
hx, hy = 330, 285 - hh // 2
o.append(f'<ellipse cx="{hx + hw / 2 + 5}" cy="{hy + hh - 12}" rx="{hw * 0.46}" ry="14" fill="#1e3a5f" opacity="0.12"/>')
o.append(f'<image x="{hx}" y="{hy}" width="{hw}" height="{hh}" href="data:image/webp;base64,{base64.b64encode(webp).decode("ascii")}"/>')
o.append(txt(hx + hw / 2, hy + hh + 26, "The bridge: ESP32 in a 3D-printed box", size=14, bold=True, anchor="middle"))

# Wi-Fi link
o.append(f'<path d="M594 285 H692" stroke="{WIFI}" stroke-width="2.5" stroke-dasharray="7 5" fill="none"/>')
o.append(f'<polygon points="683,280 683,290 692,285" fill="{WIFI}"/><polygon points="603,280 603,290 594,285" fill="{WIFI}"/>')
for r in (9, 17, 25):
    o.append(f'<path d="M{643 - r * 0.8} {268 - r * 0.6 + 6} A{r} {r} 0 0 1 {643 + r * 0.8} {268 - r * 0.6 + 6}" fill="none" stroke="{WIFI}" stroke-width="3" stroke-linecap="round"/>')
o.append(f'<circle cx="643" cy="272" r="3.5" fill="{WIFI}"/>')
o.append(txt(643, 312, "Wi-Fi", size=13, color=WIFI, bold=True, anchor="middle"))
o.append(txt(643, 328, "NMEA over TCP", size=11, color=WIFI, anchor="middle"))

# Phone with a moving map (drawn at x 680, shifted right)
o.append('<g transform="translate(16 0)">')
o.append(f'<rect x="680" y="152" width="124" height="246" rx="20" fill="#1f2937" filter="url(#shadow)"/>')
o.append('<g clip-path="url(#screen)">')
o.append('<rect x="686" y="164" width="112" height="222" fill="#eef3e2"/>')
o.append('<path d="M686 250 C710 230 730 262 760 240 C780 226 790 236 798 230 V300 C780 310 760 292 735 306 C712 318 700 300 686 312 Z" fill="#dbe7c4"/>')
o.append('<path d="M700 330 C712 318 732 322 736 334 C740 348 718 356 706 350 C698 346 696 338 700 330 Z" fill="#b5d5f1"/>')
o.append('<circle cx="742" cy="232" r="34" fill="none" stroke="#e53935" stroke-width="1.5" stroke-dasharray="4 3"/>')
o.append('<path d="M712 370 L742 282 L776 214" fill="none" stroke="#d81b60" stroke-width="2" stroke-dasharray="5 3"/>')
o.append('<path d="M742 274 l0 18 M733 280 h18 M738 290 h8" stroke="#111" stroke-width="2.4" stroke-linecap="round"/>')
o.append('<polygon points="768,252 774,264 762,264" fill="#e53935"/><polygon points="714,246 720,258 708,258" fill="#2e7d32"/>')
o.append('<rect x="686" y="164" width="112" height="22" fill="#263238"/>')
o.append(txt(692, 179, "STBY", size=9, color="#9ccc65", bold=True))
o.append(txt(792, 179, "123.505", size=11, color="white", bold=True, anchor="end"))
o.append('<rect x="686" y="364" width="112" height="22" fill="#263238"/>')
o.append(txt(692, 379, "Vario", size=9, color="#b0bec5"))
o.append(txt(792, 379, "+1.8 m/s", size=11, color="white", bold=True, anchor="end"))
o.append('</g>')
o.append('<rect x="726" y="156" width="32" height="4" rx="2" fill="#4b5563"/>')
o.append(txt(742, 424, "e.g. OpenSoar", size=15, bold=True, anchor="middle"))
o.append(txt(742, 442, "map, traffic, radio", size=12, color=SUB, anchor="middle"))
o.append('</g>')

# Features (right)
features = [("wifi", WIFI, "Own Wi-Fi hotspot", "no cable to the phone"),
            ("browser", "#1565c0", "Status and settings", "in the phone's browser"),
            ("monitor", "#455a64", "Data monitor", "see each line live"),
            ("update", "#2e7d32", "Updates over Wi-Fi", "no USB cable needed")]
for i, (kind, color, l1, l2) in enumerate(features):
    cy = 180 + i * 70
    o.append(f'<rect x="846" y="{cy - 29}" width="192" height="58" rx="14" fill="white" filter="url(#shadow)"/>')
    o += icon(kind, 876, cy, color)
    o.append(txt(905, cy - 3, l1, size=14, bold=True))
    o.append(txt(905, cy + 15, l2, size=12, color=SUB))

# Extras (bottom row)
extras = ["7–20 V supply", "Several apps at once", "Instrument presets", "Supply voltage monitor", "Original controls keep working"]
widths = [len(e) * 7.2 + 28 for e in extras]
x = (W - sum(widths) - 12 * (len(extras) - 1)) / 2
for e, w in zip(extras, widths):
    o.append(f'<rect x="{x:.0f}" y="490" width="{w:.0f}" height="30" rx="15" fill="white" stroke="{CARD_LINE}" stroke-width="1.5"/>')
    o.append(txt(x + w / 2, 510, e, size=13, color=INK, anchor="middle"))
    x += w + 12

teaser = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}" viewBox="0 0 {W} {H}" role="img" aria-labelledby="title desc">\n'
          '<title id="title">Wi-Fi Bridge for glider avionics</title>\n'
          '<desc id="desc">A small 3D-printed box in the glider collects data from Classic FLARM (traffic), the Zander ZS1 glider computer '
          '(flight data) and the Becker AR6201 radio, and sends it over Wi-Fi as NMEA over TCP to an app such as OpenSoar on a phone. '
          'The app can put an airfield frequency into the radio standby slot. Features: own Wi-Fi hotspot, status and settings page, '
          'data monitor, updates over Wi-Fi, 7 to 20 V supply, several apps at once, instrument presets, supply voltage monitor. '
          'Prototype in development.</desc>\n'
          '<g font-family="Segoe UI,Helvetica,Arial,sans-serif">\n' + "\n".join(o) + '\n</g>\n</svg>\n')
with io.open(os.path.join(OUT, "teaser.svg"), "w", encoding="utf-8", newline="\n") as f:
    f.write(teaser)
