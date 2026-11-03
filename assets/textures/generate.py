"""Rebuild the original seamless grayscale surface maps (binary PGM)."""
from pathlib import Path
from random import Random

size = 128
rng = Random(27)
for material in ("floor", "metal", "wall", "panel"):
    pixels = bytearray()
    for y in range(size):
        for x in range(size):
            grain = rng.randint(-6, 6)
            if material == "floor":
                edge = min(x, y, size - 1 - x, size - 1 - y)
                value = 80 if edge < 2 else 215 if edge == 2 else 181 + grain
            elif material == "metal":
                value = 181 + grain + ((y * 17) % 11)
            elif material == "wall":
                value = 134 if x < 2 else 207 + grain
            else:
                # Repeating perforations in the fronts of rack-mounted units.
                value = 54 if (x % 16 - 8)**2 + (y % 16 - 8)**2 < 16 else 197 + grain
            pixels.append(value)
    Path(__file__).with_name(material + ".pgm").write_bytes(
        f"P5\n{size} {size}\n255\n".encode("ascii") + pixels)
