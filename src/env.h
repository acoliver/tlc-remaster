#ifndef ENV_H
#define ENV_H 1

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define TLC_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
    #define TLC_PLATFORM_MACOS 1
#elif defined(__linux__)
    #define TLC_PLATFORM_LINUX 1
#endif

// Allegro Legacy detection
#ifdef ALLEGRO_LEGACY
    #define TLC_USING_ALLEGRO_LEGACY 1
#endif

// MessageBox fallback for non-Windows
#ifdef TLC_PLATFORM_WINDOWS
    // On Windows, include allegro.h which will set up the magic main properly.
    // allegro.h must be included before any other code that uses main().
    #include <allegro.h>
    // allegro.h defines 'main' as '_mangled_main' and END_OF_MAIN() creates WinMain.
#else
    // Ensure we use the repo's Allegro Legacy headers (not any vendored headers
    // under src/build/include).
    #include "allegro.h"
    #define MessageBox(hwnd, text, caption, type) allegro_message("%s", text)
#endif

#endif // ENV_H
