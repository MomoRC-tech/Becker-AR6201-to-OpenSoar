"""Make the status page header image from the housing render.

Crops the lid (logo, "WIFI Bridge", "7-20V") out of
cad/housing/housing-body-v1/preview-top.png, scales it to 240 px wide and
writes it as WebP into firmware/bridge/device_image.h.

    python tools/device_image.py

Needs Pillow. Rerun after a new preview-top.png; check the crop box if the
render's framing changed.
"""

from io import BytesIO
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "cad" / "housing" / "housing-body-v1" / "preview-top.png"
TARGET = ROOT / "firmware" / "bridge" / "device_image.h"
CROP = (463, 28, 934, 651)  # lid top in the 1400 x 960 render, mounting ears excluded
WIDTH = 240


def main() -> None:
    lid = Image.open(SOURCE).convert("RGB").crop(CROP)
    height = round(WIDTH * lid.height / lid.width)
    buf = BytesIO()
    lid.resize((WIDTH, height), Image.LANCZOS).save(buf, "WEBP", quality=80, method=6)
    data = buf.getvalue()
    rows = [", ".join(f"0x{b:02x}" for b in data[i:i + 16]) for i in range(0, len(data), 16)]
    TARGET.write_text(
        "// Project: housing lid as seen from above, for the status page header (step 3a).\n"
        f"// Cropped from cad/housing/housing-body-v1/preview-top.png, {WIDTH} x {height} px, WebP quality 80.\n"
        "// Regenerate with tools/device_image.py.\n\n#pragma once\n\n#include <stdint.h>\n\n"
        "static const uint8_t kDeviceImage[] = {\n    " + ",\n    ".join(rows) + "\n};\n",
        encoding="ascii",
        newline="\n",
    )
    print(f"{TARGET.relative_to(ROOT)}: {WIDTH} x {height} px, {len(data)} bytes")


if __name__ == "__main__":
    main()
