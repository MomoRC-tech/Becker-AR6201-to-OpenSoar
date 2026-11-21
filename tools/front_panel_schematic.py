"""Export the front-panel KiCad project as one searchable vector PDF.

Requires KiCad 10 (kicad-cli on PATH or passed as the first argument) and PyMuPDF.
"""

from pathlib import Path
import re
import subprocess
import sys
from tempfile import TemporaryDirectory
from xml.etree import ElementTree

import pymupdf as pdf


ROOT = Path(__file__).resolve().parents[1]
SCHEMATIC = ROOT / "electronics/integrated/front-panel/compact-bridge.kicad_sch"
OUTPUT = ROOT / "docs/images/integrated-front-schematic.pdf"
INK = (0.10, 0.18, 0.23)
ACCENT = (0.12, 0.35, 0.40)
# Positions in PDF points on one landscape A0 page. The source page order is KiCad's.
REGIONS = [
    ("J1   Instrument harness", (55, 155, 510, 1115)),
    ("U1   Controller, expansion and test points", (540, 155, 2165, 1115)),
    ("U2   Zander and FLARM serial ports", (2195, 155, 3315, 665)),
    ("U3   Becker differential interface", (2195, 684, 3315, 1115)),
    ("U4-U7   Power and battery sensing", (55, 1160, 1785, 2315)),
    ("U8   USB programming and status LEDs", (1815, 1160, 3315, 2315)),
]


def circuit_bounds(page, is_root):
    bounds = pdf.Rect()
    for _, coordinates in page.get_bboxlog():
        rect = pdf.Rect(coordinates)
        # The root's five sheet-navigation boxes are to the right of the J1 circuit.
        if not rect.is_empty and (not is_root or rect.x0 < 470):
            bounds |= rect
    return bounds + (-6, -6, 6, 6)


def compose(source, drawing):
    page = drawing.new_page(width=3370, height=2384)
    page.insert_text((55, 68), "Becker bridge | Integrated front-panel schematic",
                     fontname="hebo", fontsize=39, color=INK)
    page.insert_text((57, 104), "Matching net names connect across the drawing. "
                     "Blue crosses mark intentionally unused pins.", fontsize=19, color=INK)
    page.draw_line((55, 129), (3315, 129), color=ACCENT, width=2)
    for index, (source_page, (title, region)) in enumerate(zip(source, REGIONS, strict=True)):
        x, y, right, bottom = region
        page.insert_text((x, y + 20), title, fontname="hebo", fontsize=24, color=ACCENT)
        clip = circuit_bounds(source_page, is_root=index == 0)
        area = pdf.Rect(x, y + 42, right, bottom)
        scale = min(area.width / clip.width, area.height / clip.height)
        target = pdf.Rect(area.x0, area.y0, area.x0 + scale * clip.width,
                          area.y0 + scale * clip.height)
        page.show_pdf_page(target, source, index, clip=clip)
    page.draw_line((55, 1132), (3315, 1132), color=(0.76, 0.81, 0.82), width=1.5)
    page.draw_line((55, 2330), (3315, 2330), color=ACCENT, width=1.5)
    page.insert_text((55, 2360), "FRONT-PANEL PROTOTYPE", fontsize=15, fontname="hebo", color=INK)
    page.insert_text((3160, 2360), "SHEET 1 OF 1", fontsize=16, fontname="hebo", color=INK)
    drawing.set_metadata({"title": "Becker bridge | Complete single-sheet schematic"})


def export(cli):
    with TemporaryDirectory() as temp:
        folder = Path(temp)
        pages, netlist, result = (folder / name for name in ("pages.pdf", "netlist.xml", "result.pdf"))
        subprocess.run([cli, "sch", "export", "pdf", "--exclude-drawing-sheet", "--no-background-color",
                        "--exclude-pdf-property-popups", "-o", pages, SCHEMATIC], check=True)
        subprocess.run([cli, "sch", "export", "netlist", "--format", "kicadxml",
                        "-o", netlist, SCHEMATIC], check=True)
        with pdf.open(pages) as source, pdf.open() as drawing:
            compose(source, drawing)
            drawing.save(result, garbage=4, deflate=True, use_objstms=1)

        refs = {part.attrib["ref"] for part in ElementTree.parse(netlist).findall(".//components/comp")
                if not part.attrib["ref"].startswith("#")}
        with pdf.open(result) as drawing:
            visible = set(re.findall(r"\b[A-Z]+[0-9]+\b", drawing[0].get_text()))
            if len(drawing) != 1 or drawing[0].get_images() or refs - visible:
                raise ValueError(f"Vector export or reference check failed: {sorted(refs - visible)}")
        OUTPUT.write_bytes(result.read_bytes())


if __name__ == "__main__":
    export(sys.argv[1] if len(sys.argv) > 1 else "kicad-cli")
