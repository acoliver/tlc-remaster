# Allegro 4 API Patterns Catalog - TLC Codebase

## Analysis Summary

| Metric | Count |
|--------|-------|
| Total .cpp files analyzed | 89 |
| Total .h files analyzed | 90 |
| Files using Allegro APIs | 78 |
| BITMAP type occurrences | ~780+ |
| Graphics function calls | ~320+ |
| Color function calls (makecol) | ~190+ |
| Text rendering calls | ~99 |
| Input function references | ~56 |
| Datafile operations | ~44 |
| **Total API references** | **~1,500+** |

---

## Pattern Category: BITMAP Type

The `BITMAP` struct is the most prevalent Allegro 4 pattern in the codebase.

### Files with Heavy BITMAP Usage (10+ declarations)

| File | BITMAP Declarations | Notes |
|------|---------------------|-------|
| `ModulePlanetSurface.h` | 25+ | UI elements, tilesets, gauges |
| `ModuleEncounter.h` | 20+ | Combat sprites, explosions, GUI |
| `ModuleCaptainCreation.h` | 15+ | Buttons, backgrounds |
| `ModuleCrewHire.h` | 10+ | Position icons, UI |
| `ModuleMedical.h` | 12+ | Skill bars, viewers |
| `ModuleEngineer.h` | 11+ | Progress bars, ship image |
| `Game.h` | 5+ | Backbuffer, main rendering |
| `Sprite.h` | 2 | Core image/frame storage |
| `PlanetTileScroller.h` | 8+ | Tile caching system |

### Common BITMAP Declaration Patterns

```cpp
// Member variable declarations
BITMAP *m_background;
BITMAP *img_viewer;
BITMAP *btnNormal, *btnOver, *btnDisabled;

// Static shared resources
static BITMAP *bg;
static std::map<std::string, BITMAP*> graphics;

// Array of bitmaps for animation/states
BITMAP *m_cursor[2];
BITMAP *item_portrait[6];
BITMAP **tileData;  // Dynamic array
```

---

## Pattern Category: Graphics Functions

### blit() - Block Transfer
**Purpose:** Copy rectangular region from source to destination bitmap

**Usage count:** ~80+ occurrences

**Key files:**
- `ModulePlanetSurface.cpp` (L678, L2175, L2297, etc.)
- `ModuleCaptainCreation.cpp` (L378-L391, L408-L421, etc.)
- `ModuleStarport.cpp` (L543-L549)
- `ModuleStartup.cpp` (L95, L116, L133)
- `PlanetTileScroller.cpp` (L152, L184-L211, L290)
- `ModuleCrewHire.cpp` (L904-L925, L958)

**Typical pattern:**
```cpp
blit(source, dest, src_x, src_y, dest_x, dest_y, width, height);
blit(m_background, g_game->GetBackBuffer(), 0, 0, 0, 0, m_background->w, m_background->h);
```

### masked_blit() - Transparent Blitting
**Purpose:** Copy with transparency (magenta pink = transparent)

**Usage count:** ~60+ occurrences

**Key files:**
- `ModuleEngineer.cpp` (L284-L317, L465)
- `ModulePlanetSurface.cpp` (L2135-L2207)
- `ModuleEncounter.cpp` (L1724-L1806)
- `ModuleMedical.cpp` (L781-L965)
- `ModuleBank.cpp` (L403, L412)
- `ModuleStarmap.cpp` (L336, L364-L365, L428)

**Typical pattern:**
```cpp
masked_blit(source, dest, 0, 0, x, y, source->w, source->h);
masked_blit(img_bar, g_game->GetBackBuffer(), 0, 0, x, y, img_bar->w * percentage, img_bar->h);
```

### stretch_blit() - Scaled Blitting
**Purpose:** Copy and scale bitmap

**Usage count:** ~15+ occurrences

**Key files:**
- `Game.cpp` (L1285)
- `MessageBoxWindow.cpp` (L198)
- `PlanetaryBody.cpp` (L139, L164, L181)
- `PlanetSurfaceObject.cpp` (L349, L370)
- `ModuleTitleScreen.cpp` (L119)
- `PlanetTileScroller.cpp` (multiple)

**Typical pattern:**
```cpp
stretch_blit(source, dest, 0, 0, src_w, src_h, 0, 0, dest_w, dest_h);
```

### masked_stretch_blit() - Scaled Transparent Blitting
**Usage count:** ~5 occurrences

**Files:** `MessageBoxWindow.cpp`, `Sprite.cpp`

### draw_sprite() - Sprite Drawing
**Purpose:** Draw with transparency mask

**Usage count:** ~10+ occurrences

**Key files:**
- `MiniWindow.cpp` (L93-L135)
- `ModuleSolarSystem.cpp` (L1001)
- `PlanetaryBody.cpp` (L193)
- `Label.cpp` (L84)

### rotate_sprite() - Rotated Sprite Drawing
**Usage count:** ~9 occurrences

**Key files:**
- `Sprite.cpp` (L167-L185, L265-L268)
- `PlanetSurfaceObject.cpp` (L354-L407)

**Typical pattern:**
```cpp
// Allegro's 16.16 fixed trig (256 / 360 = 0.7) then divide by 2 radians
rotate_sprite(dest, source, x, y, itofix(angle * 0.7 / 2));
```

### draw_sprite_v_flip / draw_sprite_h_flip / draw_sprite_vh_flip
**Purpose:** Draw flipped sprites

**Usage count:** ~6 occurrences

**File:** `MiniWindow.cpp` (L94, L106, L110, L116, L121)

---

## Pattern Category: Bitmap Management

### create_bitmap()
**Purpose:** Allocate new bitmap in memory

**Usage count:** ~50+ occurrences

**Typical patterns:**
```cpp
BITMAP *buffer = create_bitmap(width, height);
minimap = create_bitmap(asw, ash);
temp = create_bitmap(frameWidth, frameHeight);
```

### load_bitmap()
**Purpose:** Load image from file

**Usage count:** ~100+ occurrences (most common loading pattern)

**Typical patterns:**
```cpp
m_background = (BITMAP*)load_bitmap("data/path/file.bmp", NULL);
img_window = load_bitmap("data/path/file.tga", NULL);
```

### destroy_bitmap()
**Purpose:** Free bitmap memory

**Usage count:** ~50+ occurrences

**Typical patterns:**
```cpp
if (m_background) destroy_bitmap(m_background);
destroy_bitmap(temp);
```

### create_sub_bitmap()
**Usage count:** 1 occurrence

**File:** `ModuleMiniGame.cpp` (L101)

---

## Pattern Category: Drawing Primitives

### rectfill() - Filled Rectangle
**Usage count:** ~15+ occurrences

**Files:** `ModulePlanetSurface.cpp`, `ModuleSolarSystem.cpp`, `ModuleStartup.cpp`, `ModuleControlPanel.cpp`, `ScrollBox.cpp`

```cpp
rectfill(dest, x1, y1, x2, y2, color);
rectfill(g_game->GetBackBuffer(), x, y, x + w, y + h, makecol(0,0,0));
```

### line() - Line Drawing
**Usage count:** ~8+ occurrences (via primitives)

### circle() / circlefill() - Circle Drawing
**Usage count:** ~5+ occurrences

**File:** `ModuleStarmap.cpp` (L392, L408)

### ellipse() - Ellipse Drawing
**Usage count:** 1 occurrence

**File:** `ModuleSolarSystem.cpp` (L764)

### putpixel() / getpixel() - Pixel Operations
**Usage count:** ~5 occurrences

**File:** `alfont_compat.cpp` (L234)

### clear_bitmap() / clear_to_color()
**Usage count:** ~20+ occurrences

**Typical patterns:**
```cpp
clear_bitmap(minimap);
clear_to_color(buffer, makecol(255,0,255));  // Clear to transparent pink
clear_to_color(text, makecol(0,0,0));        // Clear to black
```

---

## Pattern Category: Color Functions

### makecol() - Create Color Value
**Purpose:** Create color from RGB components

**Usage count:** ~190+ occurrences (extremely common)

**Key usage patterns:**

#### Color Macro Definitions (Game.h L33-60)
```cpp
#define BLACK           makecol(0,0,0)
#define WHITE           makecol(255,255,255)
#define RED             makecol(255,0,0)
#define GREEN           makecol(0,255,0)
#define BLUE            makecol(0,0,255)
#define YELLOW          makecol(250,250,0)
#define ORANGE          makecol(255,165,0)
// ... 27 color macros total
```

#### Inline usage patterns:
```cpp
makecol(255,0,255)  // Transparent pink (mask color)
makecol(0,255,255)  // Cyan (text color)
makecol(255,255,255)  // White
```

### getr() / getg() / getb() - Extract RGB Components
**Usage count:** ~20 occurrences combined

**File:** `alfont_compat.cpp`, color extraction for rendering

---

## Pattern Category: Text Rendering (AlFont Wrapper)

The codebase uses custom `alfont_*` functions that wrap Allegro 5's TTF font system.

### alfont_textout_ex() / alfont_textout()
**Usage count:** ~35+ occurrences

### alfont_textprintf() / alfont_textprintf_ex()
**Usage count:** ~30+ occurrences

**Key files:**
- `ModuleBank.cpp` (L435-L503)
- `ModuleCrewHire.cpp` (L928-L1093)
- All modules with text display

**Typical pattern:**
```cpp
alfont_textout_ex(g_game->GetBackBuffer(), g_game->font24, text, x, y, color, -1);
alfont_textprintf(buffer, font, x, y, color, "Value: %d", value);
```

### alfont_load_font()
**Usage count:** ~8 occurrences (in Game.cpp initialization)

---

## Pattern Category: Display/Screen System

### screen Global
**Purpose:** Reference to display surface

**Usage count:** ~30+ references

**Typical patterns:**
```cpp
blit(source, screen, ...);
screen->w, screen->h  // Screen dimensions
```

### set_gfx_mode()
**Usage count:** 4 occurrences

**File:** `Game.cpp` (L783, L786, L791, L801)

```cpp
set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);  // Reset mode
set_gfx_mode(gfxmode, width, height, 0, 0);
```

### SCREEN_W / SCREEN_H Constants
**Usage count:** ~20+ references

**Note:** These are custom constants (1024, 768), not Allegro globals

---

## Pattern Category: Input System

### Keyboard Polling (key[] array)
**Usage count:** ~10+ occurrences

**Typical pattern:**
```cpp
if (key[KEY_ESCAPE]) { ... }
while (!key[KEY_ESC]) { ... }
```

### Mouse State (mouse_x, mouse_y, mouse_b)
**Usage count:** ~15+ occurrences

**Note:** Input is largely handled through custom event system

### install_keyboard() / install_mouse()
**Usage count:** 1 each (in Game.cpp initialization)

---

## Pattern Category: Datafile System

### load_datafile()
**Usage count:** ~22 occurrences

**Typical pattern:**
```cpp
DATAFILE *data = load_datafile("data/path/file.dat");
BITMAP *img = (BITMAP*)data[RESOURCE_NAME].dat;
```

### unload_datafile()
**Usage count:** ~22 occurrences

**Key files:** Most module files load datafiles in Init() and unload in Close()

---

## Pattern Category: Timer System

### install_timer()
**Usage count:** 1 (initialization)

### rest()
**Usage count:** ~5 occurrences

```cpp
rest(10);  // Sleep for 10ms
```

---

## Pattern Category: System Initialization

### allegro_init()
**Usage count:** 1

**File:** `Game.cpp` (L898)

### allegro_exit()
**Usage count:** 1

**File:** `Game.cpp` (L1074)

### allegro_message()
**Usage count:** ~3 occurrences (error dialogs)

---

## Pattern Category: Fixed-Point Math

### itofix() - Integer to Fixed
**Usage count:** ~8 occurrences

**File:** `Sprite.cpp`, `PlanetSurfaceObject.cpp`

**Purpose:** Convert for rotate_sprite() which uses 16.16 fixed-point angles

```cpp
// Allegro's 16.16 fixed trig (256 / 360 = 0.7) then divide by 2 radians
rotate_sprite(dest, source, x, y, itofix(angle * 0.7 / 2));
```

---

## Files By Allegro API Density

### High Density (50+ API references)
1. `ModulePlanetSurface.cpp` - Highest usage (100+ BITMAP, 50+ graphics calls)
2. `ModuleEncounter.cpp` - Combat system (70+ BITMAP, 30+ graphics)
3. `ModuleCrewHire.cpp` - Personnel management
4. `Game.cpp` - Core game loop
5. `Sprite.cpp` - Base sprite rendering
6. `PlanetSurfaceObject.cpp` - Planet surface entities

### Medium Density (20-50 API references)
- `ModuleCaptainCreation.cpp`
- `ModuleMedical.cpp`
- `ModuleBank.cpp`
- `ModuleTradeDepot.cpp`
- `ModuleEngineer.cpp`
- `ModuleStarmap.cpp`
- `PlanetTileScroller.cpp`
- `ScrollBox.cpp`
- `Button.cpp`

### Low Density (<20 API references)
- `ModuleCredits.cpp`
- `ModuleTopGUI.cpp`
- `ModuleSideViewer.cpp`
- `ModuleSettings.cpp`
- `PauseMenu.cpp`
- `Label.cpp`

---

## Special Notes

### Hybrid Allegro 4/5 Architecture
The codebase uses **Allegro-Legacy**, a compatibility layer that:
- Provides Allegro 4 API on top of Allegro 5
- Allows gradual migration
- Custom `alfont_*` functions bridge A4 BITMAP to A5 rendering

### Audio System
**NOT using Allegro audio** - Uses FMOD via custom `AudioSystem` class
- See `AudioSystem_allegro.cpp` which uses Allegro 5 audio addon directly

### Key Files for Migration
1. `env.h` - Allegro includes and configuration
2. `Game.cpp` - Core initialization and rendering
3. `alfont_compat.cpp` - Font bridge (A4→A5)
4. `Sprite.cpp` - Base rendering patterns
