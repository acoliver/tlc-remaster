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
 * Completed Migration Phases:
 * - Phase 6: Input system migrated (key[] → al_get_keyboard_state, mouse globals → al_get_mouse_state)
 * 
 * Next Steps:
 * - Phase 2: Migrate display/graphics initialization in Game.cpp
 * - Phase 3: Convert bitmap operations (blit, masked_blit, etc.)
 * - Phase 4: Migrate drawing primitives (lines, rectangles, circles)
 * - Phase 5: Convert color system (makecol → al_map_rgb)
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
#include <allegro5/allegro_native_dialog.h> /* Native message boxes */
#include <cctype>
#include <cstring>

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

/* Forward declaration prevention - these are now typedefs, not structs */
#define BITMAP ALLEGRO_BITMAP

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

/* ASSERT macro - Allegro Legacy provides this, Allegro 5 doesn't */
#include <cassert>
#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

/* TRACE macro - debug output (Allegro Legacy provides this) */
#ifndef TRACE
#ifdef _DEBUG
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...) ((void)0)
#endif
#endif

/* END_OF_MAIN() - Allegro Legacy uses this for "magic main" on some platforms.
 * In native Allegro 5, we use allegro_main addon which handles WinMain etc. */
#ifndef END_OF_MAIN
#define END_OF_MAIN()
#endif

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
#define load_bitmap(filename, pal) tlc_load_bitmap(filename)
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
        al_clear_to_color(int_to_al_color(color)); \
        al_set_target_bitmap(_old); \
    } while(0)

/* Helper function for clear(bitmap) - Allegro 4 style */
inline void clear(ALLEGRO_BITMAP *bmp) {
    ALLEGRO_BITMAP *_old = al_get_target_bitmap();
    al_set_target_bitmap(bmp);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_set_target_bitmap(_old);
}

/*
 * Blitting Functions - CONVERTED
 * 
 * All blit/masked_blit/stretch_blit/draw_sprite/rotate_sprite calls have been
 * converted to native Allegro 5 functions. The macros have been removed.
 * 
 * Use the following Allegro 5 functions directly:
 * - al_set_target_bitmap() + al_draw_bitmap_region()
 * - al_set_target_bitmap() + al_draw_scaled_bitmap()
 * - al_set_target_bitmap() + al_draw_bitmap()
 * - al_set_target_bitmap() + al_draw_rotated_bitmap()
 */

/*=============================================================================
 * COLOR COMPATIBILITY LAYER
 *===========================================================================*/

/*
 * Color handling
 * 
 * Allegro 4: Colors are packed 32-bit integers (0xAARRGGBB or similar)
 * Allegro 5: Colors are ALLEGRO_COLOR structs
 * 
 * For API compatibility, we keep using int for color parameters.
 * Convert to ALLEGRO_COLOR only when calling A5 drawing functions.
 */

/* Pack RGB into int (same as A4 32-bit format) */
#define makecol(r, g, b) (((r) << 16) | ((g) << 8) | (b))
#define makeacol(r, g, b, a) (((a) << 24) | ((r) << 16) | ((g) << 8) | (b))

/* Extract components from packed int color */
#define getr(c) (((c) >> 16) & 0xFF)
#define getg(c) (((c) >> 8) & 0xFF)
#define getb(c) ((c) & 0xFF)
#define geta(c) (((c) >> 24) & 0xFF)

/* Convert packed int color to ALLEGRO_COLOR for A5 drawing calls */
inline ALLEGRO_COLOR int_to_al_color(int c) {
    return al_map_rgba(getr(c), getg(c), getb(c), 255);
}

/* Convert with alpha */
inline ALLEGRO_COLOR int_to_al_color_alpha(int c) {
    return al_map_rgba(getr(c), getg(c), getb(c), geta(c));
}

/* Convert ALLEGRO_COLOR to packed int (for alfont compatibility) */
inline int pack_color(ALLEGRO_COLOR c) {
    unsigned char r, g, b;
    al_unmap_rgb(c, &r, &g, &b);
    return makecol(r, g, b);
}

/* Alias for pack_color - more descriptive name when converting colors for alfont */
inline int color_to_int(ALLEGRO_COLOR c) {
    unsigned char r, g, b;
    al_unmap_rgb(c, &r, &g, &b);
    return makecol(r, g, b);
}

/*=============================================================================
 * DRAWING PRIMITIVES COMPATIBILITY
 *===========================================================================*/

/*
 * Drawing primitives (requires allegro_primitives addon)
 * 
 * Note: A5 primitives draw to current target, not to dest parameter
 */


/*=============================================================================
 * TEXT OUTPUT COMPATIBILITY
 *===========================================================================*/

/*
 * Text output functions
 * 
 * Allegro 4: textout_ex(bmp, font, text, x, y, fg_color, bg_color)
 * Allegro 5: al_draw_text(font, color, x, y, flags, text)
 * 
 * Note: A5 doesn't support background color in the same way.
 * For now, we ignore the bg_color parameter.
 */

/* Forward declaration for ALFONT_FONT compatibility */
struct ALFONT_FONT;

#define textout_ex(bmp, fnt, str, x, y, fg_color, bg_color) \
    do { \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        ALLEGRO_FONT *_font = (ALLEGRO_FONT*)(fnt); \
        al_draw_text(_font, int_to_al_color(fg_color), x, y, 0, str); \
        al_set_target_bitmap(_old); \
    } while(0)

#define textprintf_ex(bmp, fnt, x, y, fg_color, bg_color, fmt, ...) \
    do { \
        char _buf[1024]; \
        snprintf(_buf, sizeof(_buf), fmt, __VA_ARGS__); \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        ALLEGRO_FONT *_font = (ALLEGRO_FONT*)(fnt); \
        al_draw_text(_font, int_to_al_color(fg_color), x, y, 0, _buf); \
        al_set_target_bitmap(_old); \
    } while(0)


#define textprintf_centre_ex(bmp, fnt, x, y, fg_color, bg_color, fmt, ...) \
    do { \
        char _buf[1024]; \
        snprintf(_buf, sizeof(_buf), fmt, __VA_ARGS__); \
        ALLEGRO_BITMAP *_old = al_get_target_bitmap(); \
        al_set_target_bitmap(bmp); \
        ALLEGRO_FONT *_font = (ALLEGRO_FONT*)(fnt); \
        al_draw_text(_font, int_to_al_color(fg_color), x, y, ALLEGRO_ALIGN_CENTER, _buf); \
        al_set_target_bitmap(_old); \
    } while(0)

/*=============================================================================
 * SYSTEM COMPATIBILITY
 *===========================================================================*/

/* Fixed-point math (Allegro 5 removed fixed-point, use float) */
#define itofix(x) ((x) << 16)
#define fixtoi(x) ((x) >> 16)
#define fixtof(x) ((float)(x) / 65536.0f)
#define ftofix(x) ((int)((x) * 65536.0f))

/*=============================================================================
 * BITMAP PROPERTY ACCESS
 *===========================================================================*/

/*
 * In Allegro 4, BITMAP was a struct with direct member access: bmp->w, bmp->h
 * In Allegro 5, ALLEGRO_BITMAP is opaque, use accessor functions.
 * 
 * Since we can't override -> operator with macros, code that uses bmp->w
 * must be changed to use these helper macros or direct al_get_bitmap_* calls.
 */

/* Helper macros for bitmap properties - use these instead of ->w, ->h */
#define bitmap_width(bmp) al_get_bitmap_width(bmp)
#define bitmap_height(bmp) al_get_bitmap_height(bmp)


/*=============================================================================
 * MAGENTA MASK CONVERSION + LOAD HELPER
 *===========================================================================*/

inline void tlc_convert_magenta_to_alpha(ALLEGRO_BITMAP *bitmap);

inline bool tlc_is_magenta_mask_asset(const char *filename)
{
    if (!filename) {
        return false;
    }

    const char *ext = strrchr(filename, '.');
    if (!ext) {
        return false;
    }

    char extLower[8];
    size_t extLen = strlen(ext);
    if (extLen >= sizeof(extLower)) {
        extLen = sizeof(extLower) - 1;
    }
    for (size_t i = 0; i < extLen; ++i) {
        extLower[i] = (char)std::tolower((unsigned char)ext[i]);
    }
    extLower[extLen] = '\0';

    if (strcmp(extLower, ".bmp") != 0) {
        return false;
    }

    const char *base = strrchr(filename, '/');
    base = base ? base + 1 : filename;

    static const char *const kExplicitSkip[] = {
        "medical_gui_viewer.bmp",
        "medical_gui_viewer_right.bmp",
        "medical_gui_viewer_left.bmp",
        "gui_messagewindow.bmp",
        "gui_socket.bmp",
        "gui_viewer.bmp",
        "gui_viewer_right.bmp",
        "gui_aux.bmp",
        "shipconfig_btn_deactive.bmp",
        "shipconfig_btn_norm.bmp",
        "shipconfig_btn_over.bmp",
        "shipconfig_cursor0.bmp",
        "bank_background.bmp",
        "bank_banner.bmp",
        "bank_button_confirm_hover.bmp",
        "bank_button_confirm_normal.bmp",
        "bank_button_exit.bmp",
        "bank_button_exit_hover.bmp",
        "bank_button_help.bmp",
        "bank_button_help_hover.bmp",
        "bank_button_pay_hover.bmp",
        "bank_button_pay_normal.bmp",
        "bank_button_take_hover.bmp",
        "bank_button_take_normal.bmp",
        "bank_calc_button_deactivate.bmp",
        "bank_calc_button_hover.bmp",
        "bank_calc_button_normal.bmp",
        "bank_help_window.bmp",
        "cantina_background.bmp",
        "militaryops_background.bmp",
        "researchlab_background.bmp",
        "tradedepot_background.bmp",
        "tradedepot_btn.bmp",
        "tradedepot_btn_mo.bmp",
        "tradedepot_cursor0.bmp",
        "tradedepot_cursor1.bmp",
        "tradedepot_filterbtn.bmp",
        "tradedepot_filterbtn_mo.bmp",
        "tradedepot_promptbtn.bmp",
        "tradedepot_promptbtn_mo.bmp",
        "tradedepot_quantity_prompt.bmp",
        "tradedepot_spindownbtn.bmp",
        "tradedepot_spindownbtn_mo.bmp",
        "tradedepot_spinupbtn.bmp",
        "tradedepot_spinupbtn_mo.bmp",
        "crewlist_bar.bmp",
        "crewlist_bar2.bmp",
        "crewlist_bar3.bmp",
        "crewlist_bar4.bmp",
        "crewlist_bar5.bmp",
        "crewlist_bar6.bmp",
        "crewlist_bar7.bmp",
        "crewlist_bar8.bmp",
        "crewlist_bar9.bmp",
        "crewlist_bar10.bmp",
        "crewlist_bar11.bmp",
        "crewlist_bar12.bmp",
        "crewlist_bar13.bmp",
        "crewlist_bar14.bmp",
        "crewlist_bar15.bmp",
        "crewlist_bar16.bmp",
        "crewlist_bar17.bmp",
        "crewlist_bar18.bmp",
        "crewlist_bar19.bmp",
        "crewlist_bar20.bmp",
        "crewlist_bar21.bmp",
        "crewlist_bar22.bmp",
        "crewlist_bar23.bmp",
        "crewlist_bar24.bmp",
        "crewlist_bar25.bmp",
        "crewlist_bar26.bmp",
        "crewlist_bar27.bmp",
        "crewlist_bar28.bmp",
        "crewlist_bar29.bmp",
        "crewlist_bar30.bmp",
        "crewlist_bar31.bmp",
        "crewlist_bar32.bmp",
        "crewlist_bar33.bmp",
        "crewlist_bar34.bmp",
        "crewlist_bar35.bmp",
        "crewlist_bar36.bmp",
        "crewlist_bar37.bmp",
        "crewlist_bar38.bmp",
        "crewlist_bar39.bmp",
        "crewlist_bar40.bmp",
        "crewlist_bar41.bmp",
        "crewlist_bar42.bmp",
        "crewlist_bar43.bmp",
        "crewlist_bar44.bmp",
        "crewlist_bar45.bmp",
        "crewlist_bar46.bmp",
        "crewlist_bar47.bmp",
        "crewlist_bar48.bmp",
        "crewlist_bar49.bmp",
        "crewlist_bar50.bmp",
        "crewlist_bar51.bmp",
        "crewlist_bar52.bmp",
        "crewlist_bar53.bmp",
        "crewlist_bar54.bmp",
        "crewlist_bar55.bmp",
        "crewlist_bar56.bmp",
        "crewlist_bar57.bmp",
        "crewlist_bar58.bmp",


        "crewlist_bar59.bmp",
        "crewlist_bar60.bmp",
        "crewlist_bar61.bmp",
        "crewlist_bar62.bmp",
        "crewlist_bar63.bmp",
        "crewlist_bar64.bmp"
    };

    for (size_t i = 0; i < sizeof(kExplicitSkip) / sizeof(kExplicitSkip[0]); ++i) {
        if (strcmp(base, kExplicitSkip[i]) == 0) {
            return false;
        }
    }

    return true;
}

inline ALLEGRO_BITMAP *tlc_load_bitmap(const char *filename)
{
    ALLEGRO_BITMAP *bitmap = al_load_bitmap(filename);
    if (bitmap && tlc_is_magenta_mask_asset(filename)) {
        tlc_convert_magenta_to_alpha(bitmap);
    }
    return bitmap;
}



/* For code that needs a "screen" global - this should point to display backbuffer */
/* The game must set this during initialization */
extern ALLEGRO_BITMAP *_tlc_screen;
extern ALLEGRO_DISPLAY *_tlc_display;
#define screen _tlc_screen

/* Screen dimensions - game should set these during init */
extern int SCREEN_W, SCREEN_H;

#ifndef TLC_DISABLE_MAGENTA_LOAD_HOOK
#define al_load_bitmap tlc_load_bitmap
#endif

/*=============================================================================
 * DISPLAY/GRAPHICS MODE COMPATIBILITY
 *===========================================================================*/

/* Graphics mode constants (no longer used - kept for reference only) */

/* Desktop/display info */
inline int get_desktop_resolution(int *w, int *h) {
    ALLEGRO_MONITOR_INFO info;
    if (al_get_monitor_info(0, &info)) {
        *w = info.x2 - info.x1;
        *h = info.y2 - info.y1;
        return 0;
    }
    *w = 1024;
    *h = 768;
    return -1;
}

inline int desktop_color_depth() {
    return 32; /* A5 always uses 32-bit */
}

inline void set_color_depth(int depth) {
    (void)depth; /* A5 always uses 32-bit, ignore */
}

inline void set_alpha_blender() {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

inline int get_refresh_rate() {
    return 60; /* Default, could query actual rate */
}

/*=============================================================================
 * INPUT SYSTEM COMPATIBILITY
 *===========================================================================*/

/* Installation functions - return 0 on success, -1 on failure (like A4) */
inline int install_keyboard() {
    return al_install_keyboard() ? 0 : -1;
}

inline int install_mouse() {
    if (!al_install_mouse()) return -1;
    return al_get_mouse_num_buttons(); /* Return button count like A4 */
}

/* Mouse cursor control */
#define MOUSE_CURSOR_NONE 0
#define MOUSE_CURSOR_ALLEGRO 1

inline void show_os_cursor(int cursor) {
    if (cursor == MOUSE_CURSOR_NONE) {
        al_hide_mouse_cursor(_tlc_display);
    } else {
        al_show_mouse_cursor(_tlc_display);
    }
}

/* scancode_to_ascii - convert keycode to ASCII (simplified version) */
inline int scancode_to_ascii(int scancode) {
    // This is a simplified implementation
    // For full functionality, would need to track shift/caps state
    if (scancode >= ALLEGRO_KEY_A && scancode <= ALLEGRO_KEY_Z) {
        return 'a' + (scancode - ALLEGRO_KEY_A);
    }
    if (scancode >= ALLEGRO_KEY_0 && scancode <= ALLEGRO_KEY_9) {
        return '0' + (scancode - ALLEGRO_KEY_0);
    }
    if (scancode == ALLEGRO_KEY_SPACE) return ' ';
    return 0;
}

/* Key code compatibility - map Allegro 4 key codes to Allegro 5 */
#define KEY_ENTER ALLEGRO_KEY_ENTER
#define KEY_ENTER_PAD ALLEGRO_KEY_PAD_ENTER
#define KEY_ESC ALLEGRO_KEY_ESCAPE
#define KEY_SPACE ALLEGRO_KEY_SPACE
#define KEY_TAB ALLEGRO_KEY_TAB
#define KEY_BACKSPACE ALLEGRO_KEY_BACKSPACE
#define KEY_DEL ALLEGRO_KEY_DELETE
#define KEY_DEL_PAD ALLEGRO_KEY_PAD_DELETE
#define KEY_UP ALLEGRO_KEY_UP
#define KEY_DOWN ALLEGRO_KEY_DOWN
#define KEY_LEFT ALLEGRO_KEY_LEFT
#define KEY_RIGHT ALLEGRO_KEY_RIGHT
#define KEY_LCONTROL ALLEGRO_KEY_LCTRL
#define KEY_RCONTROL ALLEGRO_KEY_RCTRL

/* Number keys */
#define KEY_0 ALLEGRO_KEY_0
#define KEY_1 ALLEGRO_KEY_1
#define KEY_2 ALLEGRO_KEY_2
#define KEY_3 ALLEGRO_KEY_3
#define KEY_4 ALLEGRO_KEY_4
#define KEY_5 ALLEGRO_KEY_5
#define KEY_6 ALLEGRO_KEY_6
#define KEY_7 ALLEGRO_KEY_7
#define KEY_8 ALLEGRO_KEY_8
#define KEY_9 ALLEGRO_KEY_9

/* Numpad keys */
#define KEY_0_PAD ALLEGRO_KEY_PAD_0
#define KEY_1_PAD ALLEGRO_KEY_PAD_1
#define KEY_2_PAD ALLEGRO_KEY_PAD_2
#define KEY_3_PAD ALLEGRO_KEY_PAD_3
#define KEY_4_PAD ALLEGRO_KEY_PAD_4
#define KEY_5_PAD ALLEGRO_KEY_PAD_5
#define KEY_6_PAD ALLEGRO_KEY_PAD_6
#define KEY_7_PAD ALLEGRO_KEY_PAD_7
#define KEY_8_PAD ALLEGRO_KEY_PAD_8
#define KEY_9_PAD ALLEGRO_KEY_PAD_9
#define KEY_PLUS_PAD ALLEGRO_KEY_PAD_PLUS
#define KEY_MINUS_PAD ALLEGRO_KEY_PAD_MINUS

/* Function keys */
#define KEY_F1 ALLEGRO_KEY_F1
#define KEY_F2 ALLEGRO_KEY_F2
#define KEY_F3 ALLEGRO_KEY_F3
#define KEY_F4 ALLEGRO_KEY_F4
#define KEY_F5 ALLEGRO_KEY_F5
#define KEY_F6 ALLEGRO_KEY_F6
#define KEY_F7 ALLEGRO_KEY_F7
#define KEY_F8 ALLEGRO_KEY_F8
#define KEY_F9 ALLEGRO_KEY_F9
#define KEY_F10 ALLEGRO_KEY_F10
#define KEY_F11 ALLEGRO_KEY_F11
#define KEY_F12 ALLEGRO_KEY_F12

/* Letter keys */
#define KEY_A ALLEGRO_KEY_A
#define KEY_B ALLEGRO_KEY_B
#define KEY_C ALLEGRO_KEY_C
#define KEY_D ALLEGRO_KEY_D
#define KEY_E ALLEGRO_KEY_E
#define KEY_F ALLEGRO_KEY_F
#define KEY_G ALLEGRO_KEY_G
#define KEY_H ALLEGRO_KEY_H
#define KEY_I ALLEGRO_KEY_I
#define KEY_J ALLEGRO_KEY_J
#define KEY_K ALLEGRO_KEY_K
#define KEY_L ALLEGRO_KEY_L
#define KEY_M ALLEGRO_KEY_M
#define KEY_N ALLEGRO_KEY_N
#define KEY_O ALLEGRO_KEY_O
#define KEY_P ALLEGRO_KEY_P
#define KEY_Q ALLEGRO_KEY_Q
#define KEY_R ALLEGRO_KEY_R
#define KEY_S ALLEGRO_KEY_S
#define KEY_T ALLEGRO_KEY_T
#define KEY_U ALLEGRO_KEY_U
#define KEY_V ALLEGRO_KEY_V
#define KEY_W ALLEGRO_KEY_W
#define KEY_X ALLEGRO_KEY_X
#define KEY_Y ALLEGRO_KEY_Y
#define KEY_Z ALLEGRO_KEY_Z

/* Shift keys */
#define KEY_LSHIFT ALLEGRO_KEY_LSHIFT
#define KEY_RSHIFT ALLEGRO_KEY_RSHIFT

/* Alt keys */
#define KEY_ALT ALLEGRO_KEY_ALT
#define KEY_ALTGR ALLEGRO_KEY_ALTGR

/* Page navigation */
#define KEY_PGUP ALLEGRO_KEY_PGUP
#define KEY_PGDN ALLEGRO_KEY_PGDN
#define KEY_HOME ALLEGRO_KEY_HOME
#define KEY_END ALLEGRO_KEY_END

/*=============================================================================
 * FILE SYSTEM COMPATIBILITY
 *===========================================================================*/

/* File attribute flags (A4 style) */
#define FA_RDONLY  1
#define FA_HIDDEN  2
#define FA_SYSTEM  4
#define FA_LABEL   8
#define FA_DIREC   16
#define FA_ARCH    32
#define FA_ALL     (~FA_LABEL)

/* Check if file exists - A4: file_exists(path, attrib, aret)
 * In A5 we just check if the file can be opened */
inline int file_exists(const char *path, int attrib, int *aret) {
    (void)attrib; (void)aret;
    return al_filename_exists(path) ? -1 : 0; /* A4 returns non-zero if exists */
}

/* exists() - simple file existence check */
inline bool exists(const char *path) {
    return al_filename_exists(path);
}

/* delete_file() - delete a file */
inline int delete_file(const char *path) {
    return al_remove_filename(path) ? 0 : -1;
}

#endif /* !TLC_USING_ALLEGRO_LEGACY */

/*=============================================================================
 * GLOBAL STATE DEFINITIONS (needed by both modes)
 *===========================================================================*/

#ifndef TLC_USING_ALLEGRO_LEGACY
/* These need to be defined in one .cpp file when building without Allegro Legacy:
 * 
 * ALLEGRO_BITMAP *_tlc_screen = NULL;
 * ALLEGRO_DISPLAY *_tlc_display = NULL;
 * int SCREEN_W = 0, SCREEN_H = 0;
 * int mouse_x = 0, mouse_y = 0, mouse_b = 0;
 * ALLEGRO_KEYBOARD_STATE _tlc_keyboard_state;
 * ALLEGRO_MOUSE_STATE _tlc_mouse_state;
 */
#endif

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
 * TRANSPARENT COLOR HANDLING HELPERS
 *===========================================================================*/

/*
 * Transparent color migration helper
 * 
 * In Allegro 4, magenta (255,0,255) was commonly used as a transparency mask.
 * In Allegro 5, transparency uses alpha channel.
 * 
 * This helper converts magenta pixels to transparent alpha after loading.
 * Call this immediately after loading any bitmap that uses magenta transparency.
 * 
 * Example usage:
 *   ALLEGRO_BITMAP *sprite = al_load_bitmap("sprite.bmp");
 *   tlc_convert_magenta_to_alpha(sprite);
 */
inline void tlc_convert_magenta_to_alpha(ALLEGRO_BITMAP *bitmap)
{
    if (!bitmap) {
        return;
    }

    ALLEGRO_LOCKED_REGION *lock = al_lock_bitmap(bitmap, ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE, ALLEGRO_LOCK_READWRITE);
    if (lock) {
        unsigned char *row = (unsigned char *)lock->data;
        int width = al_get_bitmap_width(bitmap);
        int height = al_get_bitmap_height(bitmap);
        for (int y = 0; y < height; ++y) {
            unsigned char *px = row;
            for (int x = 0; x < width; ++x) {
                unsigned char r = px[0];
                unsigned char g = px[1];
                unsigned char b = px[2];
                if (r == 255 && g == 0 && b == 255) {
                    px[3] = 0;
                }
                px += 4;
            }
            row += lock->pitch;
        }
        al_unlock_bitmap(bitmap);
        return;
    }

    al_convert_mask_to_alpha(bitmap, al_map_rgb(255, 0, 255));
}

/*
 * Create a transparent bitmap (replaces clear_to_color with magenta)
 * 
 * In Allegro 4: clear_to_color(bmp, makecol(255,0,255)) created transparency
 * In Allegro 5: clear to fully transparent alpha channel
 * 
 * Example usage:
 *   ALLEGRO_BITMAP *temp = al_create_bitmap(width, height);
 *   tlc_clear_to_transparent(temp);
 */
inline void tlc_clear_to_transparent(ALLEGRO_BITMAP *bitmap)
{
    if (bitmap) {
        ALLEGRO_BITMAP *old = al_get_target_bitmap();
        al_set_target_bitmap(bitmap);
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
        al_set_target_bitmap(old);
    }
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
