# Why We Keep allegro5_compat.h and allegro5_compat_globals.cpp

## TL;DR

These files are **NOT legacy artifacts** - they are **essential infrastructure** that enables the game's Allegro 4-style code to run on modern Allegro 5. Removing them would require rewriting ~40 source files.

## The Confusion

The name "compat" suggests these are temporary shims, but they're actually a **permanent abstraction layer** (unless we do a complete API rewrite).

**Common Misconception**: "Compat files = legacy = should be removed"  
**Reality**: "Compat files = API translation layer = production code"

## What They Actually Do

### allegro5_compat.h
**Purpose**: Provides Allegro 4-compatible APIs on top of Allegro 5

**Functionality**:
- Type aliases (BITMAP → ALLEGRO_BITMAP)
- Function macros (blit() → al_draw_bitmap_region() with target management)
- Color conversion (packed ints ↔ ALLEGRO_COLOR)
- Global state management
- Input system abstraction

**Example**:
```cpp
// Game code writes (Allegro 4 style):
blit(src, dest, sx, sy, dx, dy, w, h);

// Expands to (Allegro 5):
do {
    ALLEGRO_BITMAP *_old = al_get_target_bitmap();
    al_set_target_bitmap(dest);
    al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0);
    al_set_target_bitmap(_old);
} while(0)
```

### allegro5_compat_globals.cpp
**Purpose**: Defines global variables needed by the compat layer

**Provides**:
```cpp
ALLEGRO_BITMAP *_tlc_screen = nullptr;      // Current display backbuffer
ALLEGRO_DISPLAY *_tlc_display = nullptr;    // Display handle
int SCREEN_W = 0, SCREEN_H = 0;             // Display dimensions
int mouse_x, mouse_y, mouse_b;              // Mouse state (A4 style)
ALLEGRO_KEYBOARD_STATE _tlc_keyboard_state; // Keyboard state
bool _tlc_key[ALLEGRO_KEY_MAX];             // Key array (A4 style)
```

## Why They're Needed

### Current Usage
- **40+ source files** use the compatibility macros
- **659 call sites** across the codebase
- Every file that does graphics uses them

**Files that depend on it** (partial list):
- Game.cpp, GameState.cpp
- All Module*.cpp files (30+)
- Sprite.cpp, Button.cpp, Label.cpp
- TileScroller.cpp, PlanetTileScroller.cpp
- And many more...

### What Removal Would Require

To remove these files, we would need to:

1. **Rewrite all bitmap operations** (~300 call sites)
   - Replace `create_bitmap()` with `al_create_bitmap()`
   - Replace `destroy_bitmap()` with `al_destroy_bitmap()`
   - Replace `load_bitmap()` with `al_load_bitmap()`

2. **Rewrite all blitting** (~150 call sites)
   - Replace `blit()` with manual target switching + `al_draw_bitmap_region()`
   - Replace `masked_blit()` similarly
   - Replace `stretch_blit()` with `al_draw_scaled_bitmap()`
   - Manage draw targets explicitly everywhere

3. **Rewrite all colors** (~100 call sites)
   - Replace `makecol(r,g,b)` with `al_map_rgb(r,g,b)`
   - Change all color variables from `int` to `ALLEGRO_COLOR`
   - Update all color comparisons and operations

4. **Rewrite all primitives** (~100 call sites)
   - Replace `rectfill()` with `al_draw_filled_rectangle()`
   - Replace `line()` with `al_draw_line()`
   - Replace `circle()` with `al_draw_circle()`
   - Manage targets and colors for each

5. **Rewrite global state access**
   - Replace `screen` with explicit backbuffer management
   - Replace `SCREEN_W/H` with `al_get_display_*()` calls
   - Replace `mouse_x/y` with `al_get_mouse_state()` calls
   - Replace `key[]` with event-based input

**Total effort**: 2-4 weeks of work, high risk of visual bugs

## Architecture Decision

### Option A: Keep Compatibility Layer (Current)
**Pros**:
- [OK] Works perfectly right now
- [OK] Clean separation between game logic and graphics API
- [OK] Easy to maintain game code
- [OK] No risk of regressions

**Cons**:
- [ERROR] Extra layer of abstraction
- [ERROR] Slightly harder to use new Allegro 5 features
- [ERROR] Name "compat" is confusing

**Verdict**: **Chosen approach** - practical and maintainable

### Option B: Full Native Allegro 5 Conversion
**Pros**:
- [OK] "Pure" Allegro 5 code
- [OK] Direct API access
- [OK] No abstraction overhead

**Cons**:
- [ERROR] Weeks of work
- [ERROR] High risk of visual bugs
- [ERROR] Harder to maintain game logic (graphics details exposed)
- [ERROR] No real benefit (current system is fast)

**Verdict**: Not worth the effort unless specific need arises

## The Real "Legacy" That Was Removed

Phase 1 and 2 removed the **actual legacy**:

### [OK] Removed in Phase 1:
- **DATAFILE system** - Old packed asset format
- **DATAFILE macros** - dat_get_bitmap(), etc.
- **Packed data files** - Converted to individual files

### [OK] Removed in Phase 2:
- **Allegro Legacy library** - No longer compiled or linked
- **Legacy library linkage** - CMake no longer pulls it in
- **CI builds of legacy** - Removed from all platforms

### [OK] Kept (Not Legacy):
- **allegro5_compat.h** - Active API translation layer
- **allegro5_compat_globals.cpp** - Global state provider

## Analogy

Think of it like this:

**Bad analogy**: "Compat files are like training wheels - remove them when ready"

**Good analogy**: "Compat files are like a car's transmission - they translate power (game code) to the wheels (Allegro 5). You don't remove them just because the car is running."

## When Would We Remove Them?

Scenarios where removal makes sense:

1. **Need new Allegro 5 features** that don't work through the compat layer
2. **Major graphics engine rewrite** is planned anyway
3. **Performance issues** traced to the compat layer (unlikely)
4. **Team decision** to modernize the entire codebase

Current assessment: **None of these apply**

## Renaming Consideration

To avoid future confusion, these files could be renamed:

- `allegro5_compat.h` → `allegro_api.h` or `graphics_api.h`
- `allegro5_compat_globals.cpp` → `graphics_globals.cpp`

However, this is cosmetic and not required for functionality.

## Conclusion

The `allegro5_compat.*` files are:

- [OK] **Production code** (not temporary shims)
- [OK] **Architectural layer** (not legacy artifacts)
- [OK] **Well-tested** (working in production)
- [OK] **Efficient** (no performance issues)
- [OK] **Maintainable** (clean abstraction)

They should remain as core infrastructure unless a complete API rewrite is undertaken.

**Phase 2 is complete** because we removed the **actual legacy** (Allegro Legacy library) while keeping the **essential abstraction** (compat layer).

---

**Bottom Line**: Don't confuse "compat" with "legacy". This is a deliberate architectural choice, not technical debt.

**Status**: Production infrastructure [OK]  
**Should Remove**: No [ERROR]  
**Alternative**: Full API rewrite (not recommended)  
