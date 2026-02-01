#include "alfont.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <a5alleg.h>

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

// Allegro 4 BITMAP* -> Allegro 5 ALLEGRO_BITMAP* bridge.
// a5alleg.h comes from the AllegroLegacy::allegrolegacy target.

struct ALFONT_FONT {
    std::string filename;
    int size;
    ALLEGRO_FONT* a5_font;
};

static ALLEGRO_COLOR a4_color_to_a5(int a4_color)
{
    // Allegro 4 encodes colors according to the current color depth.
    // Use the "_depth" accessors so this works correctly regardless of mode.
    const int bpp = bitmap_color_depth(screen);
    const int r = getr_depth(bpp, a4_color);
    const int g = getg_depth(bpp, a4_color);
    const int b = getb_depth(bpp, a4_color);
    return al_map_rgb(r, g, b);
}

static void alfont_reload_if_needed(ALFONT_FONT* f)
{
    if (!f) {
        return;
    }

    if (f->a5_font) {
        return;
    }

    // Use negative size to specify pixel height (not points)
    // This gives more consistent sizing across different DPI settings
    f->a5_font = al_load_ttf_font(f->filename.c_str(), -f->size, 0);
}

int alfont_init(void)
{
    if (!al_init_font_addon()) {
        return -1;
    }

    if (!al_init_ttf_addon()) {
        return -1;
    }

    return ALFONT_OK;
}

void alfont_exit(void)
{
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
}

ALFONT_FONT* alfont_load_font(const char* filename)
{
    if (!filename) {
        return nullptr;
    }

    ALFONT_FONT* f = new ALFONT_FONT();
    f->filename = filename;
    f->size = 10;
    f->a5_font = nullptr;

    alfont_reload_if_needed(f);
    if (!f->a5_font) {
        delete f;
        return nullptr;
    }

    return f;
}

void alfont_destroy_font(ALFONT_FONT* font)
{
    if (!font) {
        return;
    }

    if (font->a5_font) {
        al_destroy_font(font->a5_font);
        font->a5_font = nullptr;
    }

    delete font;
}

int alfont_set_font_size(ALFONT_FONT* font, int size)
{
    if (!font || size <= 0) {
        return 0;
    }

    // The original AlFont used a different sizing model than Allegro 5's TTF loader.
    // Allegro 5 TTF fonts use pixel height directly, but the original game's fonts
    // were designed for a specific scaling. We need to use the size as-is since
    // the game was designed around these specific point sizes.
    int adjusted_size = size;

    if (font->size == adjusted_size && font->a5_font) {
        return 1;
    }

    font->size = adjusted_size;
    if (font->a5_font) {
        al_destroy_font(font->a5_font);
        font->a5_font = nullptr;
    }

    alfont_reload_if_needed(font);
    return font->a5_font ? 1 : 0;
}

int alfont_text_height(ALFONT_FONT* font)
{
    if (!font) {
        return 0;
    }

    alfont_reload_if_needed(font);
    return font->a5_font ? al_get_font_line_height(font->a5_font) : 0;
}

int alfont_get_font_height(ALFONT_FONT* font)
{
    return alfont_text_height(font);
}

int alfont_text_length(ALFONT_FONT* font, const char* text)
{
    if (!font || !text) {
        return 0;
    }

    alfont_reload_if_needed(font);
    return font->a5_font ? (int)al_get_text_width(font->a5_font, text) : 0;
}

static void alfont_draw_text_common(BITMAP* bmp, ALFONT_FONT* font, const char* text,
    int x, int y, int color, int bg, int flags)
{
    if (!bmp || !font || !text) {
        return;
    }

    alfont_reload_if_needed(font);
    if (!font->a5_font) {
        return;
    }

    // Draw text directly onto the Allegro 4 bitmap's pixel buffer
    // We need to use Allegro 4's textout functions or draw to a temp A5 bitmap
    // and blit the result back.
    
    // Create a temporary A5 bitmap for rendering
    const int text_w = (int)al_get_text_width(font->a5_font, text) + 4;
    const int text_h = (int)al_get_font_line_height(font->a5_font) + 2;
    
    if (text_w <= 0 || text_h <= 0) {
        return;
    }
    
    ALLEGRO_BITMAP* temp = al_create_bitmap(text_w, text_h);
    if (!temp) {
        return;
    }
    
    ALLEGRO_BITMAP* prev = al_get_target_bitmap();
    al_set_target_bitmap(temp);
    
    // Clear with transparent or background color
    if (bg != -1) {
        al_clear_to_color(a4_color_to_a5(bg));
    } else {
        al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    }
    
    // Calculate draw position based on alignment
    float draw_x = 0;
    if (flags & ALLEGRO_ALIGN_CENTRE) {
        draw_x = text_w / 2.0f;
    } else if (flags & ALLEGRO_ALIGN_RIGHT) {
        draw_x = (float)(text_w - 2);
    }
    
    al_draw_text(font->a5_font, a4_color_to_a5(color), draw_x, 0, flags, text);
    
    al_set_target_bitmap(prev);
    
    // Now we need to copy the A5 bitmap pixels to the A4 bitmap
    // Lock the A5 bitmap to read pixels
    ALLEGRO_LOCKED_REGION* lock = al_lock_bitmap(temp, ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE, ALLEGRO_LOCK_READONLY);
    if (lock) {
        // Calculate destination position based on alignment
        int dest_x = x;
        if (flags & ALLEGRO_ALIGN_CENTRE) {
            dest_x = x - text_w / 2;
        } else if (flags & ALLEGRO_ALIGN_RIGHT) {
            dest_x = x - text_w + 2;
        }
        
        // Copy pixels to A4 bitmap
        unsigned char* src_row = (unsigned char*)lock->data;
        for (int py = 0; py < text_h; py++) {
            unsigned char* src = src_row;
            for (int px = 0; px < text_w; px++) {
                unsigned char r = src[0];
                unsigned char g = src[1];
                unsigned char b = src[2];
                unsigned char a = src[3];
                src += 4;
                
                // Only draw non-transparent pixels
                if (a > 128) {
                    int bmp_x = dest_x + px;
                    int bmp_y = y + py;
                    if (bmp_x >= 0 && bmp_x < bmp->w && bmp_y >= 0 && bmp_y < bmp->h) {
                        putpixel(bmp, bmp_x, bmp_y, makecol(r, g, b));
                    }
                }
            }
            src_row += lock->pitch;
        }
        al_unlock_bitmap(temp);
    }
    
    al_destroy_bitmap(temp);
}

void alfont_textout(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color)
{
    alfont_draw_text_common(bmp, font, text, x, y, color, -1, 0);
}

void alfont_textout_ex(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color, int bg)
{
    alfont_draw_text_common(bmp, font, text, x, y, color, bg, 0);
}

void alfont_textout_centre(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color)
{
    alfont_draw_text_common(bmp, font, text, x, y, color, -1, ALLEGRO_ALIGN_CENTRE);
}

void alfont_textout_centre_ex(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color, int bg)
{
    alfont_draw_text_common(bmp, font, text, x, y, color, bg, ALLEGRO_ALIGN_CENTRE);
}

void alfont_textout_right(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color)
{
    alfont_draw_text_common(bmp, font, text, x, y, color, -1, ALLEGRO_ALIGN_RIGHT);
}

void alfont_textout_right_ex(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color, int bg)
{
    alfont_draw_text_common(bmp, font, text, x, y, color, bg, ALLEGRO_ALIGN_RIGHT);
}

static void alfont_vprintf(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, int bg, int flags, const char* format, va_list args)
{
    if (!format) {
        return;
    }

    char buf[4096];
    vsnprintf(buf, sizeof(buf), format, args);
    buf[sizeof(buf) - 1] = '\0';

    alfont_draw_text_common(bmp, font, buf, x, y, color, bg, flags);
}

void alfont_textprintf(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    alfont_vprintf(bmp, font, x, y, color, -1, 0, format, args);
    va_end(args);
}

void alfont_textprintf_ex(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, int bg, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    alfont_vprintf(bmp, font, x, y, color, bg, 0, format, args);
    va_end(args);
}

void alfont_textprintf_right_ex(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, int bg, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    alfont_vprintf(bmp, font, x, y, color, bg, ALLEGRO_ALIGN_RIGHT, format, args);
    va_end(args);
}
