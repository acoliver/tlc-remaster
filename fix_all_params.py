#!/usr/bin/env python3
import re
import glob

cpp_files = glob.glob("src/*.cpp")

for filepath in cpp_files:
    with open(filepath, 'r') as f:
        content = f.read()
    
    original = content
    
    # Fix al_get_bitmap_height/width with extra parameters inside
    # Pattern: al_get_bitmap_height(img, x...) -> al_get_bitmap_height(img), x...
    pattern = r'al_get_bitmap_(height|width)\(([^,)]+),\s*([^)]+)\)'
    
    def fix_bitmap_size(match):
        func = match.group(1)
        img = match.group(2).strip()
        rest = match.group(3).strip()
        return f'al_get_bitmap_{func}({img}), {rest}'
    
    content = re.sub(pattern, fix_bitmap_size, content)
    
    # Fix missing final parameter (flags) in al_draw_scaled_bitmap
    # al_draw_scaled_bitmap(..., 0) ) should have the 0 before the )
    pattern2 = r'al_draw_scaled_bitmap\(([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\s*,\s*0\s*\)\s*\);'
    def fix_scaled_bitmap(match):
        args = [match.group(i).strip() for i in range(1, 10)]
        return f'al_draw_scaled_bitmap({", ".join(args)}, 0);'
    
    content = re.sub(pattern2, fix_scaled_bitmap, content)
    
    # Fix draw_trans_sprite
    pattern3 = r'draw_trans_sprite\(([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\);'
    def fix_draw_trans(match):
        src = match.group(1).strip()
        dest = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f'al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, 0);'
    
    content = re.sub(pattern3, fix_draw_trans, content)
    
    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Fixed {filepath}")

print("Done fixing all parameter errors!")
