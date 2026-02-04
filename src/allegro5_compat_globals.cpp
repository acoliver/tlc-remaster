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

#endif /* TLC_NATIVE_ALLEGRO5 */
