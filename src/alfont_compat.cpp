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

    f->a5_font = al_load_ttf_font(f->filename.c_str(), f->size, 0);
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

    if (font->size == size && font->a5_font) {
        return 1;
    }

    font->size = size;
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

    ALLEGRO_BITMAP* a5_bmp = all_get_a5_bitmap(bmp);
    if (!a5_bmp) {
        return;
    }

    ALLEGRO_BITMAP* prev = al_get_target_bitmap();
    al_set_target_bitmap(a5_bmp);

    // Background fill is only used when bg != -1 (matches common AlFont usage).
    if (bg != -1) {
        const int w = (int)al_get_text_width(font->a5_font, text);
        const int h = (int)al_get_font_line_height(font->a5_font);
        al_draw_filled_rectangle((float)x, (float)y, (float)(x + w), (float)(y + h), a4_color_to_a5(bg));
    }

    al_draw_text(font->a5_font, a4_color_to_a5(color), (float)x, (float)y, flags, text);

    al_set_target_bitmap(prev);
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
