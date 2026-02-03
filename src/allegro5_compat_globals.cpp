/*
 * Allegro 5 Compatibility Layer - Global State
 * 
 * This file defines the global variables needed by allegro5_compat.h
 * when building in native Allegro 5 mode (without Allegro Legacy).
 * 
 * Only compiled when TLC_NATIVE_ALLEGRO5 is defined.
 */

#include "env.h"

#ifdef TLC_NATIVE_ALLEGRO5

/* Display and screen globals */
ALLEGRO_BITMAP *_tlc_screen = nullptr;
ALLEGRO_DISPLAY *_tlc_display = nullptr;
int SCREEN_W = 0;
int SCREEN_H = 0;

/* Input state globals */
int mouse_x = 0;
int mouse_y = 0;
int mouse_b = 0;
ALLEGRO_KEYBOARD_STATE _tlc_keyboard_state;
ALLEGRO_MOUSE_STATE _tlc_mouse_state;
bool _tlc_key[ALLEGRO_KEY_MAX] = {false};

#endif /* TLC_NATIVE_ALLEGRO5 */
