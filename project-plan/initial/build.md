# Build Plan: Phase 0 - Make It Compile

**Goal:** Get TLC to compile on macOS using Allegro Legacy, with a simple "does it execute" test.

**Out of scope:** Actually running the game, loading assets, gameplay testing.

---

## Part 0: Hello Allegro (Proof of Toolchain)

Before touching TLC code, verify we can build anything with Allegro Legacy.

### 0.1 Install Dependencies

```bash
# Allegro 5 (required by Allegro Legacy)
brew install allegro

# Lua 5.1
brew install lua@5.1

# libnoise - may need to build from source or find a formula
# TBD - check if available

# FMOD - download from fmod.com or defer (game might run without audio initially)
```

### 0.2 Build Allegro Legacy

```bash
git clone https://github.com/NewCreature/Allegro-Legacy.git
cd Allegro-Legacy
mkdir build && cd build
cmake ..
make
sudo make install
```

### 0.3 Hello Allegro Test

Create a minimal test program to verify the toolchain works:

```cpp
// hello_allegro.cpp
#include <allegro.h>
#include <stdio.h>

int main(int argc, char **argv) {
    if (allegro_init() != 0) {
        printf("Failed to init Allegro\n");
        return 1;
    }
    
    printf("Allegro Legacy initialized successfully!\n");
    printf("Allegro version: %s\n", allegro_id);
    
    allegro_exit();
    return 0;
}
END_OF_MAIN()
```

Build and run:
```bash
g++ hello_allegro.cpp -o hello_allegro $(pkg-config --cflags --libs allegro-legacy) 
./hello_allegro
```

**Success criteria:** Prints version info, exits cleanly.

---

## Part 1: CMake Build System

Create a CMake build that can find all dependencies.

### 1.1 Create Root CMakeLists.txt

Location: `/CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)
project(StarflightTLC VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(ALLEGRO REQUIRED allegro-legacy)  # or however A-L exports itself
pkg_check_modules(LUA REQUIRED lua5.1)

# Add subdirectories
add_subdirectory(src)
```

### 1.2 Create src/CMakeLists.txt

Location: `/src/CMakeLists.txt`

Lists all 80 source files, links against dependencies.

### 1.3 Files to Create

- [ ] `/CMakeLists.txt` - Root build file
- [ ] `/src/CMakeLists.txt` - Source build file  
- [ ] `/cmake/FindAllegroLegacy.cmake` - Find module (if needed)
- [ ] `/cmake/FindFMOD.cmake` - Find module for FMOD

---

## Part 2: Remove MSVC-isms

These must be removed/modified before the code will compile with clang/gcc.

### 2.1 Files to Delete

| File | Reason |
|------|--------|
| `src/build/msvc10/env.cpp` | Only contains `#pragma comment(lib, ...)` - not needed with CMake |
| `src/build/msvc10/*.vcxproj*` | Visual Studio project files |
| `src/build/msvc10/*.sln` | Visual Studio solution |

### 2.2 Remove `#pragma region` / `#pragma endregion`

**112 occurrences** across these files:

| File | Count |
|------|-------|
| `ModuleControlPanel.cpp` | ~6 |
| `ModuleCaptainCreation.cpp` | ~2 |
| `ModuleSettings.cpp` | ~2 |
| `ModuleBank.cpp` | ~2 |
| `ModuleCrewHire.cpp` | ~8 |
| `ModuleTradeDepot.cpp` | ~2 |
| `ModuleCaptainsLounge.cpp` | ~2 |
| `Game.cpp` | many |
| Others... | |

**Action:** Delete all lines matching `#pragma region` or `#pragma endregion`. They're just code folding markers.

```bash
# Preview what will be removed
grep -rn "pragma region\|pragma endregion" src/ --include="*.cpp" --include="*.h"

# Remove them (sed command)
find src -name "*.cpp" -o -name "*.h" | xargs sed -i '' '/#pragma region/d; /#pragma endregion/d'
```

### 2.3 Remove `END_OF_MAIN()`

**1 occurrence** in `src/main.cpp` line 25.

**Action:** 
- If using Allegro Legacy: Keep it (A-L provides this macro)
- If doing native A5: Remove it

For now: **Keep it** - Allegro Legacy defines this macro.

### 2.4 Fix `env.h`

Current content has Windows-specific logic. Needs update:

**Current problematic code:**
```cpp
#define ALLEGRO_ASSERT 1
#define MSVC10_DEBUG 1  // Remove this

#if defined(_MSC_VER) 
    #pragma warning( disable: 4312 )
    #define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined(_MSC_VER) && !defined(WIN32)
    #define MessageBox(a,b,c,d) allegro_message(b)
#endif
```

**New content:**
```cpp
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
#ifndef TLC_PLATFORM_WINDOWS
    #define MessageBox(hwnd, text, caption, type) allegro_message(text)
#endif

#endif // ENV_H
```

### 2.5 Fix `Timer.cpp`

Current code:
```cpp
#if defined(_MSC_VER) || defined(WIN32)
    return (long) clock();
#elif defined(_POSIX_SOURCE)
    // POSIX implementation
#endif
```

**Action:** The POSIX path should work on macOS. May need to ensure `_POSIX_SOURCE` is defined, or change to:

```cpp
#if defined(_WIN32) || defined(_WIN64)
    return (long) clock();
#else
    // POSIX implementation (macOS, Linux)
    timeval current, delta;
    gettimeofday(&current, NULL);
    timersub(&current, &initial, &delta);
    return (long) (delta.tv_sec*1000 + delta.tv_usec/1000);
#endif
```

### 2.6 Fix `Game.h` 

Remove MSVC10_DEBUG conditional:
```cpp
#ifdef MSVC10_DEBUG
    #include "GameState.h"
#endif
```

Change to unconditional:
```cpp
#include "GameState.h"
```

---

## Part 3: Handle External Dependencies

### 3.1 TinyXML (Bundled)

**Status:** Already in `src/tinyxml/`, should compile as-is.

**Action:** Add to CMake build, no code changes needed.

### 3.2 libnoise

**Status:** Need macOS build.

**Options:**
1. `brew install libnoise` (check if formula exists)
2. Build from source: https://github.com/qknight/libnoise
3. Use header-only fork

**Files using it:**
- `noiseutils.cpp`
- `TexturedSphere.cpp`
- `PlanetaryBody.cpp`

### 3.3 Lua 5.1

**Status:** Easy - `brew install lua@5.1`

**Files using it:**
- `Script.cpp`

**CMake:**
```cmake
pkg_check_modules(LUA REQUIRED lua5.1)
# or
find_package(Lua 5.1 REQUIRED)
```

### 3.4 FMOD

**Status:** Needs investigation. Options:

1. **Download FMOD Core SDK** for macOS from fmod.com
2. **Stub it out** temporarily - comment out audio for initial build
3. **Replace with Allegro audio** later

**Files using it:**
- `AudioSystem.cpp` (430 lines)
- `AudioSystem.h`

**Temporary stub approach:**
```cpp
// In AudioSystem.cpp, wrap everything in:
#ifdef TLC_ENABLE_AUDIO
// ... existing FMOD code ...
#else
// Stub implementations that do nothing
bool AudioSystem::Init() { return true; }
void AudioSystem::Update() {}
bool AudioSystem::Load(...) { return true; }
// etc.
#endif
```

---

## Part 4: Source File Inventory

All 80 `.cpp` files in `src/` to be compiled:

### Core Engine (7 files)
- [ ] `main.cpp`
- [ ] `Game.cpp`
- [ ] `GameState.cpp`
- [ ] `Module.cpp`
- [ ] `ModeMgr.cpp`
- [ ] `Timer.cpp`
- [ ] `LogFile.cpp`

### Graphics/UI (8 files)
- [ ] `Sprite.cpp`
- [ ] `Button.cpp`
- [ ] `Label.cpp`
- [ ] `ScrollBox.cpp`
- [ ] `MiniWindow.cpp`
- [ ] `MessageBoxWindow.cpp`
- [ ] `PauseMenu.cpp`
- [ ] `TileScroller.cpp`

### Game Modules (29 files)
- [ ] `ModuleAuxiliaryDisplay.cpp`
- [ ] `ModuleBank.cpp`
- [ ] `ModuleCantina.cpp`
- [ ] `ModuleCaptainCreation.cpp`
- [ ] `ModuleCaptainsLounge.cpp`
- [ ] `ModuleCargoWindow.cpp`
- [ ] `ModuleControlPanel.cpp`
- [ ] `ModuleCredits.cpp`
- [ ] `ModuleCrewHire.cpp`
- [ ] `ModuleEncounter.cpp`
- [ ] `ModuleEngineer.cpp`
- [ ] `ModuleGameOver.cpp`
- [ ] `ModuleInterstellar.cpp`
- [ ] `ModuleMedical.cpp`
- [ ] `ModuleMessageGUI.cpp`
- [ ] `ModuleMiniGame.cpp`
- [ ] `ModulePlanetOrbit.cpp`
- [ ] `ModulePlanetSurface.cpp`
- [ ] `ModuleQuestLog.cpp`
- [ ] `ModuleSettings.cpp`
- [ ] `ModuleShipConfig.cpp`
- [ ] `ModuleSideViewer.cpp`
- [ ] `ModuleSolarSystem.cpp`
- [ ] `ModuleStarmap.cpp`
- [ ] `ModuleStarport.cpp`
- [ ] `ModuleStartup.cpp`
- [ ] `ModuleTitleScreen.cpp`
- [ ] `ModuleTopGUI.cpp`
- [ ] `ModuleTradeDepot.cpp`

### Game Logic (18 files)
- [ ] `Player.cpp`
- [ ] `Archive.cpp`
- [ ] `DataMgr.cpp`
- [ ] `QuestMgr.cpp`
- [ ] `Quest.cpp`
- [ ] `QuestDependency.cpp`
- [ ] `Requirement.cpp`
- [ ] `Script.cpp`
- [ ] `AudioSystem.cpp`
- [ ] `Stardate.cpp`
- [ ] `GameTime.cpp`
- [ ] `Flux.cpp`
- [ ] `CombatObject.cpp`
- [ ] `FactionStanding.cpp`
- [ ] `CaptainType.cpp`
- [ ] `CargoSize.cpp`
- [ ] `CollectItem.cpp`
- [ ] `Interact.cpp`
- [ ] `KillAnimal.cpp`
- [ ] `TotalQuestsCompleted.cpp`

### Planet/Space (10 files)
- [ ] `PlanetaryBody.cpp`
- [ ] `PlanetTileScroller.cpp`
- [ ] `PlanetSurfaceObject.cpp`
- [ ] `PlanetSurfacePlayerVessel.cpp`
- [ ] `PlanetScan.cpp`
- [ ] `TexturedSphere.cpp`
- [ ] `OrbitPlanet.cpp`
- [ ] `PlayerShipSprite.cpp`
- [ ] `TerrainVehicleSprite.cpp`
- [ ] `CombatPlayerVessel.cpp`

### Utilities (6 files)
- [ ] `Util.cpp`
- [ ] `Math.cpp`
- [ ] `Vector3.cpp`
- [ ] `Model.cpp`
- [ ] `debug.cpp`
- [ ] `noiseutils.cpp`

### Bundled Libraries (4 files)
- [ ] `tinyxml/tinystr.cpp`
- [ ] `tinyxml/tinyxml.cpp`
- [ ] `tinyxml/tinyxmlerror.cpp`
- [ ] `tinyxml/tinyxmlparser.cpp`

---

## Part 5: Build Verification Target

Add a simple `--version` check to verify the executable runs.

### 5.1 Modify `main.cpp`

Add early argument checking before game init:

```cpp
#include <cstring>

int main(int argc, char **argv) {
    // Quick version check - doesn't need Allegro init
    if (argc > 1) {
        if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
            printf("Starflight: The Lost Colony\n");
            printf("Version: 1.0.0-dev (macOS port)\n");
            printf("Built with Allegro Legacy\n");
            return 0;
        }
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
            printf("Starflight: The Lost Colony\n");
            printf("Usage: starflighttlc [options]\n");
            printf("Options:\n");
            printf("  --version, -v    Show version info\n");
            printf("  --help, -h       Show this help\n");
            return 0;
        }
    }
    
    // Original game code follows...
    g_game = new Game();
    g_game->Run();
    // ...
}
```

### 5.2 Success Criteria

```bash
./starflighttlc --version
# Should output version info and exit 0

./starflighttlc --help  
# Should output help and exit 0
```

If these work, the executable is linking correctly against all libraries.

---

## Execution Checklist

### Phase 0: Toolchain
- [ ] Install Allegro 5 via Homebrew
- [ ] Build and install Allegro Legacy
- [ ] Create and run `hello_allegro.cpp` test
- [ ] Verify pkg-config or CMake can find Allegro Legacy

### Phase 1: CMake
- [ ] Create `/CMakeLists.txt`
- [ ] Create `/src/CMakeLists.txt`
- [ ] Test empty build (no source files yet)

### Phase 2: Clean MSVC-isms  
- [ ] Delete `src/build/msvc10/` directory
- [ ] Remove all `#pragma region` / `#pragma endregion` (112 occurrences)
- [ ] Update `src/env.h`
- [ ] Fix `src/Timer.cpp` platform detection
- [ ] Fix `src/Game.h` MSVC10_DEBUG include

### Phase 3: Dependencies
- [ ] Verify TinyXML compiles
- [ ] Get libnoise building
- [ ] Link Lua 5.1
- [ ] Stub or integrate FMOD

### Phase 4: Compile
- [ ] Add all source files to CMake
- [ ] Fix compiler errors as they arise
- [ ] Get clean compile (warnings OK for now)

### Phase 5: Link & Run
- [ ] Link all libraries
- [ ] Create executable
- [ ] Test `--version` and `--help`
- [ ] **DONE** - executable runs!

---

## Known Issues to Expect

1. **Include path differences** - Headers might not be found, need to adjust CMake include dirs
2. **Missing type definitions** - Some Windows types might need stubs
3. **FMOD** - Likely to cause issues, may need to stub
4. **libnoise** - May need source build
5. **Allegro Legacy include path** - Might be `<allegro.h>` or `<allegro4/allegro.h>`
