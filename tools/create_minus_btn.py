#!/usr/bin/env python3
"""Create minus button BMPs from the plus button BMPs by replacing the + with a - sign.
The minus bar should be horizontally centered in the button, matching the horizontal
bar of the plus sign."""
from PIL import Image, ImageDraw
import os

base_dir = '/Users/acoliver/projects/tlc/bin/data/captaincreation'

for suffix, out_name in [('captaincreation_plus.bmp', 'captaincreation_minus.bmp'),
                          ('captaincreation_plus_mouseover.bmp', 'captaincreation_minus_mouseover.bmp')]:
    src_path = os.path.join(base_dir, suffix)
    if not os.path.exists(src_path):
        print(f"Source not found: {src_path}")
        continue
    
    img = Image.open(src_path).copy()
    w, h = img.size
    
    # Sample the white interior background from a known white area
    white_bg = img.getpixel((w // 4, 4))
    
    # Wipe the entire interior (between the border) to background
    # Border is the 2-pixel black frame
    border = 2
    for y in range(border, h - border):
        for x in range(border, w - border):
            img.putpixel((x, y), white_bg)
    
    # Now draw the horizontal bar (minus sign) centered vertically
    # Use the bar color from the original plus button
    orig = Image.open(src_path)
    bar_color = orig.getpixel((w // 2, h // 2))  # center of plus = bar color
    
    # The horizontal bar in the plus was roughly from x=3 to x=33, height ~7px centered
    # Let's draw a centered horizontal bar
    bar_height = 7
    bar_margin_x = 3  # pixels from border
    bar_top = (h - bar_height) // 2
    bar_bottom = bar_top + bar_height
    bar_left = border + bar_margin_x
    bar_right = w - border - bar_margin_x
    
    for y in range(bar_top, bar_bottom):
        for x in range(bar_left, bar_right):
            img.putpixel((x, y), bar_color)
    
    out_path = os.path.join(base_dir, out_name)
    img.save(out_path)
    print(f"Saved: {out_path} ({w}x{h}, bar y={bar_top}-{bar_bottom})")

# Also create a disabled version (greyed out)
minus_path = os.path.join(base_dir, 'captaincreation_minus.bmp')
if os.path.exists(minus_path):
    img = Image.open(minus_path).copy()
    for y in range(img.height):
        for x in range(img.width):
            r, g, b = img.getpixel((x, y))[:3]
            gray = int(0.3 * r + 0.59 * g + 0.11 * b)
            gray = min(255, gray + 80)
            img.putpixel((x, y), (gray, gray, gray))
    out_path = os.path.join(base_dir, 'captaincreation_minus_disabled.bmp')
    img.save(out_path)
    print(f"Saved disabled: {out_path}")

print("\nDone!")
