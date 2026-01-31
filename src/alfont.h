#ifndef ALFONT_H
#define ALFONT_H

#include <allegro.h>

#ifndef ALFONT_OK
    #define ALFONT_OK 0
#endif

// AlFont API compatibility layer.
//
// This project historically used AlFont (Allegro 4 TTF extension). We now
// provide an implementation backed by Allegro 5's font/ttf addons, drawing to
// Allegro 4 BITMAP* via AllegroLegacy's all_get_a5_bitmap().

typedef struct ALFONT_FONT ALFONT_FONT;

int alfont_init(void);
void alfont_exit(void);

ALFONT_FONT* alfont_load_font(const char* filename);
void alfont_destroy_font(ALFONT_FONT* font);

int alfont_set_font_size(ALFONT_FONT* font, int size);
int alfont_text_height(ALFONT_FONT* font);
int alfont_get_font_height(ALFONT_FONT* font);

int alfont_text_length(ALFONT_FONT* font, const char* text);

void alfont_textout(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color);
void alfont_textout_ex(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color, int bg);
void alfont_textout_centre(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color);
void alfont_textout_centre_ex(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color, int bg);
void alfont_textout_right(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color);
void alfont_textout_right_ex(BITMAP* bmp, ALFONT_FONT* font, const char* text, int x, int y, int color, int bg);

void alfont_textprintf(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, const char* format, ...);
void alfont_textprintf_ex(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, int bg, const char* format, ...);
void alfont_textprintf_right_ex(BITMAP* bmp, ALFONT_FONT* font, int x, int y, int color, int bg, const char* format, ...);

#endif // ALFONT_H
