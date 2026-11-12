"""Generate the bridge's hardware diagrams in one consistent style.

Usage (from the repository root):  python tools/diagrams.py
Writes the SVGs into docs/images. Edit the diagram sections below, run the
script, and check the result in a browser before committing.

Output: overview.svg (README), power-supply.svg, ground-loops.svg, wiring-overview.svg,
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
    "gnd":    dict(color="#616161", dash=None, width=2.5, label="Bridge ground"),
    "sgnd":   dict(color="#1e88e5", dash="7 5", width=2.5, label="Signal ground"),
    "shield": dict(color="#9e9e9e", dash="2 4", width=2.5, label="Cable shield"),
    "wifi":   dict(color="#00897b", dash="8 5", width=2.5, label="Wi-Fi"),
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
s = Svg(1120, 320, "Bridge supply path",
        "12 V from the harness through the XT30 connector and fuse F1 to the UBEC. The UBEC's 5 V feeds the ESP32 board's 5V pin; a second wire from that pin feeds the MAX490 board. "
        "The ESP32 board's 3.3 V regulator feeds pad R3 of both RS-232 modules; R-Z and R-F (10 kΩ, on the modules) run from R3 to pad R1, and R-D (10 kΩ) from 3V3 to the MAX490 driver input DI (GPIO17). Arrows show current direction.")
s.box(20, 45, 120, 56, "12 V harness", ["about 22 AWG"])
s.box(180, 45, 90, 56, "XT30")
s.box(310, 45, 100, 56, "F1 fuse", ["in housing"])
s.box(450, 45, 110, 56, "UBEC", ["5 V, 3 A"])
for x1, x2 in ((140, 180), (270, 310), (410, 450)):
    s.line([(x1, 73), (x2, 73)], "v12"); s.arrow((x1 + x2) / 2, 73, "right", "v12")
s.tag(160, 64, "+12 V", "v12")
s.box(640, 35, 170, 90, "ESP32 board", ["5V pin (J2-19)", "3.3 V regulator → 3V3"])
s.box(640, 185, 170, 56, "MAX490 board", ["VCC"])
s.line([(560, 73), (640, 73)], "v5"); s.arrow(600, 73, "right", "v5"); s.tag(600, 64, "5 V", "v5")
s.line([(700, 125), (700, 185)], "v5"); s.arrow(700, 155, "down", "v5"); s.tag(708, 159, "5 V from J2-19", "v5", anchor="start", bold=False, size=11)
s.line([(810, 80), (845, 80)], "v33")
s.line([(845, 35), (845, 190)], "v33")
s.dot(845, 80, "v33"); s.tag(828, 72, "3.3 V", "v33")
# RS-232 modules: 3V3 to pad R3, pull-up R-Z / R-F from R3 to pad R1 (soldered on the module)
for name, rname, top in (("RS-232-mod1", "R-Z", 20), ("RS-232-mod2", "R-F", 95)):
    r3, r1 = top + 15, top + 45
    s.box(960, top, 140, 60, name)
    s.text(966, r3 + 4, "R3", color=SUB, size=11, anchor="start"); s.text(966, r1 + 4, "R1", color=SUB, size=11, anchor="start")
    s.line([(845, r3), (960, r3)], "v33"); s.arrow(930, r3, "right", "v33")
    s.dot(885, r3, "v33")
    s.line([(885, r3), (885, r1), (960, r1)], "v33"); s.resistor(885, (r3 + r1) / 2, "v33", vertical=True)
    s.tag(893, (r3 + r1) / 2 + 4, f"{rname} 10 kΩ", "v33", anchor="start", bold=False, size=11)
# R-D: pull-up from 3V3 to the MAX490 driver input DI (GPIO17)
s.line([(845, 190), (960, 190)], "v33"); s.arrow(860, 190, "right", "v33")
s.resistor(905, 190, "v33"); s.tag(905, 179, "R-D 10 kΩ", "v33")
s.dot(960, 190, "v33"); s.tag(960, 209, "DI · GPIO17", "v33", bold=False, size=11)
s.legend(285, ["v12", "v5", "v33"], "Arrows show current direction. Grounds and loops: see the ground diagram.")
s.save("power-supply.svg")

# ------------------------------------------------------------- signal overview
s = Svg(1020, 380, "Bridge signal overview",
        "Zander ZS1 to RS-232-mod1 to ESP32 UART0 (receive only). Classic FLARM to RS-232-mod2 to ESP32 UART1 (receive only). "
        "Becker AR6201 to and from the MAX490 RS-422 board and ESP32 UART2 in both directions. Pull-ups R-Z and R-F from pad R1 to 3V3 on the RS-232 modules; RO through R-B1 to GPIO16 with R-B2 to ground; R-D from DI to 3V3. The ESP32 talks to OpenSoar over Wi-Fi.")
s.box(20, 50, 150, 56, "Zander ZS1", ["RS-232"])
s.box(20, 145, 150, 56, "Classic FLARM", ["RS-232"])
s.box(20, 240, 150, 66, "Becker AR6201", ["RS-422, J1"])
s.box(300, 50, 170, 56, "RS-232-mod1", ["MAX3232, 3.3 V"])
s.box(300, 145, 170, 56, "RS-232-mod2", ["MAX3232, 3.3 V"])
s.box(300, 240, 170, 66, "RS-422 board", ["MAX490, 5 V"])
s.box(660, 40, 130, 276, "ESP32", ["Wi-Fi bridge"])
s.box(870, 148, 130, 56, "OpenSoar", ["Android"])
for y, a, b in ((78, "TX → L3", "R2 → GPIO32 · UART0"), (173, "pin 5 TX → L3", "R2 → GPIO26 · UART1")):
    s.line([(170, y), (300, y)], "data"); s.arrow(235, y, "right", "data")
    s.line([(470, y), (660, y)], "data"); s.arrow(565, y, "right", "data")
    s.tag(235, y - 8, a, "data", bold=False); s.tag(565, y - 8, b, "data", bold=False)
for y, rname in ((78, "R-Z"), (173, "R-F")):
    # Pull-up from pad R1 to 3V3, soldered on the module
    s.line([(470, y + 17), (535, y + 17)], "v33"); s.resistor(503, y + 17, "v33")
    s.tag(474, y + 13, "R1", "v33", anchor="start", bold=False, size=10)
    s.tag(540, y + 21, "3V3", "v33", anchor="start", size=11)
    s.tag(503, y + 34, rname, "v33", bold=False, size=11)
s.line([(170, 262), (300, 262)], "data"); s.arrow(235, 262, "right", "data")
s.line([(300, 286), (170, 286)], "data"); s.arrow(235, 286, "left", "data")
# DI with pull-up R-D to 3V3
s.line([(660, 262), (470, 262)], "data"); s.arrow(495, 262, "left", "data"); s.dot(630, 262, "data")
s.line([(630, 262), (630, 222)], "v33"); s.resistor(630, 242, "v33", vertical=True)
s.tag(638, 246, "R-D", "v33", anchor="start", bold=False, size=11); s.tag(630, 216, "3V3", "v33", size=11)
# RO through R-B1 to GPIO16, R-B2 to ground
s.line([(470, 286), (660, 286)], "data"); s.arrow(648, 286, "right", "data"); s.resistor(530, 286, "data")
s.dot(600, 286, "data"); s.line([(600, 286), (600, 326)], "data"); s.resistor(600, 306, "data", vertical=True); s.ground(600, 326)
s.tag(530, 302, "R-B1", "data", bold=False, size=11); s.tag(608, 310, "R-B2", "data", anchor="start", bold=False, size=11)
s.tag(235, 254, "TX2± → A/B", "data", bold=False); s.tag(235, 302, "RX2± ← Y/Z", "data", bold=False)
s.tag(550, 254, "DI ← GPIO17 · UART2", "data", bold=False, size=11)
s.tag(486, 280, "RO", "data", anchor="start", bold=False, size=11); s.tag(632, 280, "GPIO16", "data", bold=False, size=11)
s.line([(790, 176), (870, 176)], "wifi"); s.arrow(860, 176, "right", "wifi"); s.arrow(800, 176, "left", "wifi")
s.tag(830, 166, "one TCP path", "wifi", bold=False); s.tag(830, 196, "per device", "wifi", bold=False)
s.legend(345, ["data", "wifi", ("v33", "3.3 V")], "Zander and FLARM: receive only. Becker: both directions. Pins and resistor values: see each device section.")
s.save("wiring-overview.svg")

# --------------------------------------------------------------------- Zander
def rs232_device(name, dev, tx_pin, gnd_pin, gpio, uart, pull, note, fname, desc):
    s = Svg(930, 250, f"{dev} to RS-232 module to ESP32", desc)
    s.box(20, 40, 170, 130, dev, ["existing harness, via DE-9"])
    s.box(340, 40, 220, 130, name, ["RS-232 ↔ 3.3 V TTL", "pads R1–R4 at right"])
    s.box(710, 40, 200, 130, "ESP32", [f"{gpio} · {uart} RX", "3V3 (J2-1)", "GND"])
    s.line([(190, 70), (340, 70)], "data"); s.arrow(265, 70, "right", "data")
    s.tag(265, 62, f"{tx_pin} → L3", "data")
    s.line([(560, 70), (710, 70)], "data"); s.arrow(635, 70, "right", "data")
    s.tag(635, 62, f"R2 → {gpio}", "data")
    s.line([(190, 145), (340, 145)], "sgnd"); s.tag(265, 137, f"{gnd_pin} → L1", "sgnd")
    # Pull-up from pad R1 to pad R3 (3.3 V supply of the module), soldered on the module
    s.line([(560, 95), (640, 95), (640, 130)], "v33"); s.tag(566, 90, "R1", "v33", anchor="start", size=11)
    s.resistor(640, 112, "v33", vertical=True); s.tag(630, 116, pull, "v33", anchor="end", bold=False, size=11)
    s.line([(710, 130), (560, 130)], "v33"); s.arrow(690, 130, "left", "v33"); s.dot(640, 130, "v33")
    s.tag(682, 123, "3V3 → R3", "v33", size=11)
    s.line([(560, 160), (710, 160)], "gnd"); s.tag(600, 155, "R4 → GND", "gnd", size=11)
    s.legend(215, ["data", "sgnd", "gnd", ("v33", "3.3 V")], note)
    s.save(fname)

rs232_device("RS-232-mod1", "Zander ZS1", "TX", "Signal GND", "GPIO32", "UART0", "R-Z 10 kΩ",
             "Receive only: L4 and GPIO33 stay unconnected. Supply via the 3.3 V rail (see power).", "wiring-zander.svg",
             "Zander TX to pad L3, TTL output R2 to GPIO32 (UART0 receive). Signal ground to pads L1 and R4 and ESP32 GND. ESP32 3V3 to pad R3; R-Z (10 kΩ) on the module from pad R1 to pad R3. Receive only.")
rs232_device("RS-232-mod2", "Classic FLARM", "pin 5 TX", "pins 7/8 GND", "GPIO26", "UART1", "R-F 10 kΩ",
             "Receive only: L4, GPIO27 and FLARM RX stay unconnected. Never connect FLARM pins 1–3 (supply).", "wiring-flarm.svg",
             "FLARM pin 5 TX to pad L3, TTL output R2 to GPIO26 (UART1 receive). FLARM pins 7/8 ground to pads L1 and R4 and ESP32 GND. ESP32 3V3 to pad R3; R-F (10 kΩ) on the module from pad R1 to pad R3. Receive only.")

# --------------------------------------------------------------------- Becker
s = Svg(980, 300, "Becker to RS-422 board to ESP32",
        "Becker J1 TX2+ and TX2- (pins 2, 9) to MAX490 receiver inputs A and B; MAX490 driver outputs Y and Z to J1 RX2+ and RX2- (pins 3, 10). "
        "J1-21 D_GND to the board GND. Cable shield to J1-11 at the radio end only. Receiver output RO through R-B1 (2.2 kΩ) to GPIO16, "
        "with R-B2 (3.3 kΩ) from GPIO16 to GND. GPIO17 to driver input DI, with R-D (10 kΩ) from DI to 3V3. Board VCC 5 V from the ESP32 5V pin.")
s.box(20, 30, 170, 190, "Becker J1", ["radio rear", "harness via DE-9"])
s.box(330, 30, 190, 190, "MAX490 board", ["A B Y Z | RO DI", "5 V, 120 Ω term."])
s.box(810, 30, 150, 190, "ESP32", ["UART2"])
s.line([(190, 55), (330, 55)], "data"); s.arrow(260, 55, "right", "data"); s.tag(260, 48, "2 TX2+ → A", "data", bold=False)
s.line([(190, 80), (330, 80)], "data"); s.arrow(260, 80, "right", "data"); s.tag(260, 73, "9 TX2− → B", "data", bold=False)
s.line([(330, 115), (190, 115)], "data"); s.arrow(260, 115, "left", "data"); s.tag(260, 108, "3 RX2+ ← Y", "data", bold=False)
s.line([(330, 140), (190, 140)], "data"); s.arrow(260, 140, "left", "data"); s.tag(260, 133, "10 RX2− ← Z", "data", bold=False)
s.line([(190, 175), (330, 175)], "sgnd"); s.tag(260, 168, "21 D_GND → GND", "sgnd", bold=False)
s.line([(190, 205), (290, 205)], "shield"); s.tag(198, 198, "11 shield, radio end only", "shield", bold=False, size=11, anchor="start")
# 3V3 rail and R-D (pull-up on DI)
s.line([(810, 45), (640, 45), (640, 85)], "v33"); s.arrow(770, 45, "left", "v33"); s.tag(740, 38, "3V3 (J2-1)", "v33")
s.resistor(640, 65, "v33", vertical=True); s.tag(630, 69, "R-D 10 kΩ", "v33", anchor="end")
s.line([(810, 85), (520, 85)], "data"); s.arrow(570, 85, "left", "data"); s.dot(640, 85, "data")
s.tag(745, 78, "DI ← GPIO17 (J3-11)", "data")
# RO through R-B1, R-B2 to GND
s.line([(520, 135), (810, 135)], "data"); s.arrow(790, 135, "right", "data")
s.tag(527, 128, "RO", "data", anchor="start")
s.resistor(600, 135, "data"); s.tag(600, 124, "R-B1 2.2 kΩ", "data", bold=False)
s.dot(720, 135, "data"); s.tag(765, 128, "GPIO16 (J3-12)", "data", size=11)
s.line([(720, 135), (720, 195)], "data"); s.resistor(720, 165, "data", vertical=True)
s.tag(730, 169, "R-B2 3.3 kΩ", "data", anchor="start", bold=False); s.dot(720, 195, "gnd")
s.line([(520, 195), (810, 195)], "gnd"); s.tag(600, 188, "GND", "gnd")
s.legend(255, ["data", "sgnd", "gnd", "shield", ("v33", "3.3 V")], "Twisted pairs: TX2± and RX2±. Board VCC 5 V from the ESP32 5V pin (see power).")
s.save("wiring-becker.svg")
# ------------------------------------------------------- README overview (no pins)
s = Svg(900, 345, "What the bridge does",
        "Vario Zander ZS1 sends flight data and collision warning Classic FLARM sends traffic to the Wi-Fi bridge. "
        "The radio Becker AR6201 exchanges standby frequency and status with the bridge in both directions. "
        "The bridge connects to the navigation app OpenSoar on Android over Wi-Fi.")
s.box(20, 20, 300, 74, ["Vario:", "Zander ZS1"], ["sends flight data"])
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
        "The battery feeds the bridge (F1, UBEC, ESP32 with MAX490 and RS-232 modules) and each device with +12 V; arrows show the normal current direction. "
        "The ESP32 ground (J2-14) returns through the UBEC; the bridge ground wire leaves from the UBEC input through the XT30. The battery minus, the bridge ground and each device's supply ground meet at the common ground point. Dashed signal grounds from the bridge to the "
        "Becker (J1-21), Zander (pin 5) and FLARM (pins 7/8) each close a ground loop: Loop B, Loop Z and Loop F.")
s.box(200, 50, 270, 320, "Bridge", group=True)
s.line([(95, 70), (95, 46)], "v12"); s.arrow(95, 58, "up", "v12"); s.tag(95, 40, "+12 V", "v12")
s.line([(335, 24), (335, 80)], "v12"); s.arrow(335, 56, "down", "v12"); s.tag(343, 34, "+12 V", "v12", anchor="start")
s.line([(335, 116), (335, 140)], "v12"); s.arrow(335, 129, "down", "v12")
s.line([(335, 176), (335, 220)], "v5"); s.arrow(335, 192, "down", "v5"); s.tag(343, 203, "5 V", "v5", anchor="start")
# Supply grounds back to the common ground point
s.line([(95, 126), (95, 440)], "gnd"); s.arrow(95, 300, "up", "gnd")
s.line([(270, 220), (270, 176)], "gnd"); s.arrow(270, 198, "up", "gnd"); s.tag(277, 202, "J2-14", "gnd", anchor="start", bold=False, size=11)
s.line([(245, 158), (215, 158), (215, 440)], "gnd"); s.arrow(215, 385, "down", "gnd")
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
s.box(245, 80, 180, 36, "F1 fuse")
s.box(245, 140, 180, 36, "UBEC 5 V")
s.box(225, 220, 220, 110, "ESP32", ["MAX490 board", "RS-232 modules (3.3 V from ESP32)"])
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
        "Right top to bottom: TXD, RXD, GND, VCC. TXD/RXD are printed labels, not a verified RO/DI assignment.")
s.text(18, 29, "RS422 TO TTL · component side", size=21, bold=True, anchor="start")
s.text(18, 53, "Header at right · labels as printed (S34)", size=14, color=SUB, anchor="start")
s.text(100, 83, "RS-422 terminals", size=16, bold=True)
s.text(460, 83, "TTL header", size=16, bold=True)
s.groups.append('<rect x="180" y="97" width="200" height="166" rx="8" fill="#eaf1fb" stroke="#9cb1cc"/>')
s.text(280, 161, "MAX490", size=18, bold=True)
s.text(280, 187, "RO → divider → ESP32", size=12, color=SUB)
s.text(280, 207, "ESP32 → DI", size=12, color=SUB)
left = [("Y", "Output +"), ("Z", "Output −"), ("B", "Input −"), ("A", "Input +")]
right = [("TXD", "TTL signal", "data"), ("RXD", "TTL signal", "data"),
         ("GND", "Ground", "gnd"), ("VCC", "5 V supply", "v5")]
for i, ((lname, ldetail), (rname, rdetail, kind)) in enumerate(zip(left, right)):
    y = 113 + i * 44
    for x, name, color in [(180, lname, STYLE["data"]["color"]), (380, rname, STYLE[kind]["color"])]:
        s.boxes.append(f'<rect x="{x-30}" y="{y-14}" width="60" height="28" rx="5" fill="{color}"/>')
        s.text(x, y+6, name, color="white", size=15, bold=True)
    s.text(138, y+5, ldetail, size=14, anchor="end")
    s.text(422, y+5, rdetail, size=14, anchor="start")
s.save("pinout-rs422-to-ttl.svg")
