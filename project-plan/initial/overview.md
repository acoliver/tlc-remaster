# Starflight: The Lost Colony - macOS/Linux Port Project Plan

## Executive Summary

This document outlines the work required to port **Starflight: The Lost Colony (TLC Remaster)** from its current Windows/Visual Studio 2010 build to a modern cross-platform build targeting **macOS** and **Linux** using **C++17** and **Allegro 5**.

**Original Codebase Stats:**
- 85 C++ source files
- 217 header files  
- ~50,000+ lines of code (estimated)
- Last updated: 2017-2018

---

## 1. Current Dependencies

| Library | Version | Purpose | Windows Files |
|---------|---------|---------|---------------|
| **Allegro** | 4.2 | Graphics, input, timers | `alleg42.dll`, `alld42.dll` |
| **AlFont** | 2.0.9 | TrueType font rendering | `alfont.dll` |
| **FMOD** | Ex 4.x | Audio playback | `fmodex.dll` |
| **libnoise** | 1.0 | Procedural terrain generation | `libnoise.dll` |
| **Lua** | 5.1 | Scripting engine | `lua5.1.dll` |
| **TinyXML** | 2.x | XML parsing | (bundled in source) |

---

## 2. Target Dependencies (Replacement Plan)

| Original | Replacement | Rationale |
|----------|-------------|-----------|
| Allegro 4.2 | **Allegro 5.2.10+** | Modern, cross-platform, maintained |
| AlFont | **Allegro 5 Font Addon** | Built into Allegro 5 |
| FMOD Ex | **Allegro 5 Audio Addon** or **FMOD Core** | Allegro audio is simpler; FMOD Core if advanced features needed |
| libnoise | **libnoise** (recompile) | Same library, just need macOS/Linux build |
| Lua 5.1 | **Lua 5.1** (Homebrew/apt) | Same version, system install |
| TinyXML | **TinyXML** (bundled) | Already in source, minimal changes |

---

## 3. Code Changes Required

### 3.1 Build System (NEW)

**Task:** Create CMake build system

- [ ] Create root `CMakeLists.txt`
- [ ] Create `src/CMakeLists.txt` for game sources
- [ ] Configure find modules for dependencies
- [ ] Add install targets for game data
- [ ] Support Debug/Release configurations

**Estimated effort:** 4-6 hours

---

### 3.2 Allegro 4 → Allegro 5 Migration

This is the **largest task**. The APIs are significantly different.

#### 3.2.1 Type Replacements

| Allegro 4 | Allegro 5 | Occurrences |
|-----------|-----------|-------------|
| `BITMAP*` | `ALLEGRO_BITMAP*` | **254+** (in headers alone) |
| `PALETTE` | N/A (not needed) | ~5 |
| `ALFONT_FONT*` | `ALLEGRO_FONT*` | **141** uses |
| `SAMPLE*` | `ALLEGRO_SAMPLE*` | N/A (using FMOD) |

#### 3.2.2 Function Replacements

| Allegro 4 Function | Allegro 5 Equivalent | Count |
|--------------------|----------------------|-------|
| `makecol(r,g,b)` | `al_map_rgb(r,g,b)` | **136** |
| `load_bitmap()` | `al_load_bitmap()` | **324** (load/create/destroy) |
| `create_bitmap()` | `al_create_bitmap()` | (included above) |
| `destroy_bitmap()` | `al_destroy_bitmap()` | (included above) |
| `blit()` | `al_draw_bitmap()` | ~100+ |
| `stretch_blit()` | `al_draw_scaled_bitmap()` | ~20 |
| `masked_blit()` | `al_draw_bitmap()` (with blending) | ~30 |
| `rectfill()` | `al_draw_filled_rectangle()` | ~50 |
| `line()` | `al_draw_line()` | ~20 |
| `circle()` / `circlefill()` | `al_draw_circle()` / `al_draw_filled_circle()` | ~15 |
| `ellipse()` | `al_draw_ellipse()` | ~5 |
| `putpixel()` / `getpixel()` | `al_put_pixel()` / `al_get_pixel()` | ~30 |
| `clear_to_color()` | `al_clear_to_color()` | ~30 |
| `allegro_init()` | `al_init()` | 1 |
| `install_keyboard()` | `al_install_keyboard()` | 1 |
| `install_mouse()` | `al_install_mouse()` | 1 |
| `install_timer()` | `al_install_timer()` | 1 |
| `set_gfx_mode()` | `al_create_display()` | 1 |
| `key[]` array | `al_key_down()` / event system | ~50 |
| `mouse_x`, `mouse_y` | `al_get_mouse_state()` | ~30 |
| `END_OF_MAIN()` | Remove entirely | 1 |

#### 3.2.3 AlFont → Allegro 5 Font Addon

| AlFont Function | Allegro 5 Equivalent | Count |
|-----------------|----------------------|-------|
| `alfont_init()` | `al_init_font_addon()` + `al_init_ttf_addon()` | 1 |
| `alfont_load_font()` | `al_load_font()` / `al_load_ttf_font()` | ~7 |
| `alfont_destroy_font()` | `al_destroy_font()` | 7 |
| `alfont_set_font_size()` | Load font at specific size | ~15 |
| `alfont_textout*()` | `al_draw_text()` | **141** |
| `alfont_text_length()` | `al_get_text_width()` | ~10 |
| `alfont_get_font_height()` | `al_get_font_line_height()` | ~5 |

#### 3.2.4 Event System (Major Architecture Change)

Allegro 5 uses an **event-driven model** instead of polling:

```cpp
// Allegro 4 (polling)
if (key[KEY_SPACE]) { ... }
if (mouse_b & 1) { ... }

// Allegro 5 (events)
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
al_register_event_source(queue, al_get_keyboard_event_source());
ALLEGRO_EVENT event;
while (al_get_next_event(queue, &event)) {
    if (event.type == ALLEGRO_EVENT_KEY_DOWN) { ... }
}
```

**Files heavily affected:**
- `Game.cpp` - Main loop, input handling (~1700 lines)
- `Module.cpp` - Event dispatching
- All `Module*.cpp` files - Input handling in each module

**Estimated effort for Allegro migration:** 40-60 hours

---

### 3.3 Audio System (FMOD)

**Options:**

1. **Keep FMOD** - Get FMOD Core SDK for macOS/Linux
   - Pros: Minimal code changes
   - Cons: Proprietary, licensing considerations

2. **Replace with Allegro 5 Audio Addon**
   - Pros: One less dependency, open source
   - Cons: More code changes (~430 lines in AudioSystem.cpp)

**Current FMOD usage:**
- `AudioSystem.cpp` - 430 lines
- `AudioSystem.h` - 60 lines
- FMOD C API calls throughout

**Estimated effort:** 
- FMOD keep: 2-4 hours (library setup only)
- Allegro audio migration: 8-12 hours

---

### 3.4 Platform-Specific Code Removal

#### 3.4.1 Windows-Specific Constructs

| Item | Location | Action |
|------|----------|--------|
| `#pragma comment(lib, ...)` | `env.cpp` | Remove (CMake handles linking) |
| `#pragma region` | `Game.cpp` | Remove (MSVC-only) |
| `END_OF_MAIN()` | `main.cpp` | Remove |
| `_MSC_VER` checks | `env.h`, `Timer.cpp` | Update for cross-platform |
| `WIN32` checks | Various | Update or remove |
| `MessageBox()` | `env.h` | Replace with `al_show_native_message_box()` |

#### 3.4.2 Timer.cpp

Already has POSIX support via `#if defined(_POSIX_SOURCE)` - needs verification on macOS.

**Estimated effort:** 2-4 hours

---

### 3.5 libnoise Integration

The game uses libnoise for procedural planet generation.

- Source available at: http://libnoise.sourceforge.net/
- Need to compile for macOS/Linux
- Header-only option available via modern forks

**Files affected:**
- `noiseutils.cpp` / `noiseutils.h`
- `TexturedSphere.cpp`
- `PlanetaryBody.cpp`

**Estimated effort:** 2-4 hours

---

### 3.6 Lua Integration

Lua 5.1 is readily available:
- macOS: `brew install lua@5.1`
- Linux: `apt install liblua5.1-dev`

**Files affected:**
- `Script.cpp` / `Script.h`
- Include path changes only

**Estimated effort:** 1-2 hours

---

### 3.7 TinyXML

Already bundled in `src/tinyxml/`. Should compile without changes on any platform.

**Estimated effort:** 0-1 hours (verification only)

---

### 3.8 File Path Handling

Windows uses backslashes, Unix uses forward slashes. The game loads many resources:

```cpp
load_bitmap("data/planetsurface/tileset_ash.tga", NULL);
```

**Action:** Verify all paths use forward slashes (they appear to already).

**Estimated effort:** 1-2 hours (audit and fix)

---

## 4. Optional Modernization (C++17)

These are **not required** for the port but would improve code quality:

| Change | Benefit | Effort |
|--------|---------|--------|
| `NULL` → `nullptr` | Type safety | Low |
| Raw pointers → smart pointers | Memory safety | High |
| Range-based for loops | Readability | Medium |
| `std::string_view` | Performance | Low |
| `std::filesystem` | Cross-platform paths | Medium |
| Remove `using namespace std` | Best practice | Low |

**Recommendation:** Defer to Phase 2 after port is working.

---

## 5. Testing Strategy

### 5.1 Build Verification
- [ ] Compiles without errors on macOS
- [ ] Compiles without errors on Linux (Ubuntu 22.04+)
- [ ] No linker errors

### 5.2 Runtime Verification
- [ ] Game launches and shows title screen
- [ ] Audio plays
- [ ] Keyboard/mouse input works
- [ ] Can start new game
- [ ] Can navigate menus
- [ ] Planet surface rendering works
- [ ] Save/load works

### 5.3 Game Modules to Test
1. Title Screen
2. Captain Creation
3. Starport
4. Starmap
5. Interstellar Travel
6. Solar System Navigation
7. Planet Orbit
8. Planet Surface
9. Encounters
10. Trading
11. Save/Load

---

## 6. Estimated Timeline

| Phase | Task | Effort (hours) |
|-------|------|----------------|
| 1 | CMake build system | 4-6 |
| 2 | Allegro 5 compatibility layer/header | 8-12 |
| 3 | Core Allegro 5 migration (Game.cpp, main.cpp) | 12-16 |
| 4 | Module migration (35+ files) | 20-30 |
| 5 | Font system migration | 4-6 |
| 6 | Audio system (keep FMOD or migrate) | 4-12 |
| 7 | libnoise/Lua integration | 4-6 |
| 8 | Platform code cleanup | 2-4 |
| 9 | Testing and debugging | 15-25 |
| **Total** | | **73-117 hours** |

---

## 7. Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Allegro 5 API differences break game logic | Medium | High | Create compatibility layer first |
| libnoise doesn't compile on macOS | Low | Medium | Use header-only fork or vcpkg |
| FMOD licensing issues | Low | Medium | Switch to Allegro audio |
| Undiscovered Windows dependencies | Medium | Medium | Incremental testing |
| Performance differences | Low | Low | Profile and optimize |

---

## 8. Deliverables

1. **CMakeLists.txt** - Cross-platform build system
2. **Allegro5Compat.h** - Compatibility macros/wrappers
3. **Updated source files** - All .cpp/.h files ported
4. **Build instructions** - README for macOS and Linux
5. **Test report** - Verification of game functionality

---

## 9. Open Questions

1. **FMOD vs Allegro Audio** - Which audio backend to use?
2. **C++ Standard** - Target C++17 or stay with C++11 for broader compatibility?
3. **Minimum OS versions** - macOS 10.15+? Ubuntu 20.04+?
4. **Package distribution** - DMG for macOS? AppImage for Linux?

---

## 10. File Inventory

### Source Files (85 total)

#### Core Engine
- `main.cpp` - Entry point
- `Game.cpp` / `Game.h` - Main game class (~1700 lines)
- `Module.cpp` / `Module.h` - Base module class
- `ModeMgr.cpp` / `ModeMgr.h` - Game state manager

#### Graphics/UI
- `Sprite.cpp` - Sprite rendering
- `Button.cpp` - UI buttons
- `Label.cpp` - Text labels
- `ScrollBox.cpp` - Scrollable lists
- `MiniWindow.cpp` - Window rendering
- `MessageBoxWindow.cpp` - Dialog boxes
- `PauseMenu.cpp` - Pause menu

#### Game Modules (35 files)
- `ModuleTitleScreen.cpp`
- `ModuleStartup.cpp`
- `ModuleCaptainCreation.cpp`
- `ModuleCaptainsLounge.cpp`
- `ModuleStarport.cpp`
- `ModuleStarmap.cpp`
- `ModuleInterstellar.cpp`
- `ModuleSolarSystem.cpp`
- `ModulePlanetOrbit.cpp`
- `ModulePlanetSurface.cpp`
- `ModuleEncounter.cpp`
- `ModuleTradeDepot.cpp`
- `ModuleBank.cpp`
- `ModuleCantina.cpp`
- `ModuleCrewHire.cpp`
- `ModuleMedical.cpp`
- `ModuleEngineer.cpp`
- `ModuleShipConfig.cpp`
- `ModuleQuestLog.cpp`
- `ModuleSettings.cpp`
- `ModuleCredits.cpp`
- `ModuleGameOver.cpp`
- `ModuleControlPanel.cpp`
- `ModuleTopGUI.cpp`
- `ModuleMessageGUI.cpp`
- `ModuleAuxiliaryDisplay.cpp`
- `ModuleSideViewer.cpp`
- `ModuleCargoWindow.cpp`
- `ModuleMiniGame.cpp`

#### Game Logic
- `Player.cpp` - Player state
- `GameState.cpp` - Save/load state
- `DataMgr.cpp` - Data management
- `QuestMgr.cpp` - Quest system
- `Script.cpp` - Lua integration
- `AudioSystem.cpp` - FMOD wrapper

#### Planet/Space
- `PlanetaryBody.cpp` - Planet generation
- `PlanetTileScroller.cpp` - Surface scrolling
- `PlanetSurfaceObject.cpp` - Surface objects
- `PlanetSurfacePlayerVessel.cpp` - Player vehicle
- `TexturedSphere.cpp` - 3D planet rendering
- `TileScroller.cpp` - Tile-based scrolling
- `OrbitPlanet.cpp` - Orbit mechanics

#### Utilities
- `Timer.cpp` - Time management
- `Util.cpp` - Helper functions
- `Math.cpp` - Math utilities
- `Vector3.cpp` - 3D vector math
- `Stardate.cpp` - Game calendar
- `LogFile.cpp` - Debug logging

#### Bundled Libraries
- `tinyxml/*.cpp` - XML parser (4 files)
- `noiseutils.cpp` - libnoise utilities

---

## Appendix A: Allegro 4 vs 5 Quick Reference

```cpp
// === INITIALIZATION ===
// Allegro 4:
allegro_init();
install_keyboard();
install_mouse();
install_timer();
set_color_depth(32);
set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);

// Allegro 5:
al_init();
al_install_keyboard();
al_install_mouse();
al_init_image_addon();
al_init_font_addon();
al_init_ttf_addon();
al_init_primitives_addon();
ALLEGRO_DISPLAY *display = al_create_display(1024, 768);

// === BITMAP OPERATIONS ===
// Allegro 4:
BITMAP *bmp = load_bitmap("image.bmp", NULL);
blit(src, dest, sx, sy, dx, dy, w, h);
destroy_bitmap(bmp);

// Allegro 5:
ALLEGRO_BITMAP *bmp = al_load_bitmap("image.bmp");
al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0);
al_destroy_bitmap(bmp);

// === COLORS ===
// Allegro 4:
int color = makecol(255, 128, 0);

// Allegro 5:
ALLEGRO_COLOR color = al_map_rgb(255, 128, 0);

// === DRAWING ===
// Allegro 4:
rectfill(bmp, x1, y1, x2, y2, color);
line(bmp, x1, y1, x2, y2, color);
circlefill(bmp, cx, cy, r, color);

// Allegro 5:
al_draw_filled_rectangle(x1, y1, x2, y2, color);
al_draw_line(x1, y1, x2, y2, color, 1.0);
al_draw_filled_circle(cx, cy, r, color);

// === TEXT ===
// Allegro 4 + AlFont:
ALFONT_FONT *font = alfont_load_font("font.ttf");
alfont_set_font_size(font, 24);
alfont_textout(bmp, font, "Hello", x, y, color);

// Allegro 5:
ALLEGRO_FONT *font = al_load_ttf_font("font.ttf", 24, 0);
al_draw_text(font, color, x, y, 0, "Hello");

// === INPUT (EVENT-BASED) ===
// Allegro 5:
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
al_register_event_source(queue, al_get_keyboard_event_source());
al_register_event_source(queue, al_get_mouse_event_source());
al_register_event_source(queue, al_get_display_event_source(display));

ALLEGRO_EVENT event;
while (al_get_next_event(queue, &event)) {
    switch (event.type) {
        case ALLEGRO_EVENT_KEY_DOWN:
            // event.keyboard.keycode
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            // event.mouse.x, event.mouse.y, event.mouse.button
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            // quit
            break;
    }
}
```

---

## 11. Expert Review Feedback

*Reviewed by: hardproblemcoder subagent (January 2026)*

### 11.1 Critical Issues (Must Address Before Starting)

1. **No reproducible dependency + CI strategy** - High risk of non-repeatable builds and slow iteration. Need to define whether using system packages, vcpkg, FetchContent, or git submodules.

2. **Underestimated semantic migration work** - Plan focuses on API name replacements but under-specifies semantic differences:
   - Allegro 4 BITMAP/palette model vs Allegro 5 target bitmap/backbuffer + GPU pipeline
   - Render-to-bitmap vs target bitmap switching
   - Blending modes and bitmap locking
   - Display/context lifetime management

3. **Testing/debugging time underestimated** - Lack of golden reference (screenshots/playthrough baseline) and continuous milestones increases schedule risk.

4. **FMOD decision not resolved early** - FMOD Ex is legacy; modern FMOD Core integration/distribution/licensing may be more complex than "2-4h" estimate.

5. **Font/text metric differences** - Can cause broad UI regressions beyond mechanical API swaps. Text width/height calculations may differ.

6. **Potential hidden Win32/MSVC runtime dependencies** - Not fully inventoried (e.g., binary serialization, struct packing, char signedness).

### 11.2 Missing Plan Elements

- **Dependency acquisition strategy**: System packages vs vcpkg/FetchContent/submodules
- **CI builds**: Automated macOS + Ubuntu builds
- **Runtime packaging**: 
  - macOS `.app` bundle layout and `@rpath` handling
  - Linux tar/AppImage/Flatpak options
- **Reproducible dev setup documentation**
- **QA strategy**: Golden-reference screenshots, sanitizer/leak checks (ASan/UBSan)
- **Continuous verification milestones**

### 11.3 Hidden/High Risks Not Originally Identified

| Risk | Notes |
|------|-------|
| Palette/8-bit assumptions | Original code may assume 8-bit color in places |
| Software→GPU rendering | Per-pixel operations (`putpixel`/`getpixel`) behave differently |
| Input edge behavior | Key repeat, focus loss handling differs |
| Timing/frame pacing | May cause gameplay feel regressions |
| FMOD SDK distribution | Licensing and bundling complexity |
| Text metrics/layout drift | UI elements may shift or overflow |
| Windows CRT/Win32 API assumptions | May be lurking in utility code |

### 11.4 Revised Effort Estimates

The original 73-117 hour estimate is **optimistic** for feature-parity port.

| Phase | Original | Revised | Notes |
|-------|----------|---------|-------|
| CMake + deps + packaging + CI | 4-6h | **10-20h** | Includes rpath, install targets, CI setup |
| Allegro 5 shim/header | 8-12h | **8-16h** (thin) or **20-40h** (semantic emulation) | Depends on approach |
| Core loop + display/input/timing | 12-16h | **20-40h** | Title screen bring-up included |
| Module migration (35+ files) | 20-30h | **40-80h** | Many edge cases |
| Font migration + UI tuning | 4-6h | **8-20h** | Layout verification |
| Audio (FMOD keep) | 2-4h | **8-20h** | Modern SDK + packaging |
| Audio (Allegro migrate) | 8-12h | **12-30h** | Full rewrite |
| libnoise/Lua integration | 4-6h | **6-15h** | Build system integration |
| Platform cleanup | 2-4h | **4-10h** | Win32 audit |
| Testing/debugging | 15-25h | **30-80h** | Milestone-based testing |
| **Total** | **73-117h** | **130-315h** | Depending on audio choice and semantic gaps |

### 11.5 Recommendations (Prioritized)

1. **Add Build & CI section**: Choose dependency strategy (vcpkg/FetchContent/system), define toolchain versions, add macOS+Ubuntu CI builds early.

2. **Plan packaging early**: macOS `.app` bundle resources layout and rpath; Linux install layout and optional AppImage/Flatpak.

3. **Define target main-loop model**: Fixed timestep vs variable, event handling approach, rendering target strategy (backbuffer vs render-to-texture).

4. **Implement thin compatibility shim**: For most-used primitives only; avoid full Allegro 4 semantic emulation unless proven necessary.

5. **Bring font rendering + one UI screen up early**: To detect layout drift before bulk migration.

6. **Decide audio backend early**: If keeping FMOD, validate modern SDK availability and distribution constraints up front.

7. **Convert testing into milestones**:
   - Hello screen → Title screen → Menu navigation → Module-by-module
   - Add optional ASan/UBSan debug builds

8. **Audit pass for Win32/MSVC-isms**: Check binary serialization assumptions before bulk edits.

### 11.6 Technical Gotchas to Watch

**Allegro 5 Specific:**
- Target bitmap switching (must `al_set_target_bitmap()` before drawing)
- Blending mode defaults differ
- Bitmap locking required for `get/put pixel` operations
- Display event handling (close button, resize)
- Resource lifetimes tied to display
- VSync variability across platforms

**C++ Portability:**
- MSVC-isms (`#pragma` directives, non-standard extensions)
- Struct packing differences (may affect save file format)
- `char` signedness (signed on x86, unsigned on ARM)
- Path/CWD assumptions

### 11.7 Alternative Approaches Considered

1. **SDL2 instead of Allegro 5**: Would work but changes scope significantly. Allegro 5 is reasonable given Allegro 4 heritage.

2. **Engine abstraction layer first**: Define `Renderer`/`Input`/`Audio` abstractions, implement Allegro 5 backend. Higher upfront cost but reduces regression risk and enables future backend swaps.

3. **Full compatibility layer**: Emulate all Allegro 4 semantics. **Not recommended** - can balloon in scope and mask bugs.

---

## 12. Updated Action Items

Based on expert review, the following items should be addressed before/during Phase 1:

### Pre-Phase 1 (Decision Points)
- [ ] **Decide**: Audio backend (FMOD Core vs Allegro Audio)
- [ ] **Decide**: Dependency strategy (vcpkg vs Homebrew/apt vs FetchContent)
- [ ] **Decide**: Target OS versions (macOS 10.15+? 11+? Ubuntu 20.04? 22.04?)
- [ ] **Audit**: Quick scan for Win32/MSVC-isms and binary serialization

### Phase 1 Additions
- [ ] Set up GitHub Actions CI for macOS and Ubuntu
- [ ] Create "hello Allegro 5" test executable to validate toolchain
- [ ] Document reproducible dev environment setup

### New Milestone-Based Testing
- [ ] **M1**: CMake builds, hello window displays
- [ ] **M2**: Title screen renders, input works
- [ ] **M3**: Menu navigation, fonts render correctly
- [ ] **M4**: One full game module works (Starport recommended)
- [ ] **M5**: All modules compile and run
- [ ] **M6**: Full playthrough possible

---

*Document prepared: January 2026*
*Expert review: January 2026*
*Target completion: TBD based on resource allocation*
*Revised estimate: 130-315 hours (was 73-117 hours)*
