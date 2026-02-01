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

// On Windows with MSVC, we need to define ALLEGRO_LEGACY_MSVC before including
// ANY Allegro Legacy headers. This is because:
//
// 1. allegro/base.h includes allegro/internal/alconfig.h
// 2. alconfig.h includes allegro/platform/alplatf.h (generated file) - ONLY if
//    SCAN_DEPEND is NOT defined
// 3. alconfig.h then checks for ALLEGRO_LEGACY_MSVC to decide which platform
//    header to include (line 53: #elif defined ALLEGRO_LEGACY_MSVC) - but this
//    is INSIDE the #ifndef SCAN_EXPORT block!
// 4. If ALLEGRO_LEGACY_MSVC is set, it includes almsvc.h which defines
//    ALLEGRO_LEGACY_WINDOWS (line 37)
// 5. almsvc.h sets ALLEGRO_LEGACY_EXTRA_HEADER to "allegro/platform/alwin.h"
// 6. allegro.h includes ALLEGRO_LEGACY_EXTRA_HEADER at the end
// 7. alwin.h sets up the "magic main" (redefines main to _mangled_main)
//
// PROBLEM: SCAN_DEPEND skips ALL platform header selection, not just alplatf.h!
// The entire platform-specific block (lines 38-76) is skipped when SCAN_DEPEND
// is defined, which means almsvc.h is never included!
//
// SOLUTION: We define ALLEGRO_LEGACY_MSVC which SHOULD be picked up by the
// generated alplatf.h when Allegro Legacy is built with MSVC. If it's not
// defined there, we'll get the "platform not supported" error because no
// platform header is selected.
//
// The real fix is ensuring the Allegro Legacy build properly defines
// ALLEGRO_LEGACY_MSVC in alplatf.h when compiled with MSVC.
#if defined(TLC_PLATFORM_WINDOWS) && defined(_MSC_VER)
    // Define this BEFORE allegro.h so alconfig.h sees it
    // Note: alplatf.h SHOULD have this defined too (from the Allegro Legacy build)
    #ifndef ALLEGRO_LEGACY_MSVC
        #define ALLEGRO_LEGACY_MSVC 1
    #endif
#endif

#include <allegro.h>

// Allegro Legacy detection
#ifdef ALLEGRO_LEGACY
    #define TLC_USING_ALLEGRO_LEGACY 1
#endif

// MessageBox fallback for non-Windows
#ifndef TLC_PLATFORM_WINDOWS
    #define MessageBox(hwnd, text, caption, type) allegro_message("%s", text)
#endif

#endif // ENV_H
