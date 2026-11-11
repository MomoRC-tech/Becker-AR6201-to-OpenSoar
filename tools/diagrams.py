"""Generate the bridge's hardware diagrams in one consistent style.

Usage (from the repository root):  python tools/diagrams.py
Writes the SVGs into docs/images. Edit the diagram sections below, run the
script, and check the result in a browser before committing.

Output: overview.svg (README), power-supply.svg, ground-loops.svg, wiring-overview.svg,
wiring-zander.svg, wiring-flarm.svg, wiring-becker.svg.
"""
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

    def dot(self, x, y, kind, r=4):
        self.marks.append(f'<circle cx="{x}" cy="{y}" r="{r}" fill="{STYLE[kind]["color"]}"/>')

    def box(self, x, y, w, h, title, subs=(), group=False):
        if group:
            self.groups.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="8" fill="#f7f9fc" stroke="#9cb1cc" stroke-width="1.5" stroke-dasharray="6 4"/>')
            self.text(x + 12, y + 20, title, bold=True, anchor="start", size=13)
            return
        self.boxes.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="5" fill="#eaf1fb" stroke="#9cb1cc" stroke-width="1.5"/>')
        n = 1 + len(subs)
        top = y + h / 2 - (n - 1) * 9 + 5
        self.text(x + w / 2, top, title, bold=True, size=14)
        for i, s in enumerate(subs):
            self.text(x + w / 2, top + 18 * (i + 1), s, color=SUB, size=12)

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
s = Svg(1000, 320, "Bridge supply path",
        "12 V from the harness through the XT30 connector and fuse F1 to the UBEC. The UBEC's 5 V feeds the ESP32 board's 5V pin and the MAX490 board. "
        "The ESP32 board's 3.3 V regulator feeds both RS-232 modules and the pull-ups R-Z, R-F and R-D. Arrows show current direction.")
s.box(20, 45, 120, 56, "12 V harness", ["about 22 AWG"])
s.box(180, 45, 90, 56, "XT30")
s.box(310, 45, 100, 56, "F1 fuse", ["in housing"])
s.box(450, 45, 110, 56, "UBEC", ["5 V, 3 A"])
for x1, x2 in ((140, 180), (270, 310), (410, 450)):
    s.line([(x1, 73), (x2, 73)], "v12"); s.arrow((x1 + x2) / 2, 73, "right", "v12")
s.tag(160, 64, "+12 V", "v12")
s.box(640, 35, 170, 90, "ESP32 board", ["5V pin (J2-19)", "3.3 V regulator → 3V3"])
s.box(640, 185, 170, 56, "MAX490 board", ["VCC"])
s.line([(560, 73), (640, 73)], "v5"); s.arrow(620, 73, "right", "v5")
s.line([(600, 73), (600, 213), (640, 213)], "v5"); s.arrow(600, 150, "down", "v5"); s.arrow(625, 213, "right", "v5")
s.dot(600, 73, "v5"); s.tag(578, 64, "5 V", "v5")
s.box(870, 25, 120, 44, "RS-232-mod1")
s.box(870, 85, 120, 44, "RS-232-mod2")
s.box(870, 145, 120, 56, "Pull-ups", ["R-Z, R-F, R-D"])
s.line([(810, 80), (845, 80)], "v33")
s.line([(845, 47), (845, 173)], "v33")
for y in (47, 107, 173):
    s.line([(845, y), (870, y)], "v33"); s.arrow(860, y, "right", "v33")
s.dot(845, 80, "v33"); s.tag(828, 72, "3.3 V", "v33")
s.legend(285, ["v12", "v5", "v33"], "Arrows show current direction. Grounds and loops: see the ground diagram.")
s.save("power-supply.svg")

# ------------------------------------------------------------- signal overview
s = Svg(1020, 380, "Bridge signal overview",
        "Zander ZS1 to RS-232-mod1 to ESP32 UART0 (receive only). Classic FLARM to RS-232-mod2 to ESP32 UART1 (receive only). "
        "Becker AR6201 to and from the MAX490 RS-422 board and ESP32 UART2 in both directions. The ESP32 talks to OpenSoar over Wi-Fi.")
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
s.line([(170, 262), (300, 262)], "data"); s.arrow(235, 262, "right", "data")
s.line([(300, 286), (170, 286)], "data"); s.arrow(235, 286, "left", "data")
s.line([(470, 262), (660, 262)], "data"); s.arrow(565, 262, "right", "data")
s.line([(660, 286), (470, 286)], "data"); s.arrow(565, 286, "left", "data")
s.tag(235, 254, "TX2± → A/B", "data", bold=False); s.tag(235, 302, "RX2± ← Y/Z", "data", bold=False)
s.tag(565, 254, "RO → divider → GPIO16", "data", bold=False); s.tag(565, 302, "DI ← GPIO17 · UART2", "data", bold=False)
s.line([(790, 176), (870, 176)], "wifi"); s.arrow(860, 176, "right", "wifi"); s.arrow(800, 176, "left", "wifi")
s.tag(830, 166, "one TCP path", "wifi", bold=False); s.tag(830, 196, "per device", "wifi", bold=False)
s.legend(345, ["data", "wifi"], "Zander and FLARM: receive only. Becker: both directions. Pin details: see each device section.")
s.save("wiring-overview.svg")

# --------------------------------------------------------------------- Zander
def rs232_device(name, dev, tx_pin, gnd_pin, gpio, uart, pull, note, fname, desc):
    s = Svg(930, 250, f"{dev} to RS-232 module to ESP32", desc)
    s.box(20, 40, 170, 130, dev, ["existing harness, via DE-9"])
    s.box(340, 40, 220, 130, name, ["RS-232 ↔ 3.3 V TTL", f"R1: {pull} pull-up to 3.3 V"])
    s.box(710, 40, 200, 130, "ESP32", [f"{gpio} · {uart} RX", "GND"])
    s.line([(190, 70), (340, 70)], "data"); s.arrow(265, 70, "right", "data")
    s.tag(265, 62, f"{tx_pin} → L3", "data")
    s.line([(560, 70), (710, 70)], "data"); s.arrow(635, 70, "right", "data")
    s.tag(635, 62, f"R2 → {gpio}", "data")
    s.line([(190, 145), (340, 145)], "sgnd"); s.tag(265, 137, f"{gnd_pin} → L1", "sgnd")
    s.line([(560, 145), (710, 145)], "gnd"); s.tag(635, 137, "R4 → GND", "gnd")
    s.legend(215, ["data", "sgnd", "gnd"], note)
    s.save(fname)

rs232_device("RS-232-mod1", "Zander ZS1", "TX", "Signal GND", "GPIO32", "UART0", "10 kΩ (R-Z)",
             "Receive only: L4 and GPIO33 stay unconnected. Supply via the 3.3 V rail (see power).", "wiring-zander.svg",
             "Zander TX to pad L3, TTL output R2 to GPIO32 (UART0 receive). Signal ground to pads L1 and R4 and ESP32 GND. R1 has a 10 kΩ pull-up to 3.3 V. Receive only.")
rs232_device("RS-232-mod2", "Classic FLARM", "pin 5 TX", "pins 7/8 GND", "GPIO26", "UART1", "10 kΩ (R-F)",
             "Receive only: L4, GPIO27 and FLARM RX stay unconnected. Never connect FLARM pins 1–3 (supply).", "wiring-flarm.svg",
             "FLARM pin 5 TX to pad L3, TTL output R2 to GPIO26 (UART1 receive). FLARM pins 7/8 ground to pads L1 and R4 and ESP32 GND. R1 has a 10 kΩ pull-up to 3.3 V. Receive only.")

# --------------------------------------------------------------------- Becker
s = Svg(980, 300, "Becker to RS-422 board to ESP32",
        "Becker J1 TX2+ and TX2- (pins 2, 9) to MAX490 receiver inputs A and B; MAX490 driver outputs Y and Z to J1 RX2+ and RX2- (pins 3, 10). "
        "J1-21 D_GND to the board GND. Cable shield to J1-11 at the radio end only. Receiver output RO through a 2.2 kΩ / 3.3 kΩ divider to GPIO16; "
        "GPIO17 to driver input DI with a 10 kΩ pull-up to 3.3 V.")
s.box(20, 30, 170, 190, "Becker J1", ["radio rear", "harness via DE-9"])
s.box(330, 30, 190, 190, "MAX490 board", ["A B Y Z | RO DI", "5 V, 120 Ω term."])
s.box(610, 50, 120, 56, "Divider", ["2.2 kΩ / 3.3 kΩ"])
s.box(810, 30, 150, 190, "ESP32", ["UART2"])
s.line([(190, 55), (330, 55)], "data"); s.arrow(260, 55, "right", "data"); s.tag(260, 48, "2 TX2+ → A", "data", bold=False)
s.line([(190, 80), (330, 80)], "data"); s.arrow(260, 80, "right", "data"); s.tag(260, 73, "9 TX2− → B", "data", bold=False)
s.line([(330, 115), (190, 115)], "data"); s.arrow(260, 115, "left", "data"); s.tag(260, 108, "3 RX2+ ← Y", "data", bold=False)
s.line([(330, 140), (190, 140)], "data"); s.arrow(260, 140, "left", "data"); s.tag(260, 133, "10 RX2− ← Z", "data", bold=False)
s.line([(190, 175), (330, 175)], "sgnd"); s.tag(260, 168, "21 D_GND → GND", "sgnd", bold=False)
s.line([(190, 205), (290, 205)], "shield"); s.tag(198, 198, "11 shield, radio end only", "shield", bold=False, size=11, anchor="start")
s.line([(520, 78), (610, 78)], "data"); s.arrow(565, 78, "right", "data"); s.tag(565, 70, "RO", "data")
s.line([(730, 78), (810, 78)], "data"); s.arrow(770, 78, "right", "data"); s.tag(770, 70, "GPIO16", "data")
s.line([(810, 140), (520, 140)], "data"); s.arrow(665, 140, "left", "data"); s.tag(665, 132, "DI ← GPIO17 (10 kΩ pull-up R-D)", "data")
s.line([(520, 175), (810, 175)], "gnd"); s.tag(665, 168, "GND", "gnd")
s.legend(255, ["data", "sgnd", "gnd", "shield"], "Twisted pairs: TX2± and RX2±. Board VCC 5 V from the UBEC (see power).")
s.save("wiring-becker.svg")
# ------------------------------------------------------- README overview (no pins)
s = Svg(900, 310, "What the bridge does",
        "Vario Zander ZS1 sends flight data and collision warning Classic FLARM sends traffic to the Wi-Fi bridge. "
        "The radio Becker AR6201 exchanges standby frequency and status with the bridge in both directions. "
        "The bridge connects to the navigation app OpenSoar on Android over Wi-Fi.")
s.box(20, 30, 300, 56, "Vario: Zander ZS1", ["sends flight data"])
s.box(20, 110, 300, 56, "Collision warning: Classic FLARM", ["sends traffic"])
s.box(20, 190, 300, 56, "Radio: Becker AR6201", ["standby frequency in, status out"])
s.box(410, 90, 170, 126, "Wi-Fi bridge", ["ESP32"])
s.box(680, 125, 200, 56, "Navigation: OpenSoar", ["Android"])
s.line([(320, 58), (365, 58), (365, 115), (410, 115)], "data"); s.arrow(390, 115, "right", "data")
s.line([(320, 138), (410, 138)], "data"); s.arrow(365, 138, "right", "data")
s.line([(320, 218), (365, 218), (365, 190), (410, 190)], "data"); s.arrow(390, 190, "right", "data"); s.arrow(340, 218, "left", "data")
s.line([(580, 153), (680, 153)], "wifi"); s.arrow(670, 153, "right", "wifi"); s.arrow(590, 153, "left", "wifi")
s.tag(630, 143, "Wi-Fi", "wifi")
s.legend(275, ["data", "wifi"], "Pins, levels and power: see the hardware pages.")
s.save("overview.svg")

# ----------------------------------------------------------------- ground loops
s = Svg(930, 530, "Bridge power and ground loops",
        "The battery feeds the bridge (F1, UBEC, ESP32 with MAX490 and RS-232 modules) and each device with +12 V; arrows show the normal current direction. "
        "The battery minus, the bridge ground and each device's supply ground meet at the common ground point. Dashed signal grounds from the bridge to the "
        "Becker (J1-21), Zander (pin 5) and FLARM (pins 7/8) each close a ground loop: Loop B, Loop Z and Loop F.")
s.box(200, 50, 270, 320, "Bridge", group=True)
s.line([(95, 70), (95, 46)], "v12"); s.arrow(95, 58, "up", "v12"); s.tag(95, 40, "+12 V", "v12")
s.line([(335, 24), (335, 80)], "v12"); s.arrow(335, 56, "down", "v12"); s.tag(343, 34, "+12 V", "v12", anchor="start")
s.line([(335, 116), (335, 140)], "v12"); s.arrow(335, 129, "down", "v12")
s.line([(335, 176), (335, 220)], "v5"); s.arrow(335, 192, "down", "v5"); s.tag(343, 203, "5 V", "v5", anchor="start")
# Supply grounds back to the common ground point
s.line([(95, 126), (95, 440)], "gnd"); s.arrow(95, 300, "up", "gnd")
s.line([(335, 330), (335, 440)], "gnd"); s.arrow(335, 385, "down", "gnd")
s.line([(900, 112), (900, 440)], "gnd")
for y in (170, 285, 395):
    s.arrow(900, y, "down", "gnd")
s.line([(95, 440), (900, 440)], "gnd", width=4)
s.arrow(620, 440, "left", "gnd"); s.arrow(215, 440, "left", "gnd")
for x, y in ((95, 440), (335, 440), (900, 440)):
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
s.tag(343, 405, "Bridge GND", "gnd", anchor="start", bold=False, size=11)
s.text(497, 462, "Common ground point (battery −)", color="#424242", size=13, bold=True)
s.legend(492, ["v12", ("v5", "5 V in bridge"), ("gnd", "Supply ground"), ("sgnd", "Signal ground, closes a loop")],
         "Arrows show the normal current direction. Signal grounds carry small shares in either direction.")
s.save("ground-loops.svg")

print("ok")
