#!/usr/bin/env python3
import re
import glob

# Find all .cpp files with these errors
cpp_files = glob.glob("src/*.cpp")

for filepath in cpp_files:
    with open(filepath, 'r') as f:
        content = f.read()
    
    original = content
    
    # Fix pattern: al_get_bitmap_height(img, x, y, z) -> al_get_bitmap_height(img), x, y, z
    pattern = r'al_get_bitmap_height\(([^,)]+),\s*([^)]+)\)'
    
    def fix_height(match):
        img = match.group(1).strip()
        rest = match.group(2).strip()
        return f'al_get_bitmap_height({img}), {rest}'
    
    content = re.sub(pattern, fix_height, content)
    
    # Fix draw_trans_sprite(src, dest, x, y) 
    pattern2 = r'draw_trans_sprite\(([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\);'
    def fix_draw_trans(match):
        src = match.group(1).strip()
        dest = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f'al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, 0);'
    
    content = re.sub(pattern2, fix_draw_trans, content)
    
    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Fixed {filepath}")

print("Done!")
