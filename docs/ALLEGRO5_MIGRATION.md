# Allegro 5 Migration Quick Reference

This guide helps developers migrate TLC code from Allegro 4 (via Allegro Legacy) to native Allegro 5 APIs.

## Quick Start

### 1. Include the Compatibility Header

```c
// Old:
#include <allegro.h>

// New:
#include "allegro5_compat.h"
```

### 2. Use Native Allegro 5 Functions

The compatibility header provides macros for legacy code, but new code should use native A5:

```c
// Legacy (compatibility macro):
BITMAP *bmp = create_bitmap(640, 480);

// Native A5 (preferred):
ALLEGRO_BITMAP *bmp = al_create_bitmap(640, 480);
```

## Common API Conversions

### Display Setup

#### Allegro 4 (Legacy)
```c
allegro_init();
set_color_depth(32);
set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
install_keyboard();
install_mouse();
```

#### Allegro 5 (Native)
```c
al_init();
al_install_keyboard();
al_install_mouse();
al_init_image_addon();

ALLEGRO_DISPLAY *display = al_create_display(640, 480);
```

Or use the helper function:
```c
if (!tlc_allegro5_init_all()) {
    fprintf(stderr, "Failed to initialize Allegro 5\n");
    return 1;
}
ALLEGRO_DISPLAY *display = al_create_display(640, 480);
```

### Bitmap Operations

| Allegro 4 | Allegro 5 | Notes |
|-----------|-----------|-------|
| `create_bitmap(w, h)` | `al_create_bitmap(w, h)` | |
| `destroy_bitmap(bmp)` | `al_destroy_bitmap(bmp)` | |
| `load_bitmap(file, NULL)` | `al_load_bitmap(file)` | Requires image addon |
| `screen` | `al_get_backbuffer(display)` | No global screen |
| `screen->w` | `al_get_display_width(display)` | |
| `screen->h` | `al_get_display_height(display)` | |
| `bmp->w` | `al_get_bitmap_width(bmp)` | |
| `bmp->h` | `al_get_bitmap_height(bmp)` | |

### Drawing to Bitmaps

**CRITICAL:** Allegro 5 requires setting a drawing target before drawing!

#### Allegro 4
```c
blit(src, dest, 0, 0, 0, 0, src->w, src->h);
draw_sprite(dest, sprite, x, y);
rectfill(dest, x1, y1, x2, y2, color);
```

#### Allegro 5
```c
// Set target once
al_set_target_bitmap(dest);

// Draw multiple things
al_draw_bitmap_region(src, 0, 0, al_get_bitmap_width(src), 
                      al_get_bitmap_height(src), 0, 0, 0);
al_draw_bitmap(sprite, x, y, 0);
al_draw_filled_rectangle(x1, y1, x2, y2, color);

// When done, restore previous target if needed
```

#### Drawing to Display
```c
// Allegro 4
blit(buffer, screen, 0, 0, 0, 0, buffer->w, buffer->h);

// Allegro 5
al_set_target_backbuffer(display);
al_draw_bitmap(buffer, 0, 0, 0);
al_flip_display();  // IMPORTANT: Must flip to show
```

### Blitting Functions

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `blit(src, dest, sx, sy, dx, dy, w, h)` | Set target, then `al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0)` |
| `masked_blit(...)` | Same as `blit` (transparency via alpha) |
| `stretch_blit(src, dest, sx, sy, sw, sh, dx, dy, dw, dh)` | Set target, then `al_draw_scaled_bitmap(src, sx, sy, sw, sh, dx, dy, dw, dh, 0)` |
| `draw_sprite(dest, src, x, y)` | Set target, then `al_draw_bitmap(src, x, y, 0)` |
| `draw_sprite_h_flip(dest, src, x, y)` | Set target, then `al_draw_bitmap(src, x, y, ALLEGRO_FLIP_HORIZONTAL)` |
| `rotate_sprite(dest, src, x, y, angle)` | Set target, then `al_draw_rotated_bitmap(...)` |

### Colors

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `makecol(r, g, b)` | `al_map_rgb(r, g, b)` |
| `makecol(255, 0, 255)` (pink mask) | Use alpha channel instead |
| `getr(color)` | `(int)(color.r * 255)` |
| `getg(color)` | `(int)(color.g * 255)` |
| `getb(color)` | `(int)(color.b * 255)` |

**Type change:** Colors are `ALLEGRO_COLOR` structs in A5, not `int` values.

```c
// Allegro 4
int red = makecol(255, 0, 0);
rectfill(screen, 0, 0, 100, 100, red);

// Allegro 5
ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
al_set_target_backbuffer(display);
al_draw_filled_rectangle(0, 0, 100, 100, red);
```

### Drawing Primitives

Requires `allegro_primitives` addon.

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `line(bmp, x1, y1, x2, y2, color)` | Set target, then `al_draw_line(x1, y1, x2, y2, color, thickness)` |
| `rect(bmp, x1, y1, x2, y2, color)` | Set target, then `al_draw_rectangle(x1, y1, x2, y2, color, thickness)` |
| `rectfill(bmp, x1, y1, x2, y2, color)` | Set target, then `al_draw_filled_rectangle(x1, y1, x2, y2, color)` |
| `circle(bmp, x, y, r, color)` | Set target, then `al_draw_circle(x, y, r, color, thickness)` |
| `circlefill(bmp, x, y, r, color)` | Set target, then `al_draw_filled_circle(x, y, r, color)` |
| `ellipse(bmp, x, y, rx, ry, color)` | Set target, then `al_draw_ellipse(x, y, rx, ry, color, thickness)` |
| `triangle(bmp, x1,y1, x2,y2, x3,y3, color)` | Set target, then `al_draw_triangle(x1,y1, x2,y2, x3,y3, color, thickness)` |

**Note:** A5 primitives have a `thickness` parameter (use `1.0f` for 1-pixel lines).

### Text Rendering

TLC already uses the `alfont_*` compatibility layer which bridges to A5. No changes needed.

### Clearing Bitmaps

| Allegro 4 | Allegro 5 |
|-----------|-----------|
| `clear_bitmap(bmp)` | Set target, then `al_clear_to_color(al_map_rgba(0,0,0,0))` |
| `clear_to_color(bmp, color)` | Set target, then `al_clear_to_color(color)` |

### Input Handling

#### Keyboard

##### Allegro 4
```c
if (key[KEY_ESC]) {
    quit = true;
}
```

##### Allegro 5 - Polling
```c
ALLEGRO_KEYBOARD_STATE state;
al_get_keyboard_state(&state);
if (al_key_down(&state, ALLEGRO_KEY_ESCAPE)) {
    quit = true;
}
```

##### Allegro 5 - Events (Recommended)
```c
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
al_register_event_source(queue, al_get_keyboard_event_source());

while (running) {
    ALLEGRO_EVENT event;
    al_wait_for_event(queue, &event);
    
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            running = false;
        }
    }
}
```

#### Mouse

##### Allegro 4
```c
int x = mouse_x;
int y = mouse_y;
bool left_pressed = (mouse_b & 1);
```

##### Allegro 5
```c
ALLEGRO_MOUSE_STATE state;
al_get_mouse_state(&state);
int x = state.x;
int y = state.y;
bool left_pressed = state.buttons & 1;
```

Or use events for clicks:
```c
if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
    if (event.mouse.button == 1) {  // Left button
        // Handle click at event.mouse.x, event.mouse.y
    }
}
```

### Timers

#### Allegro 4
```c
volatile int timer_ticks = 0;

void timer_callback() {
    timer_ticks++;
}
END_OF_FUNCTION(timer_callback)

install_timer();
LOCK_VARIABLE(timer_ticks);
LOCK_FUNCTION(timer_callback);
install_int(timer_callback, 1000/60);  // 60 FPS
```

#### Allegro 5
```c
ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);  // 60 FPS
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
al_register_event_source(queue, al_get_timer_event_source(timer));
al_start_timer(timer);

// In event loop:
if (event.type == ALLEGRO_EVENT_TIMER) {
    // 1/60 second has elapsed
    update_game();
}
```

### Audio

TLC already uses FMOD for audio. The Allegro 5 audio system is used in `AudioSystem_allegro.cpp` for basic playback.

### Fixed-Point Math

Allegro 5 removed fixed-point types. Use floats/doubles instead.

The compatibility header provides conversion macros for legacy code:
- `itofix(x)` - Integer to fixed
- `fixtoi(x)` - Fixed to integer
- `fixtof(x)` - Fixed to float
- `ftofix(x)` - Float to fixed

But prefer using `float` or `double` directly in new code.

## Migration Checklist

When migrating a file:

- [ ] Replace `#include <allegro.h>` with `#include "allegro5_compat.h"`
- [ ] Update bitmap creation: `create_bitmap()` → `al_create_bitmap()`
- [ ] Update bitmap loading: `load_bitmap()` → `al_load_bitmap()`
- [ ] Replace `screen` with `al_get_backbuffer(display)` or explicit target
- [ ] Add `al_set_target_bitmap()` before drawing operations
- [ ] Update colors: `makecol()` → `al_map_rgb()`
- [ ] Replace `blit()` with `al_draw_bitmap_region()` (with target set)
- [ ] Update primitives: `rectfill()` → `al_draw_filled_rectangle()`, etc.
- [ ] Test: Compile, run, verify graphics/input work
- [ ] Add `#define TLC_A5_MIGRATION_COMPLETE` at top when fully migrated

## Common Pitfalls

### 1. Forgetting to Set Drawing Target

```c
// WRONG - no target set
al_draw_bitmap(sprite, x, y, 0);  // Where does this draw?

// RIGHT
al_set_target_bitmap(buffer);
al_draw_bitmap(sprite, x, y, 0);  // Draws to buffer
```

### 2. Forgetting to Flip Display

```c
// WRONG - nothing shows on screen
al_set_target_backbuffer(display);
al_clear_to_color(al_map_rgb(0, 0, 0));
al_draw_bitmap(sprite, x, y, 0);
// Missing: al_flip_display()

// RIGHT
al_set_target_backbuffer(display);
al_clear_to_color(al_map_rgb(0, 0, 0));
al_draw_bitmap(sprite, x, y, 0);
al_flip_display();  // Actually show it
```

### 3. Using `int` for Colors

```c
// WRONG - color type mismatch
int red = al_map_rgb(255, 0, 0);  // Returns ALLEGRO_COLOR, not int

// RIGHT
ALLEGRO_COLOR red = al_map_rgb(255, 0, 0);
```

### 4. Accessing Bitmap Width/Height

```c
// WRONG - ALLEGRO_BITMAP has no public w/h members
int w = bmp->w;
int h = bmp->h;

// RIGHT
int w = al_get_bitmap_width(bmp);
int h = al_get_bitmap_height(bmp);
```

### 5. Not Initializing Addons

```c
// WRONG - addon not initialized
al_init();
ALLEGRO_BITMAP *img = al_load_bitmap("sprite.png");  // Returns NULL!

// RIGHT
al_init();
al_init_image_addon();  // Required for image loading
ALLEGRO_BITMAP *img = al_load_bitmap("sprite.png");
```

## Performance Tips

### Batch Drawing Operations

```c
// SLOW - sets target 1000 times
for (int i = 0; i < 1000; i++) {
    draw_sprite(buffer, sprites[i], x[i], y[i]);  // Macro overhead
}

// FAST - sets target once
al_set_target_bitmap(buffer);
for (int i = 0; i < 1000; i++) {
    al_draw_bitmap(sprites[i], x[i], y[i], 0);
}
```

### Use Held Bitmaps for Static Graphics

```c
// Background that never changes
al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
ALLEGRO_BITMAP *bg = al_load_bitmap("background.png");

// Convert to video bitmap for faster drawing
al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
ALLEGRO_BITMAP *bg_video = al_clone_bitmap(bg);
al_destroy_bitmap(bg);
```

## Getting Help

- **Allegro 5 Manual:** https://liballeg.org/a5docs/trunk/
- **Allegro Forums:** https://www.allegro.cc/forums/
- **TLC Research Docs:**
  - `/research/allegro_porting_research.md`
  - `/research/allegro_patterns_catalog.md`
  - `/research/allegro5_compat_header.md`

## Example: Complete File Migration

### Before (Allegro 4 via Legacy)

```c
#include <allegro.h>
#include "Sprite.h"

void Sprite::Draw(BITMAP *dest, int x, int y) {
    if (!image) return;
    draw_sprite(dest, image, x, y);
}

void Sprite::DrawScaled(BITMAP *dest, int x, int y, int w, int h) {
    stretch_blit(image, dest, 
                 0, 0, image->w, image->h,
                 x, y, w, h);
}
```

### After (Allegro 5 Native)

```c
#define TLC_A5_MIGRATION_COMPLETE
#include "allegro5_compat.h"
#include "Sprite.h"

void Sprite::Draw(ALLEGRO_BITMAP *dest, int x, int y) {
    if (!image) return;
    
    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
    al_set_target_bitmap(dest);
    al_draw_bitmap(image, x, y, 0);
    al_set_target_bitmap(old_target);
}

void Sprite::DrawScaled(ALLEGRO_BITMAP *dest, int x, int y, int w, int h) {
    int img_w = al_get_bitmap_width(image);
    int img_h = al_get_bitmap_height(image);
    
    ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
    al_set_target_bitmap(dest);
    al_draw_scaled_bitmap(image, 0, 0, img_w, img_h, x, y, w, h, 0);
    al_set_target_bitmap(old_target);
}
```

---

**Last Updated:** 2026-02-02  
**Status:** Ready for use
