/*
 * ALLEGRO 5 COMPATIBILITY HEADER
 * 
 * Purpose: Bridge layer for migrating from Allegro 4 (via Allegro Legacy) to native Allegro 5 APIs
 * 
 * Migration Status: PHASE 1 - INITIAL COMPATIBILITY LAYER
 * 
 * This header provides:
 * 1. All necessary Allegro 5 includes
 * 2. Type compatibility bridges (BITMAP = ALLEGRO_BITMAP)
 * 3. Function macro wrappers for common operations
 * 
 * Usage Pattern:
 * - For new code: Use native Allegro 5 APIs directly (al_* functions)
 * - For legacy code: Include this header to enable gradual migration
 * - Migration path: Replace macro calls with native al_* calls file-by-file
 * 
 * Current State:
 * - TLC uses Allegro Legacy for most graphics (provides Allegro 4 API on A5 backend)
 * - Audio system already uses native Allegro 5 (AudioSystem_allegro.cpp)
 * - Font system uses hybrid approach (alfont_compat.cpp bridges to A5)
 * 
 * Next Steps:
 * - Phase 2: Migrate display/graphics initialization in Game.cpp
 * - Phase 3: Convert bitmap operations (blit, masked_blit, etc.)
 * - Phase 4: Migrate drawing primitives (lines, rectangles, circles)
 * - Phase 5: Convert color system (makecol → al_map_rgb)
 * - Phase 6: Update input system (key[] → events, mouse globals → al_get_mouse_state)
 */

#ifndef ALLEGRO5_COMPAT_H
#define ALLEGRO5_COMPAT_H

/*=============================================================================
 * ALLEGRO 5 CORE HEADERS
 *===========================================================================*/

#include <allegro5/allegro.h>           /* Core library */
#include <allegro5/allegro_image.h>     /* Image loading (PNG, JPG, BMP, TGA) */
#include <allegro5/allegro_font.h>      /* Font support */
#include <allegro5/allegro_ttf.h>       /* TrueType font support */
#include <allegro5/allegro_primitives.h> /* Drawing primitives (lines, shapes) */
#include <allegro5/allegro_audio.h>     /* Audio playback */
#include <allegro5/allegro_acodec.h>    /* Audio codecs (WAV, OGG, etc.) */

/*=============================================================================
 * TYPE COMPATIBILITY LAYER
 *===========================================================================*/

/*
 * TEMPORARY TYPE BRIDGE
 * 
 * These typedefs allow existing code using BITMAP* to work with ALLEGRO_BITMAP*
 * during the migration period. As files are converted, these references should
 * be replaced with native ALLEGRO_BITMAP* types.
 * 
 * WARNING: This is a temporary bridge only. Do not write new code using these!
 */
#ifndef TLC_USING_ALLEGRO_LEGACY
typedef ALLEGRO_BITMAP BITMAP;
typedef ALLEGRO_COLOR COLOR;
typedef ALLEGRO_DISPLAY DISPLAY;
typedef ALLEGRO_FONT FONT;
typedef ALLEGRO_EVENT_QUEUE EVENT_QUEUE;
typedef ALLEGRO_TIMER TIMER;
typedef ALLEGRO_SAMPLE SAMPLE;
typedef ALLEGRO_SAMPLE_INSTANCE SAMPLE_INSTANCE;
typedef ALLEGRO_MOUSE_STATE MOUSE_STATE;
typedef ALLEGRO_KEYBOARD_STATE KEYBOARD_STATE;
#endif

/*=============================================================================
 * BITMAP OPERATION COMPATIBILITY MACROS
 *===========================================================================*/

/*
 * These macros provide Allegro 4-style function names that map to Allegro 5 APIs.
 * They handle the common case but may not cover all parameters/options.
 * 
 * MIGRATION NOTE: Replace these macro calls with direct al_* function calls
 * for better control, error handling, and to match Allegro 5 conventions.
 */

#ifndef TLC_USING_ALLEGRO_LEGACY

/* Bitmap creation and destruction */
#define create_bitmap(w, h) al_create_bitmap(w, h)
#define destroy_bitmap(bmp) al_destroy_bitmap(bmp)
#define load_bitmap(filename, pal) al_load_bitmap(filename)
#define save_bitmap(filename, bmp) al_save_bitmap(filename, bmp)

/* Bitmap properties */
#define bitmap_color_depth(bmp) 32  /* A5 always uses 32-bit color */
#define is_screen_bitmap(bmp) (false)  /* A5 uses display backbuffer concept */
#define is_video_bitmap(bmp) (true)    /* A5 bitmaps are GPU-accelerated by default */
#define is_memory_bitmap(bmp) (false)

/* Bitmap drawing target management */
/*
 * IMPORTANT: Allegro 5 uses an explicit drawing target system.
 * Before drawing, you must set the target bitmap:
 *   al_set_target_bitmap(bmp);
 * 
 * To draw to display backbuffer:
 *   al_set_target_backbuffer(display);
 */

/* Clear operations */
#define clear_bitmap(bmp) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_clear_to_color(al_map_rgba(0, 0, 0, 0)); \
        al_set_target_bitmap(_old); \
    } while(0)

#define clear_to_color(bmp, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_clear_to_color(color); \
        al_set_target_bitmap(_old); \
    } while(0)

/*
 * Blitting Functions
 * 
 * Allegro 4 uses blit(src, dest, sx, sy, dx, dy, w, h)
 * Allegro 5 uses al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, flags)
 * 
 * Key difference: A5 draws to current target, not to a dest parameter
 */

/* Helper: Set target and draw (use sparingly, prefer explicit target management) */
#define blit(src, dest, sx, sy, dx, dy, w, h) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0); \
        al_set_target_bitmap(_old); \
    } while(0)

#define masked_blit(src, dest, sx, sy, dx, dy, w, h) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0); \
        al_set_target_bitmap(_old); \
    } while(0)

#define stretch_blit(src, dest, sx, sy, sw, sh, dx, dy, dw, dh) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_scaled_bitmap(src, sx, sy, sw, sh, dx, dy, dw, dh, 0); \
        al_set_target_bitmap(_old); \
    } while(0)

#define masked_stretch_blit(src, dest, sx, sy, sw, sh, dx, dy, dw, dh) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_scaled_bitmap(src, sx, sy, sw, sh, dx, dy, dw, dh, 0); \
        al_set_target_bitmap(_old); \
    } while(0)

/* Sprite drawing */
#define draw_sprite(dest, src, x, y) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap(src, x, y, 0); \
        al_set_target_bitmap(_old); \
    } while(0)

#define draw_sprite_h_flip(dest, src, x, y) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap(src, x, y, ALLEGRO_FLIP_HORIZONTAL); \
        al_set_target_bitmap(_old); \
    } while(0)

#define draw_sprite_v_flip(dest, src, x, y) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap(src, x, y, ALLEGRO_FLIP_VERTICAL); \
        al_set_target_bitmap(_old); \
    } while(0)

#define draw_sprite_vh_flip(dest, src, x, y) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap(src, x, y, ALLEGRO_FLIP_HORIZONTAL | ALLEGRO_FLIP_VERTICAL); \
        al_set_target_bitmap(_old); \
    } while(0)

/*
 * Rotated sprite drawing
 * 
 * Allegro 4: rotate_sprite(dest, src, x, y, angle_fixed)
 *   - angle is 16.16 fixed point (256 = full rotation)
 * 
 * Allegro 5: al_draw_rotated_bitmap(src, cx, cy, dx, dy, angle_radians, flags)
 *   - angle is in radians (2π = full rotation)
 *   - cx, cy is center of rotation within sprite
 *   - dx, dy is destination position
 * 
 * Conversion: fixed angle / 256 * 2π radians
 */
#define rotate_sprite(dest, src, x, y, angle_fixed) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        float cx = al_get_bitmap_width(src) / 2.0f; \
        float cy = al_get_bitmap_height(src) / 2.0f; \
        float angle_rad = ((float)fixtof(angle_fixed)) * ALLEGRO_PI * 2.0f / 256.0f; \
        al_draw_rotated_bitmap(src, cx, cy, x + cx, y + cy, angle_rad, 0); \
        al_set_target_bitmap(_old); \
    } while(0)

/*=============================================================================
 * COLOR COMPATIBILITY MACROS
 *===========================================================================*/

/*
 * Color creation
 * 
 * Allegro 4: makecol(r, g, b) returns int color value
 * Allegro 5: al_map_rgb(r, g, b) returns ALLEGRO_COLOR struct
 */
#define makecol(r, g, b) al_map_rgb(r, g, b)
#define makeacol(r, g, b, a) al_map_rgba(r, g, b, a)

/* Color component extraction */
#define getr(color) (color.r * 255)
#define getg(color) (color.g * 255)
#define getb(color) (color.b * 255)
#define geta(color) (color.a * 255)

/*=============================================================================
 * DRAWING PRIMITIVES COMPATIBILITY
 *===========================================================================*/

/*
 * Drawing primitives (requires allegro_primitives addon)
 * 
 * Note: A5 primitives draw to current target, not to dest parameter
 */

#define putpixel(bmp, x, y, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_put_pixel(x, y, color); \
        al_set_target_bitmap(_old); \
    } while(0)

#define getpixel(bmp, x, y) al_get_pixel(bmp, x, y)

#define line(bmp, x1, y1, x2, y2, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_line(x1, y1, x2, y2, color, 1.0f); \
        al_set_target_bitmap(_old); \
    } while(0)

#define rect(bmp, x1, y1, x2, y2, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_rectangle(x1, y1, x2, y2, color, 1.0f); \
        al_set_target_bitmap(_old); \
    } while(0)

#define rectfill(bmp, x1, y1, x2, y2, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_filled_rectangle(x1, y1, x2, y2, color); \
        al_set_target_bitmap(_old); \
    } while(0)

#define circle(bmp, x, y, radius, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_circle(x, y, radius, color, 1.0f); \
        al_set_target_bitmap(_old); \
    } while(0)

#define circlefill(bmp, x, y, radius, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_filled_circle(x, y, radius, color); \
        al_set_target_bitmap(_old); \
    } while(0)

#define ellipse(bmp, x, y, rx, ry, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_ellipse(x, y, rx, ry, color, 1.0f); \
        al_set_target_bitmap(_old); \
    } while(0)

#define triangle(bmp, x1, y1, x2, y2, x3, y3, color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        al_draw_triangle(x1, y1, x2, y2, x3, y3, color, 1.0f); \
        al_set_target_bitmap(_old); \
    } while(0)

/*=============================================================================
 * SYSTEM COMPATIBILITY
 *===========================================================================*/

/* Timer/delay functions */
#define rest(ms) al_rest((ms) / 1000.0)

/* Fixed-point math (Allegro 5 removed fixed-point, use float) */
#define itofix(x) ((x) << 16)
#define fixtoi(x) ((x) >> 16)
#define fixtof(x) ((float)(x) / 65536.0f)
#define ftofix(x) ((int)((x) * 65536.0f))

/*=============================================================================
 * INPUT SYSTEM COMPATIBILITY NOTES
 *===========================================================================*/

/*
 * INPUT MIGRATION REQUIRED
 * 
 * Allegro 4 uses polling with global state:
 *   - key[KEY_ESC] array for keyboard
 *   - mouse_x, mouse_y, mouse_b globals
 * 
 * Allegro 5 uses event-driven system:
 *   - al_get_keyboard_state() / al_key_down()
 *   - al_get_mouse_state()
 *   - ALLEGRO_EVENT_QUEUE for event handling
 * 
 * TLC already has custom input handling in Game.cpp event loop.
 * Key migration: Replace key[] and mouse_* references with A5 state queries.
 */

#endif /* TLC_USING_ALLEGRO_LEGACY */

/*=============================================================================
 * INITIALIZATION HELPERS
 *===========================================================================*/

/*
 * Initialize all Allegro 5 subsystems required by TLC
 * 
 * This function should be called during game initialization, replacing
 * the individual addon initialization calls.
 * 
 * Returns: true on success, false on failure
 */
inline bool tlc_allegro5_init_all()
{
    if (!al_init()) {
        return false;
    }
    
    if (!al_init_image_addon()) {
        return false;
    }
    
    if (!al_init_font_addon()) {
        return false;
    }
    
    if (!al_init_ttf_addon()) {
        return false;
    }
    
    if (!al_init_primitives_addon()) {
        return false;
    }
    
    if (!al_install_keyboard()) {
        return false;
    }
    
    if (!al_install_mouse()) {
        return false;
    }
    
    if (!al_install_audio()) {
        return false;
    }
    
    if (!al_init_acodec_addon()) {
        return false;
    }
    
    return true;
}

/*=============================================================================
 * MIGRATION STATUS TRACKING
 *===========================================================================*/

/*
 * Define TLC_A5_MIGRATION_COMPLETE when a file has been fully converted
 * to native Allegro 5 APIs. This helps track migration progress.
 * 
 * Usage in source files:
 *   #define TLC_A5_MIGRATION_COMPLETE
 *   #include "allegro5_compat.h"
 * 
 * Files fully migrated:
 *   - alfont_compat.cpp (fonts)
 *   - AudioSystem_allegro.cpp (audio)
 * 
 * Files in progress:
 *   - (none yet)
 * 
 * Files remaining:
 *   - Most module files (see phase0_analysis.md for full list)
 */

#ifdef TLC_A5_MIGRATION_COMPLETE
    #pragma message("This file has been fully migrated to Allegro 5 native APIs")
#endif

#endif /* ALLEGRO5_COMPAT_H */
