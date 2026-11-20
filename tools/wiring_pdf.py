"""Build the printable bench wiring instructions (A4 PDF).

Usage (from the repository root):  python tools/wiring_pdf.py [--dirty]
Writes output/pdf/bridge-wiring-instructions.pdf from docs/hardware/wire-installation.md
(the illustrated steps) and the bench checks in docs/hardware/wiring.md, printed with
headless Edge or Chrome. The footer names the last commit that changed those pages or
their pictures, so commit them first; --dirty builds anyway and marks the revision as
"+ local changes".
Rebuild after changing either page, then commit the PDF in its own commit.
Needs markdown-it-py (pip install markdown-it-py).
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path
from urllib.parse import urljoin

try:
    from markdown_it import MarkdownIt
except ImportError:
    sys.exit("markdown-it-py is missing: pip install markdown-it-py")

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "docs/hardware"
OUT = ROOT / "output/pdf/bridge-wiring-instructions.pdf"
HOUSING = "v16"
SOURCES = ["docs/hardware", "docs/images/wire-installation", "docs/images/pins"]
BROWSERS = [r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
            r"C:\Program Files\Microsoft\Edge\Application\msedge.exe",
            r"C:\Program Files\Google\Chrome\Application\chrome.exe",
            "msedge", "google-chrome", "chromium", "chromium-browser"]


def git(*args):
    return subprocess.run(["git", "-C", str(ROOT), *args], check=True, capture_output=True, text=True).stdout.strip()


def browser():
    for b in BROWSERS:
        if Path(b).is_file() or shutil.which(b):
            return b
    sys.exit("Neither Edge nor Chrome found; add its path to BROWSERS.")


rev = git("log", "-1", "--format=%h", "--", *SOURCES)  # last commit that changed the pages and their pictures
if git("status", "--porcelain", "--", *SOURCES):
    if "--dirty" not in sys.argv:
        sys.exit(f"Uncommitted changes under {', '.join(SOURCES)}: commit them first (or pass --dirty).")
    rev += " + local changes"

md = MarkdownIt("commonmark", {"html": True}).enable("table")
base = f"https://github.com/MomoRC-tech/Becker-AR6201-to-OpenSoar/blob/{rev.split()[0]}/docs/hardware/"
install = (SRC / "wire-installation.md").read_text("utf-8")
wire = (SRC / "wiring.md").read_text("utf-8")


def fmt(s, owner="wire-installation.md"):
    s = re.sub(r'<a id="[^"]+"></a>', "", s).strip().removesuffix("---").strip()
    s = s.replace("- [ ]", "- ☐").replace("- [x]", "- ☑")
    h = md.render(s)
    h = re.sub(r'src="(\.\./images/[^"]+)"', lambda m: 'src="' + (SRC / m[1]).resolve().as_uri() + '"', h)
    h = re.sub(r'href="([^"]+)"', lambda m: 'href="' + urljoin(base + owner, m[1]) + '"', h)
    return h


pages = []


def page(title, body, label="Harness assembly"):
    pages.append(f'<section><header>BECKER AR6201 / OPENSOAR <span>HOUSING {HOUSING}</span></header><aside>{label}</aside>'
                 f'<h1>{title}</h1>{body}<footer>Bench prototype &nbsp; · &nbsp; Wiring revision {rev} '
                 f'<span>{len(pages) + 1}</span></footer></section>')


def checks(a, b):  # bench bring-up checks a..b from wiring.md
    return fmt("\n".join(l for l in wire.splitlines()
                         if (m := re.match(r"- \[[ x]\] \*\*(\d+)\.", l)) and a <= int(m[1]) <= b), "wiring.md")


sects = re.split(r"^## \d+\. [^\n]+\n", install, flags=re.M)[1:]
if len(sects) < 7:
    sys.exit(f"wire-installation.md has {len(sects)} numbered sections, 7 expected: update this script.")

page("Before soldering",
     '<p class="lead">Build and check the harness with all instruments disconnected. Start with the supply and loose boards. '
     "Switch off power before making connections.</p><p>The illustrated routes come from the accepted housing model. Physical fit, "
     "wire cut lengths and wired electrical operation still require bench checks. Leave service slack and trim at the destination.</p>"
     "<h2>Supply and board identification</h2>" + checks(1, 2) + checks(4, 5) +
     "<p>The standalone ESP32 loader milestone (check 3) is already recorded in the project. Keep USB unplugged whenever the UBEC "
     "supplies the ESP32.</p><h2>Reference documents</h2>" +
     fmt("[Illustrated installation](wire-installation.md) · [Harness cutting table](housing.md#harness-cutting) · "
         "[Power and protection](power.md) · [Full bench checklist](wiring.md#bring-up)"),
     "Bench preparation")
s = sects[0]
imgs = re.findall(r"!\[[^\n]*?\]\([^)]+\)", s)
steps = re.findall(r"^\d\. .*$", s, re.M)
page("1. Prepare the DE-9 pigtail",
     fmt(imgs[0]) + '<p class="lead">Male housing connector, viewed from the solder cups. The mating face is mirrored. '
     "Read the moulded numbers on the connector itself.</p>" + fmt("\n".join(steps[2:4]).replace("3. ", "1. ", 1)) +
     "<p>Black return leaves cup 6 of the tinned bridge. Cups 3, 7 and 6 share GND-J.</p>")
page("1. Wire the XT30 and seat the connectors", fmt(imgs[1] + "\n\n" + "\n".join(steps[:2])) + fmt(steps[4][3:]))
page("2. Lay the power harness", fmt(sects[1]))
s = sects[2].split("Route the **grey**")
page("3. Wire the RS-232 boards", fmt(s[0]))
s = "Route the **grey**" + s[1]
im = re.search(r"!\[.*?\]\([^)]+\)", s)[0]
page("3. Fit the supply voltage sense wiring",
     fmt(im) + '<p class="lead">XT30 + → 100 kΩ R-V1 → grey wire → J2-5 (GPIO34).</p>' + fmt(s.split("<a id=")[0]) +
     "<p>The divider and capacitor are shown here. The overview images for the XT30, RS-232 boards and finished harness omit them.</p>")
for n, title in [(4, "Connect Zander and FLARM"), (5, "Wire the MAX490 header"), (6, "Fit the Becker pairs"), (7, "Close up and check")]:
    body = fmt(sects[n - 1].split("<kbd>")[0])
    if n == 5:
        body += ('<div class="box"><b>Protect GPIO16.</b> MAX490 RO runs near 5 V. Use R-B1 (2.2 kΩ) in series and R-B2 (3.3 kΩ) '
                 "to ground. GPIO17 goes straight to TXD, with no resistor. Keep blue insulation on both sides of R-B1.</div>")
    page(f"{n}. {title}", body)
xt30 = wire.index("XT30 side")
table = wire[wire.rfind("\n|", 0, wire.index("\n| ---", xt30)):wire.index("\n\n", wire.index("| 9 |", xt30))]
page("Check the finished harness",
     '<p class="lead">Before applying power, complete the end-to-end check through the mated connector. Keep the instruments '
     "disconnected throughout.</p>" + checks(8, 8) + "<h2>DE-9 continuity reference</h2>" + fmt(table, "wiring.md") +
     "<h2>Powered checks</h2>" + checks(6, 7), "Bench verification")
page("Test at the DE-9",
     '<p class="lead">Use a female DE-9 breakout and a PC USB RS-232 adapter. Once the modules are soldered, never fit a loopback '
     "jumper on the ESP32 itself.</p>" + checks(9, 12) +
     "<p>A passing radio loop can hide a reversal of both pairs. Check the individual pin assignments and the receiver polarity on "
     "the preceding page.</p><h2>Calibrate the voltage input</h2>" + checks(13, 13) +
     '<div class="box">Record the wired test results before connecting instruments. Aircraft integration requires separate approval.</div>',
     "Bench verification")

CSS = """@page{size:A4;margin:0}*{box-sizing:border-box}body{margin:0;font:11pt/1.38 'Segoe UI',sans-serif;color:#172d39}section{height:296mm;padding:12mm 16mm 15mm;position:relative;break-after:page}section:last-child{break-after:auto}header,footer{font-size:9pt;color:#395e70;border-bottom:1px solid #a7bec5;padding-bottom:3mm;font-weight:600}span{float:right}aside{color:#066d71;text-transform:uppercase;letter-spacing:1px;font-size:9pt;font-weight:bold;margin-top:6mm}h1{font-size:23pt;line-height:1.13;margin:2mm 0 4mm}h2{font-size:15pt;color:#155961;margin:4mm 0 2mm}p{margin:2.7mm 0}li{margin-bottom:2mm}ol,ul{padding-left:7mm}img{display:block;width:100%;height:88mm;object-fit:contain}a{color:#1d6073}table{border-collapse:collapse;width:100%;font-size:10.5pt}td,th{padding:1.5mm 2.5mm;text-align:left;border-bottom:1px solid #becdd0}th,.box{background:#e8f2f2}.box{padding:3mm;margin-top:4mm;border-left:3px solid teal}.lead{font-size:12pt;font-weight:600}footer{position:absolute;bottom:8mm;left:16mm;right:16mm;border:0;border-top:1px solid #a7bec5;padding-top:2mm}"""

OUT.parent.mkdir(parents=True, exist_ok=True)
with tempfile.TemporaryDirectory() as tmp:
    html = Path(tmp) / "guide.html"
    html.write_text("<meta charset=utf-8><title>Bridge wiring instructions</title><style>" + CSS + "</style>" + "".join(pages), "utf-8")
    before = OUT.stat().st_mtime if OUT.exists() else 0
    subprocess.run([browser(), "--headless", "--disable-gpu", "--no-pdf-header-footer", f"--user-data-dir={Path(tmp) / 'profile'}",
                    f"--print-to-pdf={OUT}", html.as_uri()], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
if not OUT.exists() or OUT.stat().st_mtime <= before:
    sys.exit("The browser did not write the PDF.")
count = len(re.findall(rb"/Type\s*/Page\b", OUT.read_bytes()))
print(f"{OUT.relative_to(ROOT)}: {count} pages, {OUT.stat().st_size:,} bytes, wiring revision {rev}")
if count != len(pages):
    print(f"Warning: {len(pages)} sections but {count} PDF pages; a page overflows. Check the layout.")
