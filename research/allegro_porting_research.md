# Allegro 4 to Allegro 5 Porting Research

## Overview

Allegro 5 is a complete rewrite of the Allegro game programming library. Unlike the transition from Allegro 3 to Allegro 4, **Allegro 5 is NOT backwards compatible with Allegro 4**. The API has been redesigned from the ground up with modern hardware and programming paradigms in mind.

## Key Architectural Differences

### 1. Event-Driven vs Polling Model

**Allegro 4:**
- Uses polling model for input (checking `key[]` array, `mouse_x`, `mouse_y` globals)
- Timer callbacks via `install_int()` and `install_int_ex()`
- Interrupt-driven timers

**Allegro 5:**
- Event-driven architecture with event queues
- All input (keyboard, mouse, timer, display) generates events
- Events are retrieved via `al_wait_for_event()` or `al_get_next_event()`
- Much cleaner separation of concerns

```c
// Allegro 4 polling
while (!key[KEY_ESC]) {
    if (key[KEY_UP]) move_up();
    rest(10);
}

// Allegro 5 events
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
al_register_event_source(queue, al_get_keyboard_event_source());
while (running) {
    ALLEGRO_EVENT event;
    al_wait_for_event(queue, &event);
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) running = false;
    }
}
```

### 2. Display/Graphics Mode Setup

**Allegro 4:**
```c
allegro_init();
set_color_depth(32);
set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
```

**Allegro 5:**
```c
al_init();
ALLEGRO_DISPLAY *display = al_create_display(640, 480);
```

### 3. Bitmap and Drawing

**Allegro 4:**
- `BITMAP` structure
- `screen` global for display buffer
- `blit()`, `draw_sprite()`, `masked_blit()` functions
- Direct pixel access common
- Double buffering manually managed

**Allegro 5:**
- `ALLEGRO_BITMAP` structure
- Backbuffer concept (display has implicit backbuffer)
- `al_draw_bitmap()`, `al_draw_bitmap_region()`, `al_draw_scaled_bitmap()`
- GPU-accelerated drawing (OpenGL/Direct3D backend)
- `al_flip_display()` for page flipping

### 4. Color Handling

**Allegro 4:**
```c
makecol(255, 0, 0);  // Red
```

**Allegro 5:**
```c
al_map_rgb(255, 0, 0);  // Returns ALLEGRO_COLOR
```

### 5. Timer System

**Allegro 4:**
```c
install_timer();
install_int(timer_callback, 1000/60);  // 60 FPS callback
volatile int timer_ticks = 0;
void timer_callback() { timer_ticks++; }
```

**Allegro 5:**
```c
ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);
al_register_event_source(queue, al_get_timer_event_source(timer));
al_start_timer(timer);
// Timer events come through the event queue
```

### 6. Audio System

**Allegro 4:**
```c
install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
SAMPLE *sound = load_sample("sound.wav");
play_sample(sound, 255, 128, 1000, 0);
```

**Allegro 5:**
```c
al_install_audio();
al_init_acodec_addon();
ALLEGRO_SAMPLE *sample = al_load_sample("sound.wav");
al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
```

## API Function Mapping

| Allegro 4 | Allegro 5 | Notes |
|-----------|-----------|-------|
| `allegro_init()` | `al_init()` | |
| `install_keyboard()` | `al_install_keyboard()` | |
| `install_mouse()` | `al_install_mouse()` | |
| `install_timer()` | `al_install_system()` handles this | Built into core |
| `set_gfx_mode()` | `al_create_display()` | |
| `set_color_depth()` | Display options before creation | |
| `create_bitmap()` | `al_create_bitmap()` | |
| `load_bitmap()` | `al_load_bitmap()` | Requires image addon |
| `destroy_bitmap()` | `al_destroy_bitmap()` | |
| `blit()` | `al_draw_bitmap_region()` | |
| `draw_sprite()` | `al_draw_bitmap()` | |
| `masked_blit()` | `al_draw_bitmap()` with alpha | Transparency via alpha channel |
| `stretch_blit()` | `al_draw_scaled_bitmap()` | |
| `rotate_sprite()` | `al_draw_rotated_bitmap()` | |
| `clear_bitmap()` | `al_clear_to_color()` | |
| `makecol()` | `al_map_rgb()` | |
| `putpixel()` | `al_put_pixel()` | |
| `getpixel()` | `al_get_pixel()` | |
| `line()` | `al_draw_line()` | Primitives addon |
| `rect()` | `al_draw_rectangle()` | Primitives addon |
| `rectfill()` | `al_draw_filled_rectangle()` | Primitives addon |
| `circle()` | `al_draw_circle()` | Primitives addon |
| `circlefill()` | `al_draw_filled_circle()` | Primitives addon |
| `triangle()` | `al_draw_triangle()` | Primitives addon |
| `textout_ex()` | `al_draw_text()` | Font addon |
| `textprintf_ex()` | `al_draw_textf()` | Font addon |
| `load_font()` | `al_load_font()` | Font addon |
| `key[]` array | Event queue + key state | `al_key_down()` for state |
| `mouse_x`, `mouse_y` | `al_get_mouse_state()` | |
| `install_int()` | `al_create_timer()` + events | |
| `rest()` | `al_rest()` | |
| `screen` global | `al_get_backbuffer()` | |
| `SCREEN_W`, `SCREEN_H` | `al_get_display_width/height()` | |
| `install_sound()` | `al_install_audio()` | Audio addon |
| `load_sample()` | `al_load_sample()` | Audio addon |
| `play_sample()` | `al_play_sample()` | |
| `load_midi()` | No direct equivalent | Use audio streams |
| `play_midi()` | No direct equivalent | |
| `packfile` functions | `ALLEGRO_FILE` API | |
| `load_datafile()` | No direct equivalent | Use individual files |
| `fixed` type | Removed | Use float/double |

## Library Structure

Allegro 5 is modular with separate addons:

| Addon | Purpose | Link Flag |
|-------|---------|-----------|
| `allegro` | Core library | `-lallegro` |
| `allegro_image` | Image loading (PNG, JPG, BMP, etc.) | `-lallegro_image` |
| `allegro_font` | Font support | `-lallegro_font` |
| `allegro_ttf` | TrueType font support | `-lallegro_ttf` |
| `allegro_primitives` | Drawing primitives (lines, shapes) | `-lallegro_primitives` |
| `allegro_audio` | Audio playback | `-lallegro_audio` |
| `allegro_acodec` | Audio codecs (WAV, OGG, etc.) | `-lallegro_acodec` |
| `allegro_dialog` | Native dialogs | `-lallegro_dialog` |
| `allegro_memfile` | Memory file I/O | `-lallegro_memfile` |
| `allegro_physfs` | PhysicsFS integration | `-lallegro_physfs` |

## Compatibility Layers

Two projects exist to help with porting:

### 1. Allegro-Legacy (NewCreature)
- https://github.com/NewCreature/Allegro-Legacy
- Full Allegro 4 API compatibility
- Uses Allegro 5 backend
- Can compile existing Allegro 4 code with minimal changes
- Good for quick ports where you want to maintain Allegro 4 code

### 2. allegro4-to-5 (kazzmir)
- https://github.com/kazzmir/allegro4-to-5
- Compatibility layer between Allegro 4 and Allegro 5
- Implements Allegro 4 API on top of Allegro 5

## Removed Features (No Direct Allegro 5 Equivalent)

1. **MIDI playback** - Allegro 5 removed built-in MIDI support
2. **Datafiles (.dat)** - Use individual resource files instead
3. **GUI system** - Allegro 4's GUI was removed; use external GUI libraries
4. **RLE sprites** - GPU handles this efficiently now
5. **Compiled sprites** - Not needed with GPU acceleration
6. **Mode-X graphics** - Legacy DOS feature
7. **Fixed-point math** - Use floating point
8. **FLI/FLC animation** - Use video addon or external libraries
9. **Palette manipulation** - Truecolor is standard now

## Best Practices for Porting

1. **Start with the event loop** - Convert to event-driven architecture first
2. **Replace globals** - `screen`, `key[]`, mouse variables all change
3. **Add addon initialization** - Each addon needs explicit init
4. **Update drawing code** - All drawing targets display backbuffer by default
5. **Handle color differently** - Allegro 5 uses ALLEGRO_COLOR struct
6. **Test incrementally** - Port one subsystem at a time

## Sample Conversion: Minimal Program

**Allegro 4:**
```c
#include <allegro.h>

int main() {
    allegro_init();
    install_keyboard();
    set_color_depth(32);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
    
    clear_to_color(screen, makecol(0, 0, 0));
    textout_centre_ex(screen, font, "Hello World!", 320, 240, makecol(255, 255, 255), -1);
    
    while (!key[KEY_ESC]) {
        rest(10);
    }
    
    return 0;
}
END_OF_MAIN()
```

**Allegro 5:**
```c
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

int main() {
    al_init();
    al_install_keyboard();
    al_init_font_addon();
    
    ALLEGRO_DISPLAY *display = al_create_display(640, 480);
    ALLEGRO_FONT *font = al_create_builtin_font();
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    
    al_register_event_source(queue, al_get_keyboard_event_source());
    
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(font, al_map_rgb(255, 255, 255), 320, 240, ALLEGRO_ALIGN_CENTER, "Hello World!");
    al_flip_display();
    
    bool running = true;
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            running = false;
        }
    }
    
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(queue);
    
    return 0;
}
```

## Resources

- Official Allegro 5 Manual: https://liballeg.org/a5docs/trunk/
- Allegro Wiki: https://wiki.allegro.cc/
- Allegro Forums: https://www.allegro.cc/forums/
- Allegro-Legacy: https://github.com/NewCreature/Allegro-Legacy
- allegro4-to-5: https://github.com/kazzmir/allegro4-to-5
