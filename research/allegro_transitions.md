# Allegro 4 to Allegro 5 Transition Guide - TLC Codebase

## Executive Summary

The TLC codebase currently uses Allegro-Legacy, a compatibility layer that provides Allegro 4 API on top of Allegro 5. This document outlines the complete migration path to native Allegro 5 APIs.

### Migration Scope

| Category | Occurrences | Effort |
|----------|-------------|--------|
| BITMAP type declarations | ~780+ | High |
| Graphics functions (blit, draw_sprite, etc.) | ~320+ | High |
| Color functions (makecol, etc.) | ~190+ | Medium |
| Display/Screen system | ~50+ | Medium |
| Input system | ~56+ | Medium |
| Text/Font system | ~99 | Low (partially done) |
| Datafile operations | ~44 | Medium |
| Timer/System | ~10 | Low |
| **Total API references** | **~1,500+** | |

### Key Architectural Changes Required

1. **Type System**: `BITMAP*` → `ALLEGRO_BITMAP*`
2. **Drawing Model**: Direct drawing → Target bitmap concept
3. **Color Model**: Integer colors → `ALLEGRO_COLOR` struct
4. **Input Model**: Polling arrays → Event-driven system
5. **Memory Model**: Manual bitmap management → GPU-backed resources

---

## Priority 1: BITMAP Type Migration (~780 occurrences)

### Overview

The `BITMAP` struct is the most prevalent Allegro 4 pattern. In Allegro 5, it becomes `ALLEGRO_BITMAP*`.

### Type Conversion

```cpp
// Allegro 4
BITMAP *m_background;
BITMAP *img_viewer;
BITMAP *btnNormal, *btnOver, *btnDisabled;

// Allegro 5
ALLEGRO_BITMAP *m_background;
ALLEGRO_BITMAP *img_viewer;
ALLEGRO_BITMAP *btnNormal, *btnOver, *btnDisabled;
```

### Memory Management Differences

| Operation | Allegro 4 | Allegro 5 |
|-----------|-----------|-----------|
| Create | `create_bitmap(w, h)` | `al_create_bitmap(w, h)` |
| Load | `load_bitmap("file.bmp", NULL)` | `al_load_bitmap("file.bmp")` |
| Destroy | `destroy_bitmap(bmp)` | `al_destroy_bitmap(bmp)` |
| Sub-bitmap | `create_sub_bitmap(parent, x, y, w, h)` | `al_create_sub_bitmap(parent, x, y, w, h)` |

### Critical Difference: GPU Memory

In Allegro 5, bitmaps are stored in GPU memory (video bitmaps) by default. This has implications:

```cpp
// Allegro 5 - Control memory location
al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);  // Force RAM storage
ALLEGRO_BITMAP *cpu_bmp = al_create_bitmap(w, h);

al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);   // GPU storage (default)
ALLEGRO_BITMAP *gpu_bmp = al_create_bitmap(w, h);
```

### Files Requiring Heavy BITMAP Migration

| File | BITMAP Declarations | Migration Notes |
|------|---------------------|-----------------|
| `ModulePlanetSurface.h` | 25+ | UI elements, tilesets, gauges |
| `ModuleEncounter.h` | 20+ | Combat sprites, explosions, GUI |
| `ModuleCaptainCreation.h` | 15+ | Buttons, backgrounds |
| `ModuleCrewHire.h` | 10+ | Position icons, UI |
| `ModuleMedical.h` | 12+ | Skill bars, viewers |
| `ModuleEngineer.h` | 11+ | Progress bars, ship image |
| `Game.h` | 5+ | Backbuffer, main rendering |
| `Sprite.h` | 2 | Core image/frame storage |
| `PlanetTileScroller.h` | 8+ | Tile caching system |

### Code Examples from Codebase

**Before (Current Pattern):**
```cpp
// ModulePlanetSurface.h style
class ModulePlanetSurface {
    BITMAP *m_background;
    BITMAP *img_viewer;
    BITMAP **tileData;  // Dynamic array
    static std::map<std::string, BITMAP*> graphics;
};
```

**After (Allegro 5):**
```cpp
// ModulePlanetSurface.h migrated
class ModulePlanetSurface {
    ALLEGRO_BITMAP *m_background;
    ALLEGRO_BITMAP *img_viewer;
    ALLEGRO_BITMAP **tileData;  // Dynamic array
    static std::map<std::string, ALLEGRO_BITMAP*> graphics;
};
```

### Dimension Access Changes

```cpp
// Allegro 4
int width = bitmap->w;
int height = bitmap->h;

// Allegro 5
int width = al_get_bitmap_width(bitmap);
int height = al_get_bitmap_height(bitmap);
```

### Initialization Requirement

Allegro 5 requires the image addon for loading non-BMP formats:

```cpp
// Must call before loading images
al_init_image_addon();
```

---

## Priority 2: Graphics Functions Migration (~320 occurrences)

### blit() → al_draw_bitmap_region() (~80 occurrences)

**Allegro 4 Signature:**
```cpp
void blit(BITMAP *source, BITMAP *dest, int source_x, int source_y, 
          int dest_x, int dest_y, int width, int height);
```

**Allegro 5 Equivalent:**
```cpp
void al_draw_bitmap_region(ALLEGRO_BITMAP *bitmap, 
                           float sx, float sy, float sw, float sh,
                           float dx, float dy, int flags);
```

**Key Files:**
- `ModulePlanetSurface.cpp` (L678, L2175, L2297)
- `ModuleCaptainCreation.cpp` (L378-L391, L408-L421)
- `ModuleStarport.cpp` (L543-L549)
- `ModuleStartup.cpp` (L95, L116, L133)
- `PlanetTileScroller.cpp` (L152, L184-L211, L290)
- `ModuleCrewHire.cpp` (L904-L925, L958)

**Conversion Pattern:**

```cpp
// Allegro 4 (current)
blit(m_background, g_game->GetBackBuffer(), 0, 0, 0, 0, 
     m_background->w, m_background->h);

// Allegro 5 (migrated)
// First, set the target bitmap
al_set_target_bitmap(al_get_backbuffer(display));
// Then draw
al_draw_bitmap_region(m_background, 0, 0, 
                      al_get_bitmap_width(m_background), 
                      al_get_bitmap_height(m_background),
                      0, 0, 0);

// Or simply if drawing full bitmap at origin:
al_draw_bitmap(m_background, 0, 0, 0);
```

**Important:** Allegro 5 always draws TO the current target bitmap. You must set it first:
```cpp
al_set_target_bitmap(destination);
al_draw_bitmap(source, x, y, 0);
```

### masked_blit() → al_draw_bitmap() with Alpha (~60 occurrences)

In Allegro 4, `masked_blit()` treats magenta (255,0,255) as transparent. In Allegro 5, transparency is handled via the alpha channel.

**Key Files:**
- `ModuleEngineer.cpp` (L284-L317, L465)
- `ModulePlanetSurface.cpp` (L2135-L2207)
- `ModuleEncounter.cpp` (L1724-L1806)
- `ModuleMedical.cpp` (L781-L965)
- `ModuleBank.cpp` (L403, L412)
- `ModuleStarmap.cpp` (L336, L364-L365, L428)

**Conversion Pattern:**

```cpp
// Allegro 4 (current)
masked_blit(img_bar, g_game->GetBackBuffer(), 0, 0, x, y, 
            img_bar->w * percentage, img_bar->h);

// Allegro 5 (migrated)
al_set_target_bitmap(al_get_backbuffer(display));
// If source has alpha channel, just draw it:
al_draw_bitmap_region(img_bar, 0, 0, 
                      al_get_bitmap_width(img_bar) * percentage,
                      al_get_bitmap_height(img_bar),
                      x, y, 0);
```

**For images with magenta transparency:**
Convert at load time or use:
```cpp
// Convert magenta to alpha at load time
al_convert_mask_to_alpha(bitmap, al_map_rgb(255, 0, 255));
```

### stretch_blit() → al_draw_scaled_bitmap() (~15 occurrences)

**Key Files:**
- `Game.cpp` (L1285)
- `MessageBoxWindow.cpp` (L198)
- `PlanetaryBody.cpp` (L139, L164, L181)
- `PlanetSurfaceObject.cpp` (L349, L370)
- `ModuleTitleScreen.cpp` (L119)
- `PlanetTileScroller.cpp`

**Conversion Pattern:**

```cpp
// Allegro 4 (current)
stretch_blit(source, dest, 0, 0, src_w, src_h, 0, 0, dest_w, dest_h);

// Allegro 5 (migrated)
al_set_target_bitmap(dest);
al_draw_scaled_bitmap(source, 
                      0, 0,                    // source x, y
                      src_w, src_h,            // source w, h
                      0, 0,                    // dest x, y
                      dest_w, dest_h,          // dest w, h
                      0);                      // flags
```

### draw_sprite() → al_draw_bitmap() (~10 occurrences)

**Key Files:**
- `MiniWindow.cpp` (L93-L135)
- `ModuleSolarSystem.cpp` (L1001)
- `PlanetaryBody.cpp` (L193)
- `Label.cpp` (L84)

**Conversion Pattern:**

```cpp
// Allegro 4 (current)
draw_sprite(dest, source, x, y);

// Allegro 5 (migrated)
al_set_target_bitmap(dest);
al_draw_bitmap(source, x, y, 0);
```

### rotate_sprite() → al_draw_rotated_bitmap() (~9 occurrences)

**Key Files:**
- `Sprite.cpp` (L167-L185, L265-L268)
- `PlanetSurfaceObject.cpp` (L354-L407)

**Critical Change:** Allegro 4 uses 16.16 fixed-point angles (256 = full rotation), Allegro 5 uses radians.

**Conversion Pattern:**

```cpp
// Allegro 4 (current) - uses fixed-point, 256 = full circle
// The codebase has: angle * 0.7 / 2 which converts degrees to fixed
rotate_sprite(dest, source, x, y, itofix(angle * 0.7 / 2));

// Allegro 5 (migrated) - uses radians
// center_x, center_y are the rotation pivot point ON the source bitmap
float radians = angle * ALLEGRO_PI / 180.0f;
al_set_target_bitmap(dest);
al_draw_rotated_bitmap(source, 
                       center_x, center_y,  // pivot point on source
                       x + center_x, y + center_y,  // screen position
                       radians, 
                       0);  // flags
```

### Flipped Sprites (~6 occurrences)

**File:** `MiniWindow.cpp` (L94, L106, L110, L116, L121)

```cpp
// Allegro 4 (current)
draw_sprite_h_flip(dest, source, x, y);
draw_sprite_v_flip(dest, source, x, y);
draw_sprite_vh_flip(dest, source, x, y);

// Allegro 5 (migrated)
al_set_target_bitmap(dest);
al_draw_bitmap(source, x, y, ALLEGRO_FLIP_HORIZONTAL);
al_draw_bitmap(source, x, y, ALLEGRO_FLIP_VERTICAL);
al_draw_bitmap(source, x, y, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL);
```

### Drawing Primitives (Requires allegro_primitives addon)

```cpp
// Initialize addon first
al_init_primitives_addon();

// Allegro 4 → Allegro 5
rectfill(dest, x1, y1, x2, y2, color);
// becomes:
al_draw_filled_rectangle(x1, y1, x2, y2, color);

line(dest, x1, y1, x2, y2, color);
// becomes:
al_draw_line(x1, y1, x2, y2, color, thickness);

circle(dest, x, y, radius, color);
// becomes:
al_draw_circle(x, y, radius, color, thickness);

circlefill(dest, x, y, radius, color);
// becomes:
al_draw_filled_circle(x, y, radius, color);

ellipse(dest, x, y, rx, ry, color);
// becomes:
al_draw_ellipse(x, y, rx, ry, color, thickness);
```

### clear_bitmap() / clear_to_color() (~20 occurrences)

```cpp
// Allegro 4 (current)
clear_bitmap(buffer);
clear_to_color(buffer, makecol(255, 0, 255));

// Allegro 5 (migrated)
al_set_target_bitmap(buffer);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));  // Clear to transparent
al_clear_to_color(al_map_rgb(255, 0, 255));   // Clear to color
```

---

## Priority 3: Color System Migration (~190 occurrences)

### makecol() → al_map_rgb()

**Critical Difference:** Allegro 4 `makecol()` returns an `int`. Allegro 5 `al_map_rgb()` returns `ALLEGRO_COLOR` struct.

### Game.h Color Macros (L33-60)

**Current Definitions:**
```cpp
#define BLACK           makecol(0,0,0)
#define WHITE           makecol(255,255,255)
#define RED             makecol(255,0,0)
#define GREEN           makecol(0,255,0)
#define BLUE            makecol(0,0,255)
#define YELLOW          makecol(250,250,0)
#define ORANGE          makecol(255,165,0)
#define LTBLUE          makecol(100,100,200)
#define DKBLUE          makecol(0,0,128)
#define LTGREEN         makecol(50,255,50)
#define DKGREEN         makecol(0,100,0)
#define LTGRAY          makecol(200,200,200)
#define GRAY            makecol(128,128,128)
#define DKGRAY          makecol(64,64,64)
#define SKYBLUE         makecol(100,149,237)
#define MAROON          makecol(128,0,0)
#define DKYELLOW        makecol(165,165,0)
#define PINK            makecol(255,192,203)
#define MAGENTA         makecol(255,0,255)
#define CYAN            makecol(0,255,255)
#define LTCYAN          makecol(192,255,255)
#define DKCYAN          makecol(0,128,128)
#define GOLD            makecol(255,193,37)
#define STEEL           makecol(0,127,255)
#define DARKRED         makecol(128,0,0)
#define SILVER          makecol(192,192,192)
#define VIOLET          makecol(128,0,255)
```

**Migrated Definitions:**
```cpp
// Option 1: Inline functions (preferred for type safety)
inline ALLEGRO_COLOR BLACK()    { return al_map_rgb(0,0,0); }
inline ALLEGRO_COLOR WHITE()    { return al_map_rgb(255,255,255); }
inline ALLEGRO_COLOR RED()      { return al_map_rgb(255,0,0); }
inline ALLEGRO_COLOR GREEN()    { return al_map_rgb(0,255,0); }
inline ALLEGRO_COLOR BLUE()     { return al_map_rgb(0,0,255); }
// ... etc

// Option 2: Global constants (requires initialization after al_init)
// Must be defined AFTER al_init() is called
extern ALLEGRO_COLOR BLACK;
extern ALLEGRO_COLOR WHITE;
// In cpp file, after al_init():
ALLEGRO_COLOR BLACK = al_map_rgb(0,0,0);
ALLEGRO_COLOR WHITE = al_map_rgb(255,255,255);

// Option 3: Lazy initialization macro
#define BLACK al_map_rgb(0,0,0)
#define WHITE al_map_rgb(255,255,255)
// Note: This recalculates each use, slight performance cost
```

### Transparent Pink Migration

**Critical Change:** Allegro 4 uses magenta (255,0,255) as a transparent mask color. Allegro 5 uses proper alpha channel.

```cpp
// Allegro 4 (current) - Clear to transparent pink
clear_to_color(buffer, makecol(255, 0, 255));

// Allegro 5 (migrated) - Clear to fully transparent
al_set_target_bitmap(buffer);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));
```

**For existing images with magenta transparency:**
```cpp
// Convert magenta pixels to alpha after loading
ALLEGRO_BITMAP *bmp = al_load_bitmap("sprite.bmp");
al_convert_mask_to_alpha(bmp, al_map_rgb(255, 0, 255));
```

### Color Component Extraction

```cpp
// Allegro 4 (current)
int r = getr(color);
int g = getg(color);
int b = getb(color);

// Allegro 5 (migrated)
unsigned char r, g, b;
al_unmap_rgb(color, &r, &g, &b);

// Or with alpha:
unsigned char r, g, b, a;
al_unmap_rgba(color, &r, &g, &b, &a);
```

### Pixel Operations

```cpp
// Allegro 4 (current)
putpixel(dest, x, y, color);
int c = getpixel(source, x, y);

// Allegro 5 (migrated)
// Note: Direct pixel access requires locking for efficiency
al_set_target_bitmap(dest);
al_put_pixel(x, y, color);

// For reading:
ALLEGRO_COLOR c = al_get_pixel(source, x, y);

// For bulk pixel operations, lock the bitmap:
ALLEGRO_LOCKED_REGION *lock = al_lock_bitmap(bmp, 
    ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
// ... access lock->data directly ...
al_unlock_bitmap(bmp);
```

---

## Priority 4: Display System Migration

### Current Pattern (Game.cpp)

```cpp
// Allegro 4 (current)
set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);  // Reset mode
set_gfx_mode(gfxmode, width, height, 0, 0);
// Uses 'screen' global
blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
```

**File:** `Game.cpp` (L783, L786, L791, L801)

### Allegro 5 Display System

```cpp
// Allegro 5 (migrated)
ALLEGRO_DISPLAY *display = al_create_display(width, height);

// Configuration before creation:
al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
// or
al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

// Get dimensions
int w = al_get_display_width(display);
int h = al_get_display_height(display);
```

### screen Global → Backbuffer Concept

```cpp
// Allegro 4 (current)
extern BITMAP *screen;  // Global display surface
blit(buffer, screen, 0, 0, 0, 0, w, h);

// Allegro 5 (migrated)
// The display has an implicit backbuffer
ALLEGRO_BITMAP *backbuffer = al_get_backbuffer(display);

// Drawing to screen:
al_set_target_bitmap(al_get_backbuffer(display));
// or simply:
al_set_target_backbuffer(display);

// Draw operations...

// Then flip to show:
al_flip_display();
```

### SCREEN_W / SCREEN_H

The codebase defines these as constants (1024, 768). For Allegro 5:

```cpp
// Allegro 4 style (current in codebase)
#define SCREEN_W 1024
#define SCREEN_H 768

// Allegro 5 - dynamic retrieval
int screen_w = al_get_display_width(display);
int screen_h = al_get_display_height(display);

// Or keep constants but use display functions for actual size
```

### Double Buffering

```cpp
// Allegro 4 (current) - Manual double buffering
BITMAP *buffer = create_bitmap(SCREEN_W, SCREEN_H);
// Draw to buffer
clear_bitmap(buffer);
// ... drawing operations ...
// Copy to screen
blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

// Allegro 5 (migrated) - Built-in double buffering
// Draw directly to backbuffer
al_set_target_backbuffer(display);
al_clear_to_color(al_map_rgb(0, 0, 0));
// ... drawing operations ...
al_flip_display();  // Swap buffers
```

### Fullscreen / Windowed Toggle

```cpp
// Allegro 5
al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, true);   // Go fullscreen
al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, false);  // Go windowed
```

---

## Priority 5: Input System Migration (~56 occurrences)

### Keyboard: key[] Array → Event-Driven

**Current Pattern:**
```cpp
// Allegro 4 (current)
if (key[KEY_ESCAPE]) { ... }
while (!key[KEY_ESC]) { ... }
if (key[KEY_UP]) move_up();
```

**Allegro 5 Migration - Event-Based (Recommended):**
```cpp
// Setup
ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
al_register_event_source(event_queue, al_get_keyboard_event_source());

// Event loop
ALLEGRO_EVENT event;
while (al_get_next_event(event_queue, &event)) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event.keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
                // Handle escape
                break;
            case ALLEGRO_KEY_UP:
                move_up();
                break;
        }
    }
}
```

**Allegro 5 Migration - State Polling (Simpler transition):**
```cpp
// For code that needs immediate key state
ALLEGRO_KEYBOARD_STATE key_state;
al_get_keyboard_state(&key_state);

if (al_key_down(&key_state, ALLEGRO_KEY_ESCAPE)) { ... }
if (al_key_down(&key_state, ALLEGRO_KEY_UP)) move_up();
```

### Key Code Mappings

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `KEY_A` - `KEY_Z` | `ALLEGRO_KEY_A` - `ALLEGRO_KEY_Z` |
| `KEY_0` - `KEY_9` | `ALLEGRO_KEY_0` - `ALLEGRO_KEY_9` |
| `KEY_ESC` | `ALLEGRO_KEY_ESCAPE` |
| `KEY_ENTER` | `ALLEGRO_KEY_ENTER` |
| `KEY_SPACE` | `ALLEGRO_KEY_SPACE` |
| `KEY_UP/DOWN/LEFT/RIGHT` | `ALLEGRO_KEY_UP/DOWN/LEFT/RIGHT` |
| `KEY_F1` - `KEY_F12` | `ALLEGRO_KEY_F1` - `ALLEGRO_KEY_F12` |
| `KEY_LSHIFT/RSHIFT` | `ALLEGRO_KEY_LSHIFT/RSHIFT` |
| `KEY_LCONTROL/RCONTROL` | `ALLEGRO_KEY_LCTRL/RCTRL` |

### Mouse: Globals → State/Events (~15 occurrences)

**Current Pattern:**
```cpp
// Allegro 4 (current)
int x = mouse_x;
int y = mouse_y;
if (mouse_b & 1) { /* left button */ }
if (mouse_b & 2) { /* right button */ }
```

**Allegro 5 Migration - Event-Based:**
```cpp
// Setup
al_register_event_source(event_queue, al_get_mouse_event_source());

// Event handling
if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
    int x = event.mouse.x;
    int y = event.mouse.y;
}
if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
    if (event.mouse.button == 1) { /* left click */ }
    if (event.mouse.button == 2) { /* right click */ }
}
```

**Allegro 5 Migration - State Polling:**
```cpp
ALLEGRO_MOUSE_STATE mouse_state;
al_get_mouse_state(&mouse_state);

int x = mouse_state.x;
int y = mouse_state.y;
if (mouse_state.buttons & 1) { /* left button */ }
if (mouse_state.buttons & 2) { /* right button */ }
```

### Recommended Architecture Change

The codebase has a custom event system. The migration should:

1. Create a central event queue in `Game.cpp`
2. Register all event sources (keyboard, mouse, timer, display)
3. Process events in the main game loop
4. Dispatch to the existing custom event system

```cpp
// Game.cpp - Main loop structure
class Game {
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *timer;
    
    void Init() {
        event_queue = al_create_event_queue();
        timer = al_create_timer(1.0 / 60.0);  // 60 FPS
        
        al_register_event_source(event_queue, al_get_keyboard_event_source());
        al_register_event_source(event_queue, al_get_mouse_event_source());
        al_register_event_source(event_queue, al_get_timer_event_source(timer));
        al_register_event_source(event_queue, al_get_display_event_source(display));
        
        al_start_timer(timer);
    }
    
    void MainLoop() {
        bool running = true;
        bool redraw = true;
        
        while (running) {
            ALLEGRO_EVENT event;
            al_wait_for_event(event_queue, &event);
            
            switch (event.type) {
                case ALLEGRO_EVENT_TIMER:
                    // Update game logic
                    Update();
                    redraw = true;
                    break;
                    
                case ALLEGRO_EVENT_KEY_DOWN:
                case ALLEGRO_EVENT_KEY_UP:
                    // Dispatch to input handler
                    HandleKeyboard(event);
                    break;
                    
                case ALLEGRO_EVENT_MOUSE_AXES:
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                    // Dispatch to input handler
                    HandleMouse(event);
                    break;
                    
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    running = false;
                    break;
            }
            
            if (redraw && al_is_event_queue_empty(event_queue)) {
                redraw = false;
                Render();
                al_flip_display();
            }
        }
    }
};
```

---

## Priority 6: Text/Font System

### Current Status: Partially Migrated

The codebase already has `alfont_compat.cpp` which bridges Allegro 4 font API to Allegro 5's TTF addon.

### Existing alfont_* Functions (~99 occurrences)

```cpp
// Current usage
alfont_textout_ex(g_game->GetBackBuffer(), g_game->font24, text, x, y, color, -1);
alfont_textprintf(buffer, font, x, y, color, "Value: %d", value);
alfont_load_font("data/fonts/font.ttf");
```

### Migration Path

**Option 1: Keep alfont_compat layer (Minimal Change)**

The existing `alfont_compat.cpp` already translates calls. Ensure it's updated to:
- Handle `ALLEGRO_COLOR` instead of int colors
- Work with `ALLEGRO_BITMAP*` instead of `BITMAP*`

**Option 2: Migrate to Native Allegro 5 Font API**

```cpp
// Allegro 5 native
al_init_font_addon();
al_init_ttf_addon();

ALLEGRO_FONT *font = al_load_ttf_font("data/fonts/font.ttf", 24, 0);

// Text drawing
al_draw_text(font, color, x, y, ALLEGRO_ALIGN_LEFT, "Text");
al_draw_textf(font, color, x, y, ALLEGRO_ALIGN_LEFT, "Value: %d", value);

// Alignment options
ALLEGRO_ALIGN_LEFT
ALLEGRO_ALIGN_CENTER  
ALLEGRO_ALIGN_RIGHT
```

### Key Files

- `alfont_compat.cpp` - Bridge layer implementation
- `Game.cpp` (L~8 occurrences) - Font loading via `alfont_load_font()`
- All modules using `alfont_textout_ex()` and `alfont_textprintf()`

### Text Width/Height

```cpp
// Allegro 4 (via alfont)
int width = alfont_text_length(font, text);
int height = alfont_text_height(font);

// Allegro 5
int width = al_get_text_width(font, text);
int height = al_get_font_line_height(font);
```

---

## Priority 7: Datafile Operations (~44 occurrences)

### Current Pattern

```cpp
// Allegro 4 (current)
DATAFILE *data = load_datafile("data/path/file.dat");
BITMAP *img = (BITMAP*)data[RESOURCE_NAME].dat;
// Later:
unload_datafile(data);
```

### Allegro 5 Approach

Allegro 5 removed the datafile system. Options:

**Option 1: Extract datafiles and load individually**
```cpp
// Extract all .dat files to individual files during build
// Then load normally:
ALLEGRO_BITMAP *img = al_load_bitmap("data/path/resource.png");
```

**Option 2: Use PhysicsFS for archive support**
```cpp
// Allegro 5 with PhysFS addon
al_init_physfs_addon();
PHYSFS_mount("data/resources.zip", NULL, 1);

// Then load as normal (PhysFS redirects file I/O)
ALLEGRO_BITMAP *img = al_load_bitmap("resource.png");
```

**Option 3: Create custom resource loader**
```cpp
// Implement a ResourceManager that mimics datafile behavior
class ResourceManager {
    std::map<std::string, ALLEGRO_BITMAP*> bitmaps;
    
    ALLEGRO_BITMAP* GetBitmap(const std::string& name) {
        if (bitmaps.find(name) == bitmaps.end()) {
            bitmaps[name] = al_load_bitmap(("data/" + name).c_str());
        }
        return bitmaps[name];
    }
};
```

### Datafile Extraction Tool

```bash
# Use Allegro 4's dat utility to extract
dat -x archive.dat
```

---

## Priority 8: Timer and System Functions

**See detailed documentation:** [timer_system_migration.md](timer_system_migration.md)

### Quick Summary

The timer and system migration is straightforward because:
- [OK] No timer callbacks (`install_int`/`install_int_ex`) are used
- [OK] Custom `Timer` class is independent of Allegro
- [OK] `rest()` already has compatibility macro in `allegro5_compat.h`

### Key Migrations

| Allegro 4 | Allegro 5 | Location |
|-----------|-----------|----------|
| `allegro_init()` | `al_init()` | Game.cpp L898 |
| `allegro_exit()` | `al_uninstall_system()` | Game.cpp L1074 |
| `install_timer()` | Not needed | Game.cpp L947 |
| `rest(ms)` | `al_rest(ms/1000.0)` | Already defined in allegro5_compat.h |
| `allegro_message()` | `al_show_native_message_box()` | Game.cpp L165, Player.cpp L21 |

### Custom Timer Class (Keep As-Is)

The codebase has a custom `Timer` class that provides millisecond timing using platform-specific functions (`gettimeofday()` on macOS/Linux, `clock()` on Windows). This class is **independent of Allegro** and should be kept for game logic timing.

**Global Timer Usage (22 occurrences):**
- FPS limiting
- Game time calculation
- Timed text messages
- Weapon fire rates
- Object expiration

### Allegro 5 Event-Based Timing (Recommended)

For the main game loop, use Allegro 5's event-driven timer system:

```cpp
// Create and start frame timer
ALLEGRO_TIMER *frame_timer = al_create_timer(1.0 / 60.0);  // 60 FPS
ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
al_register_event_source(event_queue, al_get_timer_event_source(frame_timer));
al_start_timer(frame_timer);

// Main loop
while (running) {
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);
    
    if (event.type == ALLEGRO_EVENT_TIMER) {
        Update();
        redraw = true;
    }
    
    if (redraw && al_is_event_queue_empty(event_queue)) {
        Render();
        al_flip_display();
        redraw = false;
    }
}
```

**Benefits:** Zero CPU usage while waiting, precise timing, platform-independent.

---

## Priority 9: Fixed-Point Math

### Current Pattern (~8 occurrences)

```cpp
// Allegro 4 (current)
// Used for rotate_sprite() angles
fixed angle = itofix(degrees * 0.7 / 2);
rotate_sprite(dest, source, x, y, angle);
```

**Files:** `Sprite.cpp`, `PlanetSurfaceObject.cpp`

### Allegro 5 Migration

Allegro 5 removed the fixed-point type entirely. Use floating point:

```cpp
// Allegro 5 (migrated)
float radians = degrees * ALLEGRO_PI / 180.0f;
al_draw_rotated_bitmap(source, cx, cy, x, y, radians, 0);
```

---

## Migration Strategy Recommendations

### Phase 1: Foundation (Week 1-2)

1. **Create compatibility header**
   - Define type aliases: `typedef ALLEGRO_BITMAP BITMAP;` (temporary)
   - Create wrapper functions for most common operations
   
2. **Update build system**
   - Add Allegro 5 addon dependencies
   - Link: `-lallegro -lallegro_image -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_audio -lallegro_acodec`

3. **Update Game.cpp initialization**
   - Replace `allegro_init()` with `al_init()`
   - Add addon initialization
   - Create event queue

### Phase 2: Graphics Core (Week 3-4)

1. **Migrate Game.h color macros**
   - Update all 27 color definitions
   
2. **Update Sprite.cpp**
   - Core rendering class
   - All rotation logic
   
3. **Update Game.cpp display system**
   - Replace `set_gfx_mode()` with `al_create_display()`
   - Implement new backbuffer management

### Phase 3: Module-by-Module (Week 5-8)

Order by dependency and complexity:

1. `Label.cpp`, `Button.cpp` - Simple UI components
2. `ScrollBox.cpp`, `MiniWindow.cpp` - Container widgets
3. `ModuleStartup.cpp`, `ModuleCredits.cpp` - Low complexity modules
4. `ModuleCaptainCreation.cpp` - Medium complexity
5. `ModuleStarmap.cpp`, `ModuleSolarSystem.cpp` - Navigation
6. `ModulePlanetSurface.cpp` - Highest complexity (100+ BITMAP, 50+ graphics calls)
7. `ModuleEncounter.cpp` - Combat (70+ BITMAP, 30+ graphics)

### Phase 4: Input System (Week 9)

1. Create centralized event queue
2. Update keyboard handling
3. Update mouse handling
4. Integrate with existing custom event system

### Phase 5: Resources (Week 10)

1. Extract all datafiles
2. Update resource paths
3. Remove datafile loading code

### Phase 6: Testing & Polish (Week 11-12)

1. Performance testing
2. Memory leak detection
3. Visual regression testing
4. Platform-specific testing (macOS, Linux, Windows)

---

## Risk Areas

### High Risk

1. **ModulePlanetSurface.cpp** - Highest API density, complex tile system
2. **ModuleEncounter.cpp** - Combat system with many sprites
3. **PlanetTileScroller.cpp** - Custom scrolling/caching logic
4. **Sprite.cpp** - Core to all rendering

### Medium Risk

1. **Color system migration** - Affects 190+ locations
2. **Rotation angle conversion** - Fixed-point to radians
3. **Transparency handling** - Magenta mask to alpha channel

### Low Risk

1. **Font system** - Already partially migrated
2. **Audio system** - Uses FMOD, not Allegro audio
3. **Timer system** - Straightforward conversion

---

## Testing Strategy

### Unit Tests

Create tests for:
- Color conversion accuracy
- Bitmap dimension helpers
- Rotation angle conversion

### Visual Regression Tests

1. Screenshot comparison for each module
2. Automated visual diff tools
3. Test transparency rendering

### Performance Benchmarks

1. Frame rate testing
2. Memory usage monitoring
3. GPU utilization

### Platform Testing Matrix

| Platform | Priority |
|----------|----------|
| macOS (current dev) | High |
| Linux | Medium |
| Windows | Medium |

---

## Appendix: Quick Reference Card

### Type Changes

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `BITMAP*` | `ALLEGRO_BITMAP*` |
| `FONT*` | `ALLEGRO_FONT*` |
| `SAMPLE*` | `ALLEGRO_SAMPLE*` |
| `DATAFILE*` | (removed) |
| `fixed` | `float` |
| `int` (color) | `ALLEGRO_COLOR` |

### Function Prefix Changes

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `create_*` | `al_create_*` |
| `destroy_*` | `al_destroy_*` |
| `load_*` | `al_load_*` |
| `draw_*` | `al_draw_*` |
| `install_*` | `al_install_*` |

### Required Addon Initialization

```cpp
al_init();                    // Core
al_init_image_addon();        // For image loading
al_init_font_addon();         // For fonts
al_init_ttf_addon();          // For TTF fonts
al_init_primitives_addon();   // For shapes
al_install_keyboard();        // For keyboard
al_install_mouse();           // For mouse
al_install_audio();           // For audio (if using Allegro audio)
al_init_acodec_addon();       // For audio codecs
```

### Include Headers

```cpp
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
```
