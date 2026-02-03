# Phase 0: Infrastructure Analysis for Allegro 5 Conversion

## Executive Summary

The TLC codebase uses a **hybrid Allegro setup**:
- **Allegro Legacy** provides Allegro 4 API compatibility (macros like `blit()`, `makecol()`, etc.)
- **Native Allegro 5** is already used for: audio, fonts/TTF
- The conversion goal is to migrate from Allegro 4 APIs to native Allegro 5 APIs

## 1. Main Include Structure

### Files Including `<allegro.h>` Directly (62 files in src/)

**Core Files:**
- `src/env.h` (line 47) - **Central environment header**
- `src/Game.cpp` (line 14)
- `src/Timer.cpp` (line 1)
- `src/ModeMgr.cpp` (line 9)
- `src/Module.cpp` (line 9)
- `src/DataMgr.cpp` (line 9)
- `src/Script.cpp` (line 11)

**UI/Graphics Files:**
- `src/Sprite.h` (line 13)
- `src/Sprite.cpp` (line 12)
- `src/Button.h` (line 6)
- `src/Label.h` (line 4)
- `src/ScrollBox.h` (line 5)
- `src/MiniWindow.h` (line 18) - uses `"allegro.h"` (quoted)
- `src/MiniWindow.cpp` (line 2) - uses `"allegro.h"` (quoted)
- `src/MessageBoxWindow.h` (line 3)
- `src/PauseMenu.h` (line 3)
- `src/TileScroller.h` (line 5)
- `src/TileScroller.cpp` (line 3)
- `src/PlanetTileScroller.cpp` (line 3)
- `src/TexturedSphere.h` (line 12)
- `src/TexturedSphere.cpp` (line 2)

**Game Module Files:**
- `src/ModuleAuxiliaryDisplay.cpp` (line 50)
- `src/ModuleBank.h` (line 11)
- `src/ModuleCantina.h` (line 12)
- `src/ModuleCaptainCreation.h` (line 6)
- `src/ModuleCaptainsLounge.h` (line 6)
- `src/ModuleControlPanel.h` (line 15)
- `src/ModuleCrewHire.h` (line 12)
- `src/ModuleEncounter.h` (line 18)
- `src/ModuleEngineer.cpp` (line 9)
- `src/ModuleGameOver.h` (line 11)
- `src/ModuleInterstellar.h` (line 16)
- `src/ModuleMedical.cpp` (line 10)
- `src/ModuleMessageGUI.cpp` (line 9)
- `src/ModuleMiniGame.h` (line 12)
- `src/ModulePlanetOrbit.h` (line 12)
- `src/ModulePlanetSurface.h` (line 17)
- `src/ModuleQuestLog.h` (line 12)
- `src/ModuleQuestLog.cpp` (line 9)
- `src/ModuleSettings.h` (line 4)
- `src/ModuleSettings.cpp` (line 10)
- `src/ModuleShipConfig.h` (line 12)
- `src/ModuleSolarSystem.cpp` (implied)
- `src/ModuleStarmap.cpp` (line 9)
- `src/ModuleStarport.h` (line 12)
- `src/ModuleStartup.h` (line 11)
- `src/ModuleStartup.cpp` (line 12)
- `src/ModuleTitleScreen.cpp` (line 12)
- `src/ModuleTopGUI.cpp` (line 9)
- `src/ModuleTradeDepot.h` (line 6)

**Game Logic Files:**
- `src/Player.h` (line 12)
- `src/GameState.h` (line 14)
- `src/CombatObject.h` (line 4)
- `src/CombatPlayerVessel.h` (line 4)
- `src/PlanetaryBody.h` (line 19)
- `src/PlanetSurfaceObject.h` (line 4)
- `src/PlanetSurfacePlayerVessel.h` (line 4)
- `src/PlayerShipSprite.h` (line 11)
- `src/TerrainVehicleSprite.h` (line 12)
- `src/Util.h` (line 11)

**Font System:**
- `src/alfont.h` (line 4) - custom font compatibility layer

## 2. Environment Header (env.h) Analysis

Location: `/Users/acoliver/projects/tlc/src/env.h`

**Key Features:**
- Platform detection (Windows, macOS, Linux)
- Allegro Legacy configuration for MSVC
- Includes `<allegro.h>` at line 47
- Defines `TLC_USING_ALLEGRO_LEGACY` when Allegro Legacy is detected

**Important Note:** The `env.h` is the logical place to centralize Allegro header management. The migration strategy should update this file to include Allegro 5 headers instead.

## 3. Existing Allegro 5 Usage

The codebase already uses native Allegro 5 for specific subsystems:

### alfont_compat.cpp
```cpp
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
```
This provides TrueType font rendering using Allegro 5's font addons.

### AudioSystem_allegro.cpp
```cpp
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
```
Audio playback uses native Allegro 5 audio system.

### AudioSystem.h
```cpp
#include <allegro5/allegro_audio.h>
```
Forward declarations for audio types.

## 4. CMake Configuration Analysis

### Root CMakeLists.txt
- Uses `find_package(AllegroLegacy REQUIRED)` for Allegro 4 compatibility
- Configures Lua dependency (platform-specific)

### src/CMakeLists.txt
**Allegro Legacy Configuration:**
- Defines `ALLEGRO_LEGACY_NO_FIX_ALIASES` to avoid glibc conflicts
- Windows: `ALLEGRO_LEGACY_STATICLINK`, `ALLEGRO_LEGACY_NO_MAGIC_MAIN`, `ALLEGRO_LEGACY_MSVC`

**Allegro 5 Libraries Linked (all platforms):**
- `allegro`
- `allegro_font`
- `allegro_ttf`
- `allegro_primitives`
- `allegro_audio`
- `allegro_acodec`
- `allegro_main`
- `allegro_image` (Windows only currently)

**Platform-Specific:**
- Windows: Uses vcpkg or pkg-config for Allegro 5
- macOS: Homebrew paths (`/opt/homebrew/opt/allegro/`)
- Linux: pkg-config for Allegro 5 packages

**Important:** The CMake already links both Allegro Legacy AND Allegro 5. The migration will eventually remove the Allegro Legacy dependency.

## 5. Migration Strategy Recommendations

### Phase 1: Create Compatibility Header
Create `allegro5_compat.h` that:
1. Includes all Allegro 5 headers
2. Provides macros mapping Allegro 4 functions to Allegro 5
3. Allows gradual, file-by-file migration

### Key API Changes Needed
Based on file count analysis:
- ~62 files include allegro.h directly
- `makecol()` → `al_map_rgb()` (heavy usage in color macros)
- `blit()`, `masked_blit()` → `al_draw_bitmap_region()` with target management
- `create_bitmap()` → `al_create_bitmap()`
- `key[]` array → `al_key_down()` or event system
- `mouse_x/y/b` → `al_get_mouse_state()`

### Include Centralization
Eventually, most files should include `env.h` or a new unified header instead of `<allegro.h>` directly. This will make the transition cleaner.

## 6. Files Already Partially Migrated

The following files already use Allegro 5 APIs:
- `alfont_compat.cpp` - Font rendering (fully A5)
- `AudioSystem_allegro.cpp` - Audio (fully A5)
- `AudioSystem.h` - Audio types (A5)

These can serve as reference implementations for the migration.

---

**Analysis completed:** 2026-02-02
**Ready for:** Phase 1 - Compatibility Header Creation
