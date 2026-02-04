#!/usr/bin/env python3
import re

files_to_fix = [
    "src/ModuleCredits.cpp",
    "src/ModuleEngineer.cpp"
]

for filepath in files_to_fix:
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Fix pattern: al_get_bitmap_height(img, x, y, 0) -> al_get_bitmap_height(img), x, y, 0
    pattern = r'al_get_bitmap_height\(([^,)]+),\s*([^)]+)\)'
    
    def fix_height(match):
        img = match.group(1).strip()
        rest = match.group(2).strip()
        return f'al_get_bitmap_height({img}), {rest}'
    
    new_content = re.sub(pattern, fix_height, content)
    
    # Fix draw_trans_sprite
    pattern2 = r'//(.*)draw_trans_sprite'
    new_content = re.sub(pattern2, r'//\1al_draw_bitmap', new_content)
    
    # Find draw_trans_sprite(img_ship, g_game->GetBackBuffer(), 342+X_OFFSET, 95+viewer_offset_y);
    pattern3 = r'draw_trans_sprite\(([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\);'
    def fix_draw_trans(match):
        src = match.group(1).strip()
        dest = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f'al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, 0);'
    
    new_content = re.sub(pattern3, fix_draw_trans, new_content)
    
    if new_content != content:
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Fixed {filepath}")
    else:
        print(f"No changes for {filepath}")
