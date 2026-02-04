# Color API Migration Status

## Completed
- [OK] Game.h color constant macros (BLACK, WHITE, etc.) converted to al_map_rgb
- [OK] Game Print functions now use ALLEGRO_COLOR
- [OK] ScrollBox color members and methods converted
- [OK] MessageBoxWindow color members converted
- [OK] Label color members converted
- [OK] Partial ModulePlanetSurface.cpp getr/getg/getb conversion (1 of 6 instances)

## Remaining Work

### High Priority - Core Systems
- [ ] Complete ModulePlanetSurface.cpp getr/getg/getb conversions (5 remaining)
- [ ] ModuleControlPanel.cpp - heavy makecol usage (~30 instances)
- [ ] ModuleAuxiliaryDisplay.cpp - makecol color variables
- [ ] ModuleBank.cpp - extensive makecol in UI elements
- [ ] ModuleMedical.cpp - makecol button colors

### Medium Priority - Module Files
- [ ] ModuleSettings.cpp - ScrollBox color setup
- [ ] ModuleCaptainCreation.cpp - TEXTCOL macro
- [ ] ModuleCrewHire.cpp - extensive makecol for labels and UI
- [ ] ModuleSolarSystem.cpp - planet color calculations
- [ ] ModulePlanetOrbit.cpp - drawing colors
- [ ] ModuleStarmap.cpp - map and text colors
- [ ] ModuleEngineer.cpp - text color usage

### Low Priority - Misc Files
- [ ] MiniWindow.cpp - PINK color for clearing
- [ ] Sprite.cpp - transparency color
- [ ] All clear_to_color(bmp, makecol(255,0,255)) → transparent clear
- [ ] All clear_bitmap calls

### Compatibility Layer Cleanup
- [ ] Remove makecol/makeacol macros from allegro5_compat.h
- [ ] Remove getr/getg/getb/geta macros from allegro5_compat.h

## Pattern for getr/getg/getb Conversion

OLD:
```cpp
int color, r, g, b;
color = getpixel(bitmap, x, y);
r = getr(color);
g = getg(color);
b = getb(color);
```

NEW:
```cpp
unsigned char r, g, b;
ALLEGRO_COLOR acolor = al_get_pixel(bitmap, x, y);
al_unmap_rgb(acolor, &r, &g, &b);
```

## Pattern for clear_to_color with Magenta

OLD:
```cpp
clear_to_color(bitmap, makecol(255,0,255));
```

NEW:
```cpp
ALLEGRO_BITMAP *old = al_get_target_bitmap();
al_set_target_bitmap(bitmap);
al_clear_to_color(al_map_rgba(0,0,0,0));
al_set_target_bitmap(old);
```

## Compilation Issues to Fix

Current errors are related to int vs ALLEGRO_COLOR type mismatches. Need to:
1. Fix GameState.cpp color usages (appears to still use int for colors)
2. Verify all Button constructor calls use ALLEGRO_COLOR for color params
3. Check all alfont_textprintf_ex calls for color parameters

## Testing Strategy

After each file conversion:
1. Compile and fix immediate errors
2. Test the specific module if possible
3. Commit incrementally

## Notes

- ALLEGRO_COLOR is a struct, not an int - cannot be assigned to/from int
- Default parameter values like WHITE now return ALLEGRO_COLOR
- Some functions may need wrapper functions if they store colors in game state as int
- Consider adding helper functions for common color operations
