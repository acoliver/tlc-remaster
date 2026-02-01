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
    // 
    // CRITICAL: alconfig.h includes alplatf.h FIRST (line 40), which is a generated
    // file from the Allegro Legacy build. When Allegro Legacy is built with MSVC,
    // it SHOULD define ALLEGRO_LEGACY_MSVC in alplatf.h (see line 236 of its
    // CMakeLists.txt). The generated alplatf.h should have:
    //   #define ALLEGRO_LEGACY_MSVC
    // not:
    //   /* #undef ALLEGRO_LEGACY_MSVC */
    //
    // If the generated alplatf.h doesn't have ALLEGRO_LEGACY_MSVC defined, it means
    // the Allegro Legacy CMake didn't detect MSVC correctly. We define it here as
    // a fallback, but the proper fix is to ensure Allegro Legacy is built correctly.
    #if defined(_MSC_VER)
        #ifndef ALLEGRO_LEGACY_MSVC
            #define ALLEGRO_LEGACY_MSVC 1
        #endif
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
