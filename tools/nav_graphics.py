"""Generate the documentation's navigation graphics: menu cards, pills and state strips.

Usage (from the repository root):  python tools/nav_graphics.py [--preview out.html]
Writes light and dark SVGs into docs/images/nav. Pages show them with <picture>, so
GitHub picks the variant that matches its theme. Edit the lists at the bottom, run
the script, check the preview in a browser, commit script and SVGs together.
"""
import os
import sys
from xml.sax.saxutils import escape

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")
OUT = os.path.join(ROOT, "docs", "images", "nav")
FONT = "-apple-system,BlinkMacSystemFont,'Segoe UI','Noto Sans',Helvetica,Arial,sans-serif"

# GitHub's own surface colours, so the graphics sit flush on the page in both themes.
THEMES = {
    "light": dict(bg="#ffffff", card="#f6f8fa", line="#d1d9e0", ink="#1f2328", sub="#59636e", arrow="#818b98"),
    "dark": dict(bg="#0d1117", card="#151b23", line="#3d444d", ink="#f0f6fc", sub="#9198a1", arrow="#656c76"),
}
# Accent per meaning: (disc / stroke, light tint, dark tint)
TONES = {
    "use": ("#00897b", "#e0f2f1", "#0f2e2b"),
    "build": ("#e8710a", "#fdeedd", "#33230f"),
    "ref": ("#5b6b7a", "#eceff1", "#1e252c"),
    "neutral": ("#6e7781", "#f6f8fa", "#151b23"),
    "busy": ("#1f6feb", "#ddf4ff", "#0c2d6b"),
    "good": ("#1a7f37", "#dafbe1", "#0f2d19"),
    "fallback": ("#bc4c00", "#fff1e5", "#3d1f00"),
}


def text_width(s, size, bold=False):
    """Rough width for the sans stack above; errs on the wide side."""
    narrow = sum(c in "ilj.,:;|!'· ()" for c in s)
    wide = sum(c in "MWmw@" for c in s)
    return size * (0.56 * (len(s) - narrow - wide) + 0.3 * narrow + 0.85 * wide) * (1.06 if bold else 1.0)


def icon(kind, cx, cy, color):
    """White line icon on a coloured disc, 36 px."""
    st = 'fill="none" stroke="#ffffff" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"'
    o = [f'<circle cx="{cx}" cy="{cy}" r="18" fill="{color}"/>']
    p = {
        "book": [f'M{cx} {cy - 6} C{cx - 3} {cy - 9} {cx - 8} {cy - 9} {cx - 10} {cy - 8} V{cy + 7} C{cx - 8} {cy + 6} {cx - 3} {cy + 6} {cx} {cy + 9} Z',
                 f'M{cx} {cy - 6} C{cx + 3} {cy - 9} {cx + 8} {cy - 9} {cx + 10} {cy - 8} V{cy + 7} C{cx + 8} {cy + 6} {cx + 3} {cy + 6} {cx} {cy + 9}'],
        "chip": [f'M{cx - 6} {cy - 6} H{cx + 6} V{cy + 6} H{cx - 6} Z',
                 f'M{cx - 3} {cy - 6} V{cy - 10} M{cx + 3} {cy - 6} V{cy - 10} M{cx - 3} {cy + 6} V{cy + 10} M{cx + 3} {cy + 6} V{cy + 10}',
                 f'M{cx - 6} {cy - 3} H{cx - 10} M{cx - 6} {cy + 3} H{cx - 10} M{cx + 6} {cy - 3} H{cx + 10} M{cx + 6} {cy + 3} H{cx + 10}'],
        "list": [f'M{cx - 4} {cy - 6} H{cx + 9} M{cx - 4} {cy} H{cx + 9} M{cx - 4} {cy + 6} H{cx + 6}',
                 f'M{cx - 9} {cy - 6} h0.5 M{cx - 9} {cy} h0.5 M{cx - 9} {cy + 6} h0.5'],
        "wifi": [f'M{cx - 10} {cy - 2} A14 14 0 0 1 {cx + 10} {cy - 2}', f'M{cx - 6} {cy + 2} A8.5 8.5 0 0 1 {cx + 6} {cy + 2}'],
    }[kind]
    o += [f'<path d="{d}" {st}/>' for d in p]
    if kind == "wifi":
        o.append(f'<circle cx="{cx}" cy="{cy + 7}" r="2" fill="#ffffff"/>')
    return o


def svg(w, h, body, title, theme):
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}" role="img" aria-label="{escape(title)}">\n'
            f'<title>{escape(title)}</title>\n<g font-family="{FONT}">\n' + "\n".join(body) + "\n</g>\n</svg>\n")


def save(name, w, h, draw, title):
    """draw(theme_dict, theme_name) -> body list; writes name.svg and name-dark.svg."""
    for t in THEMES:
        path = os.path.join(OUT, f"{name}{'' if t == 'light' else '-dark'}.svg")
        with open(path, "w", encoding="utf-8", newline="\n") as f:
            f.write(svg(w, h, draw(THEMES[t], t), title, t))
    return name


def tint(tone, t):
    c, light, dark = TONES[tone]
    return c, (light if t == "light" else dark)


def txt(x, y, s, size, color, bold=False, anchor="start"):
    b = ' font-weight="600"' if bold else ""
    return f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" text-anchor="{anchor}"{b}>{escape(s)}</text>'


# ------------------------------------------------------------------ menu cards

def card(name, kind, tone, title, sub, w=300, h=72):
    def draw(th, t):
        c, _ = tint(tone, t)
        return [f'<rect x="0.75" y="0.75" width="{w - 1.5}" height="{h - 1.5}" rx="12" fill="{th["card"]}" stroke="{th["line"]}" stroke-width="1.5"/>',
                f'<rect x="0.75" y="{h / 2 - 16}" width="3.5" height="32" rx="1.75" fill="{c}"/>',
                *icon(kind, 38, h / 2, c),
                txt(68, h / 2 - 3, title, 17, th["ink"], bold=True),
                txt(68, h / 2 + 17, sub, 13, th["sub"]),
                f'<path d="M{w - 26} {h / 2 - 6} L{w - 20} {h / 2} L{w - 26} {h / 2 + 6}" fill="none" stroke="{th["arrow"]}" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>']
    return save(name, w, h, draw, f"{title}: {sub}")


# ------------------------------------------------------------- pills and strips

def pill(name, number, label, tone="neutral", active=False, chevron=False):
    """Section or step button: number disc + label; chevron adds the step arrow on the right."""
    h, size = 30, 13
    tw = text_width(label, size, bold=True)
    w_pill = 30 + tw + 14
    w = w_pill + (22 if chevron else 0)

    def draw(th, t):
        c, tt = tint(tone, t)
        fill, stroke, ink = (tt, c, th["ink"]) if active else (th["card"], th["line"], th["ink"])
        o = [f'<rect x="0.75" y="0.75" width="{w_pill - 1.5}" height="{h - 1.5}" rx="{(h - 1.5) / 2}" fill="{fill}" stroke="{stroke}" stroke-width="1.5"/>',
             f'<circle cx="15" cy="{h / 2}" r="10" fill="{c}"/>',
             txt(15, h / 2 + 4.3, str(number), 12, "#ffffff", bold=True, anchor="middle"),
             txt(30, h / 2 + 4.6, label, size, ink, bold=True)]
        if chevron:
            x = w_pill + 8
            o.append(f'<path d="M{x} {h / 2 - 5} L{x + 5} {h / 2} L{x} {h / 2 + 5}" fill="none" stroke="{th["arrow"]}" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>')
        return o
    return save(name, round(w + 1), h, draw, f"{number} · {label}")


# ---------------------------------------------------------------- state strips

class Strip:
    """Compact left-to-right state diagram. Nodes: (id, x, y, w, title, sub, tone)."""

    def __init__(self, w, h, title):
        self.w, self.h, self.title, self.nodes, self.edges, self.labels = w, h, title, {}, [], []

    def node(self, nid, x, y, w, title, sub, tone, start=False):
        self.nodes[nid] = (x, y, w, title, sub, tone, start)

    def edge(self, pts, label=None, at=None):
        self.edges.append((pts, label, at))

    def draw(self, th, t):
        o = []
        for pts, label, at in self.edges:
            d = "M" + " L".join(f"{x} {y}" for x, y in pts)
            o.append(f'<path d="{d}" fill="none" stroke="{th["arrow"]}" stroke-width="1.6" stroke-linejoin="round"/>')
            (x1, y1), (x2, y2) = pts[-2], pts[-1]
            dx, dy = (x2 > x1) - (x2 < x1), (y2 > y1) - (y2 < y1)
            o.append(f'<path d="M{x2 - 6 * dx - 4 * dy} {y2 - 6 * dy - 4 * dx} L{x2} {y2} L{x2 - 6 * dx + 4 * dy} {y2 - 6 * dy + 4 * dx}" fill="none" stroke="{th["arrow"]}" stroke-width="1.6" stroke-linecap="round" stroke-linejoin="round"/>')
            if label:
                o.append(txt(at[0], at[1], label, 11.5, th["sub"], anchor=at[2] if len(at) > 2 else "middle"))
        for x, y, w, title, sub, tone, start in self.nodes.values():
            c, tt = tint(tone, t)
            h = 46 if sub else 32
            r = h / 2 if start else 9
            o.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{r}" fill="{tt}" stroke="{c}" stroke-width="1.4"/>')
            if sub:
                o += [txt(x + w / 2, y + 19, title, 13.5, th["ink"], bold=True, anchor="middle"),
                      txt(x + w / 2, y + 35, sub, 11.5, th["sub"], anchor="middle")]
            else:
                o.append(txt(x + w / 2, y + 20.5, title, 13.5, th["ink"], bold=True, anchor="middle"))
        return o

    def save(self, name):
        return save(name, self.w, self.h, self.draw, self.title)


def wifi_startup():
    s = Strip(720, 112, "Wi-Fi start-up: join the stored network, otherwise open the hotspot until the next restart")
    s.node("on", 4, 40, 112, "Power on", "or restart", "neutral", start=True)
    s.node("join", 168, 40, 168, "Joining network", "up to the join time", "busy")
    s.node("net", 432, 6, 284, "On the network", "reconnects by itself after a drop", "good")
    s.node("ap", 432, 60, 284, "Hotspot WiFi-BridgeTom", "10.10.10.10 · until the next restart", "fallback")
    s.edge([(116, 63), (166, 63)])
    s.edge([(336, 55), (376, 55), (376, 29), (430, 29)], "found", (404, 22))
    s.edge([(336, 71), (376, 71), (376, 83), (430, 83)], "not found", (404, 100))
    return s.save("state-wifi-startup")


def settings_trial():
    s = Strip(720, 140, "Settings trial: press Keep within 10 minutes, otherwise the previous settings come back")
    s.node("save", 4, 62, 150, "Saved", "Wi-Fi, hotspot or port", "neutral", start=True)
    s.node("trial", 206, 62, 160, "On trial", "Keep within 10 min", "busy")
    s.node("kept", 462, 28, 254, "Kept", "new settings stay", "good")
    s.node("back", 462, 86, 254, "Previous settings back", "bridge restarts with them", "fallback")
    s.edge([(154, 85), (204, 85)])
    s.edge([(366, 77), (406, 77), (406, 51), (460, 51)], "Keep", (433, 44))
    s.edge([(366, 93), (406, 93), (406, 109), (460, 109)], "10 min or 3rd start", (400, 124, "end"))
    s.edge([(250, 62), (250, 30), (322, 30), (322, 60)], "restart: 10 min again", (286, 22))
    return s.save("state-settings-trial")


# ---------------------------------------------------------------- preview page

def preview(path, items):
    rows = []
    for t in THEMES:
        th = THEMES[t]
        suf = "" if t == "light" else "-dark"
        blocks = []
        for kind, names in items:
            imgs = " ".join(f'<img src="{os.path.relpath(os.path.join(OUT, n + suf + ".svg"), os.path.dirname(path))}" alt="">' for n in names)
            blocks.append(f'<p style="margin:14px 0">{imgs}</p>')
        rows.append(f'<section style="background:{th["bg"]};padding:24px 32px"><h3 style="color:{th["ink"]};font:600 15px sans-serif">{t}</h3>{"".join(blocks)}</section>')
    with open(path, "w", encoding="utf-8") as f:
        f.write('<!doctype html><meta charset="utf-8"><title>Navigation graphics</title><body style="margin:0">' + "".join(rows))


# ------------------------------------------------------------------- contents
# Section menus: page key -> (tone, [(label, anchor), ...]); one pill per H2 section.
MENUS = {
    "manual": ("use", [("Quick start", "quick-start"), ("How it behaves", "behaviour"),
                       ("Web pages", "web-pages"), ("Troubleshooting", "troubleshooting")]),
}
# Step strips: strip key -> (tone, [(label, anchor), ...]); pills joined by chevrons.
STEPS = {
    "use": ("use", [("Power on", "power-on"), ("Join the Wi-Fi", "join-wifi"),
                    ("Open the status page", "open-status"), ("Connect OpenSoar", "connect-opensoar")]),
    "build": ("build", [("Parts", "parts"), ("Housing", "housing"), ("Wiring", "wiring"),
                        ("Power", "power"), ("Flash", "flash"), ("Bench check", "bench-check")]),
}


def picture(name, alt, rel, height=None, width=None):
    """<picture> markup for a generated graphic; rel = path from the page to docs/images/nav."""
    size = f' height="{height}"' if height else f' width="{width}"' if width else ""
    return (f'<picture><source media="(prefers-color-scheme: dark)" srcset="{rel}/{name}-dark.svg">'
            f'<img src="{rel}/{name}.svg" alt="{escape(alt)}"{size}></picture>')


def links(prefix, items, rel):
    return " ".join(f'<a href="#{a}">{picture(f"{prefix}-{i}", f"{i} · {l}", rel, height=30)}</a>'
                    for i, (l, a) in enumerate(items, 1))


if __name__ == "__main__":
    os.makedirs(OUT, exist_ok=True)
    cards = [card("card-use", "book", "use", "Use the bridge", "Connect, pages, behaviour"),
             card("card-build", "chip", "build", "Build one", "Parts to bench check"),
             card("card-reference", "list", "ref", "Reference", "Design, project, research")]
    menus = {k: [pill(f"menu-{k}-{i}", i, l, tone) for i, (l, _) in enumerate(items, 1)]
             for k, (tone, items) in MENUS.items()}
    strips = {k: [pill(f"step-{k}-{i}", i, l, tone, chevron=(i < len(items))) for i, (l, _) in enumerate(items, 1)]
              for k, (tone, items) in STEPS.items()}
    states = [wifi_startup(), settings_trial()]
    if "--preview" in sys.argv:
        preview(sys.argv[sys.argv.index("--preview") + 1],
                [("cards", cards)] + [(k, v) for k, v in menus.items()] + [(k, v) for k, v in strips.items()]
                + [("wifi", states[:1]), ("trial", states[1:])])
    if "--snippets" in sys.argv:  # markup to paste, for pages in docs/<folder>/
        rel = "../images/nav"
        for k, (_, items) in MENUS.items():
            print(f"menu {k}:\n🧭 {links(f'menu-{k}', items, rel)}\n")
        for k, (_, items) in STEPS.items():
            print(f"steps {k}:\n{links(f'step-{k}', items, rel)}\n")
