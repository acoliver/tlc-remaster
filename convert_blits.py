#!/usr/bin/env python3
import re
import os
import sys

def convert_file(filepath):
    """Convert all blit/masked_blit/stretch_blit/draw_sprite/rotate_sprite calls in a file"""
    if not os.path.exists(filepath):
        print(f"File not found: {filepath}")
        return False
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    original_content = content
    
    # Count before
    blit_count = len(re.findall(r'\bblit\s*\(', content))
    masked_blit_count = len(re.findall(r'\bmasked_blit\s*\(', content))
    stretch_blit_count = len(re.findall(r'\bstretch_blit\s*\(', content))
    masked_stretch_count = len(re.findall(r'\bmasked_stretch_blit\s*\(', content))
    draw_sprite_count = len(re.findall(r'\bdraw_sprite\s*\(', content))
    draw_sprite_v_flip_count = len(re.findall(r'\bdraw_sprite_v_flip\s*\(', content))
    draw_sprite_h_flip_count = len(re.findall(r'\bdraw_sprite_h_flip\s*\(', content))
    draw_sprite_vh_flip_count = len(re.findall(r'\bdraw_sprite_vh_flip\s*\(', content))
    rotate_sprite_count = len(re.findall(r'\brotate_sprite\s*\(', content))
    
    total = blit_count + masked_blit_count + stretch_blit_count + masked_stretch_count + \
            draw_sprite_count + rotate_sprite_count + draw_sprite_v_flip_count + \
            draw_sprite_h_flip_count + draw_sprite_vh_flip_count
    
    if total == 0:
        return False
    
    print(f"\n{filepath}:")
    print(f"  Found: {total} calls (blit:{blit_count}, masked_blit:{masked_blit_count}, " +
          f"stretch_blit:{stretch_blit_count}, masked_stretch_blit:{masked_stretch_count}, " +
          f"draw_sprite:{draw_sprite_count}, draw_sprite_v_flip:{draw_sprite_v_flip_count}, " +
          f"draw_sprite_h_flip:{draw_sprite_h_flip_count}, draw_sprite_vh_flip:{draw_sprite_vh_flip_count}, " +
          f"rotate_sprite:{rotate_sprite_count})")
    
    # Store a flag for whether we need to set target at function start
    lines = content.split('\n')
    
    # Replace blit/masked_blit calls
    # Pattern: blit(src, dest, sx, sy, dx, dy, w, h)
    # Replace with: al_set_target_bitmap(dest); al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0);
    pattern = r'\b(masked_)?blit\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    
    def replace_blit(match):
        masked = match.group(1)
        src = match.group(2).strip()
        dest = match.group(3).strip()
        sx = match.group(4).strip()
        sy = match.group(5).strip()
        dx = match.group(6).strip()
        dy = match.group(7).strip()
        w = match.group(8).strip()
        h = match.group(9).strip()
        return f"al_set_target_bitmap({dest}); al_draw_bitmap_region({src}, {sx}, {sy}, {w}, {h}, {dx}, {dy}, 0)"
    
    content = re.sub(pattern, replace_blit, content)
    
    # Replace stretch_blit/masked_stretch_blit
    # Pattern: stretch_blit(src, dest, sx, sy, sw, sh, dx, dy, dw, dh)
    # Replace with: al_set_target_bitmap(dest); al_draw_scaled_bitmap(src, sx, sy, sw, sh, dx, dy, dw, dh, 0);
    pattern = r'\b(masked_)?stretch_blit\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    
    def replace_stretch_blit(match):
        masked = match.group(1)
        src = match.group(2).strip()
        dest = match.group(3).strip()
        sx = match.group(4).strip()
        sy = match.group(5).strip()
        sw = match.group(6).strip()
        sh = match.group(7).strip()
        dx = match.group(8).strip()
        dy = match.group(9).strip()
        dw = match.group(10).strip()
        dh = match.group(11).strip()
        return f"al_set_target_bitmap({dest}); al_draw_scaled_bitmap({src}, {sx}, {sy}, {sw}, {sh}, {dx}, {dy}, {dw}, {dh}, 0)"
    
    content = re.sub(pattern, replace_stretch_blit, content)
    
    # Replace draw_sprite_vh_flip(dest, src, x, y)
    pattern = r'\bdraw_sprite_vh_flip\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    def replace_draw_sprite_vh_flip(match):
        dest = match.group(1).strip()
        src = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f"al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL)"
    content = re.sub(pattern, replace_draw_sprite_vh_flip, content)
    
    # Replace draw_sprite_v_flip(dest, src, x, y)
    pattern = r'\bdraw_sprite_v_flip\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    def replace_draw_sprite_v_flip(match):
        dest = match.group(1).strip()
        src = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f"al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, ALLEGRO_FLIP_VERTICAL)"
    content = re.sub(pattern, replace_draw_sprite_v_flip, content)
    
    # Replace draw_sprite_h_flip(dest, src, x, y)
    pattern = r'\bdraw_sprite_h_flip\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    def replace_draw_sprite_h_flip(match):
        dest = match.group(1).strip()
        src = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f"al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, ALLEGRO_FLIP_HORIZONTAL)"
    content = re.sub(pattern, replace_draw_sprite_h_flip, content)
    
    # Replace draw_sprite(dest, src, x, y)
    pattern = r'\bdraw_sprite\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    def replace_draw_sprite(match):
        dest = match.group(1).strip()
        src = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        return f"al_set_target_bitmap({dest}); al_draw_bitmap({src}, {x}, {y}, 0)"
    content = re.sub(pattern, replace_draw_sprite, content)
    
    # Replace rotate_sprite(dest, src, x, y, angle_fixed)
    # This one is more complex - need to calculate center
    pattern = r'\brotate_sprite\s*\(\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^,]+),\s*([^)]+)\)'
    def replace_rotate_sprite(match):
        dest = match.group(1).strip()
        src = match.group(2).strip()
        x = match.group(3).strip()
        y = match.group(4).strip()
        angle_fixed = match.group(5).strip()
        # We'll use a temp var approach for center calculation
        return (f"al_set_target_bitmap({dest}); " +
                f"{{ float _cx = al_get_bitmap_width({src}) / 2.0f; " +
                f"float _cy = al_get_bitmap_height({src}) / 2.0f; " +
                f"float _angle_rad = ((float)fixtof({angle_fixed})) * ALLEGRO_PI * 2.0f / 256.0f; " +
                f"al_draw_rotated_bitmap({src}, _cx, _cy, {x} + _cx, {y} + _cy, _angle_rad, 0); }}")
    content = re.sub(pattern, replace_rotate_sprite, content)
    
    if content != original_content:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"  [OK] Converted and saved")
        return True
    else:
        print(f"   No changes made")
        return False

# Main execution
files = [
    "src/ModuleTitleScreen.cpp", "src/ScrollBox.cpp", "src/MiniWindow.cpp", 
    "src/TileScroller.cpp", "src/ModuleCrewHire.cpp", "src/ModuleMiniGame.cpp",
    "src/ModuleSideViewer.cpp", "src/ModuleShipConfig.cpp", "src/ModuleCantina.cpp",
    "src/PlanetaryBody.cpp", "src/ModuleStartup.cpp", "src/Label.cpp",
    "src/ModuleCargoWindow.cpp", "src/ModuleStarmap.cpp", "src/PlanetSurfaceObject.cpp",
    "src/ModuleSolarSystem.cpp", "src/ModuleQuestLog.cpp", "src/ModuleCaptainsLounge.cpp",
    "src/ModuleInterstellar.cpp", "src/ModuleStarport.cpp", "src/ModuleEncounter.cpp",
    "src/MessageBoxWindow.cpp", "src/ModuleMessageGUI.cpp", "src/ModulePlanetOrbit.cpp",
    "src/ModuleTopGUI.cpp", "src/ModulePlanetSurface.cpp", "src/ModuleCredits.cpp",
    "src/ModuleEngineer.cpp", "src/Sprite.cpp"
]

print("Converting blit/masked_blit/stretch_blit/draw_sprite/rotate_sprite to Allegro 5...")
converted_count = 0
for filepath in files:
    if convert_file(filepath):
        converted_count += 1

print(f"\n[OK] Converted {converted_count} files")
print("\nRun this to verify:")
print("  grep -R 'blit(' src --include='*.cpp' | wc -l")
