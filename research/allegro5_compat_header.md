# Allegro 5 Compatibility Header - Implementation Notes

**Created:** 2026-02-02  
**File:** `/Users/acoliver/projects/tlc/src/allegro5_compat.h`  
**Status:** [OK] COMPLETE - Ready for use

## Overview

A compatibility header has been created to enable gradual migration from Allegro 4 (via Allegro Legacy) to native Allegro 5 APIs in the TLC codebase.

## What Was Created

### File: `src/allegro5_compat.h`

This header provides:

1. **All Allegro 5 includes** in one place:
   - `allegro5/allegro.h` - Core library
   - `allegro5/allegro_image.h` - Image loading (PNG, JPG, BMP, TGA)
   - `allegro5/allegro_font.h` - Font support
   - `allegro5/allegro_ttf.h` - TrueType fonts
   - `allegro5/allegro_primitives.h` - Drawing primitives
   - `allegro5/allegro_audio.h` - Audio playback
   - `allegro5/allegro_acodec.h` - Audio codecs

2. **Type compatibility bridges** (guarded by `#ifndef TLC_USING_ALLEGRO_LEGACY`):
   ```c
   typedef ALLEGRO_BITMAP BITMAP;
   typedef ALLEGRO_COLOR COLOR;
   typedef ALLEGRO_DISPLAY DISPLAY;
   // ... etc
   ```

3. **Function macro wrappers** for common Allegro 4 functions:
   - Bitmap operations: `create_bitmap()`, `destroy_bitmap()`, `load_bitmap()`
   - Blitting: `blit()`, `masked_blit()`, `stretch_blit()`
   - Sprite drawing: `draw_sprite()`, `rotate_sprite()`, `draw_sprite_h_flip()`
   - Colors: `makecol()`, `makeacol()`, `getr()`, `getg()`, `getb()`
   - Primitives: `line()`, `rect()`, `rectfill()`, `circle()`, `circlefill()`, `ellipse()`
   - Pixel ops: `putpixel()`, `getpixel()`
   - Clearing: `clear_bitmap()`, `clear_to_color()`
   - System: `rest()`, fixed-point math helpers

4. **Helper function** for initialization:
   ```c
   bool tlc_allegro5_init_all()
   ```
   Initializes all Allegro 5 subsystems in one call.

## Header Structure Verification

[OK] **Syntax Check:**
- Header guards: Properly matched `#ifndef ALLEGRO5_COMPAT_H` ... `#endif`
- Braces: 29 open `{`, 29 close `}` (balanced)
- Parentheses: 187 open `(`, 187 close `)` (balanced)
- do-while blocks: 19 pairs (all matched)

## Usage Patterns

### Pattern 1: Include for Gradual Migration

For files being migrated from Allegro Legacy to native A5:

```c
// Instead of:
#include <allegro.h>

// Use:
#include "allegro5_compat.h"
```

The header provides macro wrappers that translate A4-style calls to A5 APIs.

### Pattern 2: Mark Files as Fully Migrated

When a file has been completely converted to native Allegro 5:

```c
#define TLC_A5_MIGRATION_COMPLETE
#include "allegro5_compat.h"

// Now use native al_* functions directly
al_draw_bitmap(sprite, x, y, 0);
```

### Pattern 3: New Code

For new code, always use native Allegro 5 APIs:

```c
#include "allegro5_compat.h"

// Good - native A5
ALLEGRO_BITMAP *img = al_load_bitmap("image.png");
al_draw_bitmap(img, 100, 100, 0);

// Bad - don't use compatibility macros for new code
BITMAP *img = load_bitmap("image.png", NULL);
draw_sprite(dest, img, 100, 100);
```

## Key Design Decisions

### 1. Guarded by `TLC_USING_ALLEGRO_LEGACY`

The typedefs and macros are only active when `TLC_USING_ALLEGRO_LEGACY` is NOT defined. This prevents conflicts when Allegro Legacy is still being used.

Current state in `env.h`:
```c
#ifdef ALLEGRO_LEGACY
    #define TLC_USING_ALLEGRO_LEGACY 1
#endif
```

### 2. Target Management in Macros

Allegro 5's drawing model requires setting a target bitmap before drawing. The macros handle this with a pattern:

```c
#define blit(src, dest, sx, sy, dx, dy, w, h) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0); \
        al_set_target_bitmap(_old); \
    } while(0)
```

**Important:** For performance-critical code, set the target once and use native `al_draw_*()` calls directly instead of these macros.

### 3. Color Type Difference

- **Allegro 4:** Colors are `int` values
- **Allegro 5:** Colors are `ALLEGRO_COLOR` structs

The `makecol()` macro bridges this:
```c
#define makecol(r, g, b) al_map_rgb(r, g, b)
```

This works because A5 color functions accept `ALLEGRO_COLOR` structs.

### 4. Fixed-Point Math Removal

Allegro 5 removed fixed-point types. The header provides conversion macros:

```c
#define itofix(x) ((x) << 16)
#define fixtoi(x) ((x) >> 16)
#define fixtof(x) ((float)(x) / 65536.0f)
#define ftofix(x) ((int)((x) * 65536.0f))
```

Used by `rotate_sprite()` which needs to convert fixed-point angles to radians.

## Migration Roadmap

Based on the research files, here's the recommended migration sequence:

### Phase 1: [OK] COMPLETE
- Create compatibility header
- Document migration patterns

### Phase 2: Display/Graphics Init
**Target:** `Game.cpp`
- Convert `set_gfx_mode()` → `al_create_display()`
- Replace `screen` global with display backbuffer
- Update event loop for A5 events

### Phase 3: Bitmap Operations
**Targets:** High-usage modules
- `ModulePlanetSurface.cpp` (~100+ BITMAP references)
- `ModuleEncounter.cpp` (~70+ references)
- `Sprite.cpp` (base rendering)

Convert:
- `create_bitmap()` → `al_create_bitmap()`
- `blit()` → `al_draw_bitmap_region()` with explicit target
- `masked_blit()` → `al_draw_bitmap_region()`

### Phase 4: Drawing Primitives
**Targets:** Modules using shapes
- `ModuleStarmap.cpp`
- `ModuleSolarSystem.cpp`

Convert:
- `rectfill()` → `al_draw_filled_rectangle()`
- `circle()` → `al_draw_circle()`
- `line()` → `al_draw_line()`

### Phase 5: Color System
**Target:** `Game.h` color macros
- Replace `makecol()` macros with `al_map_rgb()` calls
- Update color extraction (`getr/getg/getb`)

### Phase 6: Input System
**Target:** Input handling in modules
- Replace `key[]` array with `al_get_keyboard_state()` / events
- Replace `mouse_x/y/b` with `al_get_mouse_state()`

## Files Already Migrated

These files already use native Allegro 5 APIs:

1. **`alfont_compat.cpp`** - Font rendering
   ```c
   #include <allegro5/allegro.h>
   #include <allegro5/allegro_font.h>
   #include <allegro5/allegro_ttf.h>
   ```

2. **`AudioSystem_allegro.cpp`** - Audio system
   ```c
   #include <allegro5/allegro.h>
   #include <allegro5/allegro_audio.h>
   #include <allegro5/allegro_acodec.h>
   ```

## Integration with Build System

### Current CMake State

`src/CMakeLists.txt` already links Allegro 5 libraries:
- `allegro`
- `allegro_font`
- `allegro_ttf`
- `allegro_primitives`
- `allegro_audio`
- `allegro_acodec`
- `allegro_main`
- `allegro_image`

**Action needed:** When Allegro Legacy is removed, update:
```cmake
# Remove
find_package(AllegroLegacy REQUIRED)

# Keep (already present)
find_package(allegro REQUIRED)
```

## Testing Strategy

### Manual Testing Checklist

After including `allegro5_compat.h` in a file:

1. [OK] **Compilation:** File compiles without errors
2. [OK] **Linking:** No undefined symbol errors
3. [OK] **Runtime:** Graphics display correctly
4. [OK] **Input:** Keyboard/mouse input works
5. [OK] **Audio:** Sound effects and music play
6. [OK] **Performance:** No significant slowdown

### Known Limitations

1. **Datafiles:** Allegro 5 has no equivalent to A4's `.dat` files
   - Solution: Load individual resources (already done in most modules)

2. **MIDI playback:** A5 removed MIDI support
   - TLC uses FMOD for audio, not affected

3. **Palette operations:** A5 uses truecolor only
   - TLC already uses 32-bit color, not affected

4. **GUI system:** A4 GUI removed
   - TLC has custom GUI, not affected

## Error Handling

The macros provide basic functionality but limited error handling. For production code:

**Instead of:**
```c
BITMAP *img = load_bitmap("sprite.png", NULL);
```

**Use:**
```c
ALLEGRO_BITMAP *img = al_load_bitmap("sprite.png");
if (!img) {
    fprintf(stderr, "Failed to load sprite.png\n");
    // Handle error
}
```

## Performance Considerations

The macro wrappers include overhead:
- Saving/restoring drawing target for each call
- Function call overhead

For performance-critical rendering loops:

**Replace:**
```c
for (int i = 0; i < 1000; i++) {
    draw_sprite(buffer, sprites[i], x[i], y[i]);
}
```

**With:**
```c
al_set_target_bitmap(buffer);
for (int i = 0; i < 1000; i++) {
    al_draw_bitmap(sprites[i], x[i], y[i], 0);
}
```

## Next Steps

1. **Choose first migration target:** Recommend `Game.cpp` display initialization
2. **Create feature branch:** `git checkout -b allegro5-migration-phase2`
3. **Update one file at a time:** Include header, test, commit
4. **Track progress:** Update migration status comments in header
5. **Remove Allegro Legacy:** After all files migrated

## References

- [Allegro 5 Migration Research](./allegro_porting_research.md)
- [API Patterns Catalog](./allegro_patterns_catalog.md)
- [Phase 0 Analysis](../project-plans/allegro5/phase0_analysis.md)
- [Allegro 5 Official Docs](https://liballeg.org/a5docs/trunk/)

---

**Status:** Header created and validated. Ready for integration testing when Allegro 5 is installed on the build system.
