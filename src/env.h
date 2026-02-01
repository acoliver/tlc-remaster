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
    // On Windows with MSVC, we need to define ALLEGRO_LEGACY_MSVC before including
    // allegro.h so that alconfig.h selects the correct platform header (almsvc.h).
    // almsvc.h sets up ALLEGRO_LEGACY_WINDOWS and the magic main handling.
    // The generated alplatf.h from the Allegro Legacy build may not have this
    // defined correctly since it's generated based on Allegro Legacy's build
    // environment, not ours.
    #if defined(_MSC_VER) && !defined(ALLEGRO_LEGACY_MSVC)
        #define ALLEGRO_LEGACY_MSVC 1
    #endif
    #include <allegro.h>
    // allegro.h defines 'main' as '_mangled_main' and END_OF_MAIN() creates WinMain.
#else
    // Ensure we use the repo's Allegro Legacy headers (not any vendored headers
    // under src/build/include).
    #include "allegro.h"
    #define MessageBox(hwnd, text, caption, type) allegro_message("%s", text)
#endif

#endif // ENV_H
