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
// 2. alconfig.h includes allegro/platform/alplatf.h (generated file)
// 3. alconfig.h then checks for ALLEGRO_LEGACY_MSVC to decide which platform
//    header to include (line 53: #elif defined ALLEGRO_LEGACY_MSVC)
// 4. If ALLEGRO_LEGACY_MSVC is set, it includes almsvc.h which defines
//    ALLEGRO_LEGACY_WINDOWS (line 37)
// 5. almsvc.h sets ALLEGRO_LEGACY_EXTRA_HEADER to "allegro/platform/alwin.h"
// 6. allegro.h includes ALLEGRO_LEGACY_EXTRA_HEADER at the end
// 7. alwin.h sets up the "magic main" (redefines main to _mangled_main)
//
// IMPORTANT: The define must happen BEFORE #include "allegro/base.h" is processed,
// which means before allegro.h is included. We use SCAN_DEPEND to skip alplatf.h
// inclusion so our define takes precedence.
#if defined(TLC_PLATFORM_WINDOWS) && defined(_MSC_VER)
    // Skip alplatf.h so our defines take effect
    #define SCAN_DEPEND 1
    #define ALLEGRO_LEGACY_MSVC 1
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
