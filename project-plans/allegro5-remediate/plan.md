# Allegro Legacy → Allegro 5 Remediation Plan (Direct Port, No Shim)

---
## QUICK START - READ THIS FIRST

**If you're starting fresh (context cleared), follow these steps exactly:**

### Step 1: Verify Environment
```bash
cd /Users/acoliver/projects/tlc
git branch --show-current  # Should be: allegro5-conversion
```
If not on correct branch: `git checkout allegro5-conversion`

### Step 2: Verify Subagents Available
Call `list_subagents` and confirm these exist:
- `cplusplu-expert` (primary coder)
- `deepthinker` (verifier)
- `codeanalayzer` (analysis)
- `docwriter` or `reviewer` (opus-based plan review)

If `cplusplu-expert` doesn't exist, check for `cplusplus-expert` and use it instead. If neither exists, report to user.

### Step 3: Check Current Progress
```bash
cat /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md 2>/dev/null || echo "No progress file - starting fresh"
git log --oneline -10
```

### Step 4: Create Todo List (AGENT DOES THIS)
Call `todo_write` with the exact JSON structure from the "Todo Management" section below.
Do NOT ask the user to do this - the agent creates all todos automatically.
Each phase needs separate items for: Execute, Compile, Verify, Remediate (if needed), Commit.

### Step 5: Execute Phases Sequentially (AGENT DOES ALL OF THIS)
For each phase, the agent executes these steps autonomously:
1. **Execute**: Call `task(subagent_name="cplusplu-expert", goal_prompt="[phase prompt from below]", timeout_seconds=900)` (use `codeanalayzer` for analysis phases)
2. **Compile**: Call `run_shell_command("cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4")`
3. **If compile fails**: Call `task()` with cplusplu-expert to remediate, loop until compile passes
4. **Verify**: Call `task(subagent_name="deepthinker", goal_prompt="[verification prompt from below]", timeout_seconds=300)`
5. **If verify fails**: Call `task()` with cplusplu-expert to remediate, then re-compile, then re-verify
6. **Commit**: Call `run_shell_command("git add -A && git commit -m '[commit message from phase]'")`
7. **Update progress.md**: Call `replace` or `write_file` to mark phase completed
8. **Update todo status**: Call `todo_write` to mark items as `completed`

### Step 6: AGENT AUTONOMY RULES
- Do NOT stop for "progress updates" - complete ALL phases autonomously
- Do NOT ask user for permission at each step - just execute
- Do NOT skip compilation checks
- Do NOT accept "preexisting errors" as excuse - fix everything
- Loop remediation until verification passes
- Only report to user when ALL phases are complete or if truly blocked

### Key File Paths
- **This plan**: `/Users/acoliver/projects/tlc/project-plans/allegro5-remediate/plan.md`
- **Progress tracking**: `/Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md`
- **Datafile mapping**: `/Users/acoliver/projects/tlc/project-plans/allegro5-remediate/datafile_mapping.md`
- **Datafile inventory**: `/Users/acoliver/projects/tlc/project-plans/allegro5/datafile_inventory.md`
- **Source code**: `/Users/acoliver/projects/tlc/src/`

---

## Overview

This remediation plan replaces the prior "shim" approach with a **true Allegro 5 port**, using **direct A4→A5 API replacements** and **removing Allegro Legacy** from the build. The analysis and phase structure from `project-plans/allegro5/` is still valuable, but execution must be **implementation-first** and **no compatibility macros** are allowed.

**Branch:** `allegro5-conversion`

**Key Principles:**
- **No Allegro Legacy** after Phase 2 (build must compile with only Allegro 5)
- **No compatibility shim** (`allegro5_compat.h` must be removed/ignored)
- **Direct API replacements** in source code
- **Datafile system removed** (assets already exist as extracted files)

### What We Keep vs Remove
**Keep:**
- Include centralization (`env.h` usage) and related refactors
- libnoise include/link fix
- Any non-Legacy refactors that reduce include complexity

**Remove / ignore:**
- `allegro5_compat.h` and `allegro5_compat_globals.cpp`
- `USE_ALLEGRO_LEGACY` build flag and AllegroLegacy target linkage
- All compatibility macros for A4 APIs

---

## Coordinator Instructions

### Subagent Assignment Matrix

| Task Type | Subagent | Profile |
|-----------|----------|---------|
| Code Migration | `cplusplu-expert` | C++ coding, direct API replacements |
| Code Search/Analysis | `codeanalayzer` | Pattern finding, mapping | 
| Work Verification | `deepthinker` | Deep analysis, QA | 
| Documentation Review | `docwriter`/`reviewer` | Plan review (opus) |

---

### Todo Management

For each phase, the coordinator (main agent) MUST use `todo_write` to create items.

**AGENT ACTION: At the start of execution, call `todo_write` with this exact structure:**
```json
{
  "todos": [
    {"id": "p0-exec", "content": "Phase 0: Preflight datafile mapping (codeanalayzer)", "status": "pending"},
    {"id": "p0-compile", "content": "Phase 0: Compile baseline build", "status": "pending"},
    {"id": "p0-verify", "content": "Phase 0: Verify datafile mapping (deepthinker)", "status": "pending"},
    {"id": "p0-remediate", "content": "Phase 0: Remediate mapping if needed", "status": "pending"},
    {"id": "p0-commit", "content": "Phase 0: Commit mapping", "status": "pending"},
    {"id": "p1-exec", "content": "Phase 1: Datafile removal + filesystem loading (cplusplu-expert)", "status": "pending"},
    {"id": "p1-compile", "content": "Phase 1: Compile check and remediate if needed", "status": "pending"},
    {"id": "p1-verify", "content": "Phase 1: Verify datafile migration (deepthinker)", "status": "pending"},
    {"id": "p1-remediate", "content": "Phase 1: Remediate datafile migration if needed", "status": "pending"},
    {"id": "p1-commit", "content": "Phase 1: Commit changes", "status": "pending"},
    {"id": "p2-exec", "content": "Phase 2: Remove Allegro Legacy + shim artifacts (cplusplu-expert)", "status": "pending"},
    {"id": "p2-compile", "content": "Phase 2: Compile check and remediate if needed", "status": "pending"},
    {"id": "p2-verify", "content": "Phase 2: Verify build cleanup (deepthinker)", "status": "pending"},
    {"id": "p2-remediate", "content": "Phase 2: Remediate build cleanup if needed", "status": "pending"},
    {"id": "p2-commit", "content": "Phase 2: Commit changes", "status": "pending"},
    {"id": "p3-exec", "content": "Phase 3: Core init + display system (cplusplu-expert)", "status": "pending"},
    {"id": "p3-compile", "content": "Phase 3: Compile check and remediate if needed", "status": "pending"},
    {"id": "p3-verify", "content": "Phase 3: Verify display + init (deepthinker)", "status": "pending"},
    {"id": "p3-remediate", "content": "Phase 3: Remediate display/init if needed", "status": "pending"},
    {"id": "p3-commit", "content": "Phase 3: Commit changes", "status": "pending"},
    {"id": "p4-exec", "content": "Phase 4: Bitmap + color conversions (cplusplu-expert)", "status": "pending"},
    {"id": "p4-compile", "content": "Phase 4: Compile check and remediate if needed", "status": "pending"},
    {"id": "p4-verify", "content": "Phase 4: Verify bitmap/color conversion (deepthinker)", "status": "pending"},
    {"id": "p4-remediate", "content": "Phase 4: Remediate bitmap/color conversion if needed", "status": "pending"},
    {"id": "p4-commit", "content": "Phase 4: Commit changes", "status": "pending"},
    {"id": "p5-exec", "content": "Phase 5: Blit/stretch/sprites (direct A5) (cplusplu-expert)", "status": "pending"},
    {"id": "p5-compile", "content": "Phase 5: Compile check and remediate if needed", "status": "pending"},
    {"id": "p5-verify", "content": "Phase 5: Verify blit/sprite conversion (deepthinker)", "status": "pending"},
    {"id": "p5-remediate", "content": "Phase 5: Remediate blit/sprite conversion if needed", "status": "pending"},
    {"id": "p5-commit", "content": "Phase 5: Commit changes", "status": "pending"},
    {"id": "p6-exec", "content": "Phase 6: Drawing primitives conversion (cplusplu-expert)", "status": "pending"},
    {"id": "p6-compile", "content": "Phase 6: Compile check and remediate if needed", "status": "pending"},
    {"id": "p6-verify", "content": "Phase 6: Verify primitives conversion (deepthinker)", "status": "pending"},
    {"id": "p6-remediate", "content": "Phase 6: Remediate primitives conversion if needed", "status": "pending"},
    {"id": "p6-commit", "content": "Phase 6: Commit changes", "status": "pending"},
    {"id": "p7-exec", "content": "Phase 7: Input system conversion (cplusplu-expert)", "status": "pending"},
    {"id": "p7-compile", "content": "Phase 7: Compile check and remediate if needed", "status": "pending"},
    {"id": "p7-verify", "content": "Phase 7: Verify input conversion (deepthinker)", "status": "pending"},
    {"id": "p7-remediate", "content": "Phase 7: Remediate input conversion if needed", "status": "pending"},
    {"id": "p7-commit", "content": "Phase 7: Commit changes", "status": "pending"},
    {"id": "p8-exec", "content": "Phase 8: Timer/system + misc API conversion (cplusplu-expert)", "status": "pending"},
    {"id": "p8-compile", "content": "Phase 8: Compile check and remediate if needed", "status": "pending"},
    {"id": "p8-verify", "content": "Phase 8: Verify timer/system conversion (deepthinker)", "status": "pending"},
    {"id": "p8-remediate", "content": "Phase 8: Remediate timer/system conversion if needed", "status": "pending"},
    {"id": "p8-commit", "content": "Phase 8: Commit changes", "status": "pending"},
    {"id": "p9-exec", "content": "Phase 9: Final cleanup + verification (cplusplu-expert)", "status": "pending"},
    {"id": "p9-compile", "content": "Phase 9: Compile check and remediate if needed", "status": "pending"},
    {"id": "p9-verify", "content": "Phase 9: Final verification (deepthinker)", "status": "pending"},
    {"id": "p9-remediate", "content": "Phase 9: Remediate final verification issues", "status": "pending"},
    {"id": "p9-commit", "content": "Phase 9: Final commit - A5 MIGRATION COMPLETE", "status": "pending"}
  ]
}
```

---

## Verification Loop Pattern

```
For each phase:
  1. Execute phase (subagent task)
  2. Verify compilation: `cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4`
  3. If compilation fails → Remediate (subagent task) → Return to step 2
  4. Verify work (reviewer subagent)
  5. If verification fails → Remediate (subagent task) → Return to step 2
  6. Commit changes
  7. Mark phase complete in progress.md
```

---

## Phase 0: Preflight Datafile Mapping

**Subagent:** `codeanalayzer`
**Timeout:** 300 seconds

### Task Prompt
```
Create a datafile mapping based on the extracted assets (no .dat usage).

Inputs:
- /Users/acoliver/projects/tlc/project-plans/allegro5/datafile_inventory.md
- /Users/acoliver/projects/tlc/src/DataMgr.cpp
- /Users/acoliver/projects/tlc/src/DataMgr.h

Search patterns / commands:
- grep -R "DATAFILE" /Users/acoliver/projects/tlc/src
- grep -R "load_datafile" /Users/acoliver/projects/tlc/src
- grep -R "unload_datafile" /Users/acoliver/projects/tlc/src
- grep -R "\\.dat" /Users/acoliver/projects/tlc/src

Tasks:
1. Identify all DATAFILE object usage in DataMgr and modules.
2. For each datafile asset reference, map to its extracted file path under /bin/data/... (canonical root). Use /bin/data as the authoritative base unless an asset is only present under /data (document any exceptions).
3. Create a mapping file at:
   /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/datafile_mapping.md
   Include: datafile name, object id/index, target file path, and asset type (bmp/tga/ogg/wav).
4. Call out any references that have no clear file match.

Compile after mapping (baseline build):
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- datafile_mapping.md exists
- grep -R "load_datafile" src lists all mapped items

Update progress:
- Mark Phase 0 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Review /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/datafile_mapping.md

Verify:
1. All datafile references in DataMgr have a mapped filesystem path
2. Mapping covers each module datafile (.dat) listed in datafile_inventory.md
3. Any missing assets are explicitly noted

Compile:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL with specific reasons.
```

### Remediation Prompt (codeanalayzer)
```
Datafile mapping is incomplete or incorrect.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/datafile_mapping.md:
- Resolve unmapped assets
- Correct any incorrect paths
- Document any true gaps

Re-run compile:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 0: Datafile mapping for direct A5 load

- Mapped DATAFILE objects to filesystem assets
- Confirmed extracted asset coverage
- Documented any unresolved mappings
```

---

## Phase 1: Datafile Removal + Filesystem Loading

**Subagent:** `cplusplu-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Replace DATAFILE usage with direct filesystem loading.

Use:
- /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/datafile_mapping.md
- /Users/acoliver/projects/tlc/project-plans/allegro5/datafile_inventory.md

Target files (minimum):
- /Users/acoliver/projects/tlc/src/DataMgr.cpp
- /Users/acoliver/projects/tlc/src/DataMgr.h
- Any module referencing DATAFILE objects (discover via grep)

Search patterns:
- load_datafile\(
- unload_datafile\(
- DATAFILE
- ->dat
- \\.dat

Rules:
1. Replace DATAFILE loads with `al_load_bitmap` for images, `al_load_sample` for short SFX, and `al_load_audio_stream` for music/long tracks. Use asset extensions to choose loader: .bmp/.tga → bitmap, .wav → sample, .ogg → stream unless clearly SFX (document in mapping).
2. Use explicit filesystem paths (from mapping) under /bin/data/... unless the mapping specifies /data exceptions.
3. Remove .dat references entirely in code.
4. Ensure errors are logged when file loads fail.
5. Ensure audio system is initialized and samples reserved (`al_install_audio`, `al_init_acodec_addon`, `al_reserve_samples`) in Game init if not already present.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "load_datafile" src returns 0 matches
- grep -R "DATAFILE" src returns 0 matches

Update progress:
- Mark Phase 1 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify datafile migration (filesystem loads).

Check:
1. No remaining load_datafile/unload_datafile in src
2. DataMgr loads from file paths only
3. All mapped assets are referenced correctly

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL with issues.
```

### Remediation Prompt (cplusplu-expert)
```
Datafile migration failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix DATAFILE removal and filesystem loading. Re-check:
- DataMgr load paths
- Missing asset handling
- Any lingering DATAFILE types

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 1: Replace DATAFILE with filesystem asset loading

- Removed DATAFILE usage and .dat dependencies
- DataMgr now loads assets from extracted files
- Asset load failures now report explicit paths
```

---

## Phase 2: Remove Allegro Legacy + Shim Artifacts

**Subagent:** `cplusplu-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Remove Allegro Legacy and all shim artifacts from the build.

Target files:
- /Users/acoliver/projects/tlc/CMakeLists.txt
- /Users/acoliver/projects/tlc/src/CMakeLists.txt
- /Users/acoliver/projects/tlc/src/env.h
- /Users/acoliver/projects/tlc/src/allegro5_compat.h
- /Users/acoliver/projects/tlc/src/allegro5_compat_globals.cpp

Search patterns:
- AllegroLegacy
- USE_ALLEGRO_LEGACY
- allegro5_compat
- ALLEGRO_LEGACY_

Tasks:
1. Update CMakeLists.txt (root and src/) to remove AllegroLegacy usage and the USE_ALLEGRO_LEGACY option.
2. Ensure the build links only Allegro 5 libraries.
3. Update env.h to include Allegro 5 headers directly.
4. Remove/ignore allegro5_compat.h and allegro5_compat_globals.cpp (delete if unused).
5. Remove ALLEGRO_LEGACY_* definitions from build.
6. If any files are deleted, remove them from target source lists (e.g., add_executable / target_sources).

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "AllegroLegacy" CMakeLists.txt src/CMakeLists.txt returns 0 matches

Update progress:
- Mark Phase 2 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify Allegro Legacy removal.

Check:
1. No AllegroLegacy target in CMake
2. No USE_ALLEGRO_LEGACY flag remains
3. Build links only Allegro 5
4. No allegro5_compat.h usage

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Legacy removal failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix the build cleanup:
- Remove remaining AllegroLegacy references
- Remove legacy flags
- Remove deleted files from source lists

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 2: Remove Allegro Legacy + shim artifacts

- Dropped AllegroLegacy from build
- Removed compatibility header usage
- env.h now includes Allegro 5 directly
```

---

## Phase 3: Core Initialization + Display System

**Subagent:** `cplusplu-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Convert display system and core initialization to Allegro 5.

Target files:
- /Users/acoliver/projects/tlc/src/Game.cpp
- /Users/acoliver/projects/tlc/src/Game.h

Search patterns:
- allegro_init\(
- allegro_exit\(
- set_gfx_mode\(
- screen
- GFX_AUTODETECT

Tasks:
1. Replace allegro_init/allegro_exit usage with al_init/al_uninstall_system.
2. Add Allegro 5 addon init sequence: image, font, ttf, primitives, audio, acodec.
3. Replace set_gfx_mode() with al_create_display().
4. Replace screen global usage with al_get_backbuffer(display).
5. Update frame presentation: use al_set_target_backbuffer + al_draw_scaled_bitmap + al_flip_display().
6. Ensure display is destroyed on shutdown.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "set_gfx_mode" src returns 0 matches
- grep -R "screen" src only matches comment strings or updated references

Update progress:
- Mark Phase 3 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify core init and display conversion.

Check:
1. All Allegro 5 addons initialized in proper order
2. Display created via al_create_display
3. Frame present uses al_flip_display
4. No set_gfx_mode or screen global usage

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Display/init conversion failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix Game.cpp/Game.h display and init changes:
- addon init order
- display creation
- frame presentation

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 3: Convert core init + display to Allegro 5

- Added A5 addon initialization
- Replaced set_gfx_mode with al_create_display
- Updated frame presentation to al_flip_display
```

---

## Phase 4: Bitmap + Color Conversions

**Subagent:** `cplusplu-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Replace bitmap and color APIs with Allegro 5 equivalents.

Target files (examples; expand via grep):
- /Users/acoliver/projects/tlc/src/Game.cpp
- /Users/acoliver/projects/tlc/src/Game.h
- /Users/acoliver/projects/tlc/src/ModulePlanetSurface.cpp
- /Users/acoliver/projects/tlc/src/ModuleEngineer.cpp
- /Users/acoliver/projects/tlc/src/ModuleBank.cpp
- /Users/acoliver/projects/tlc/src/ScrollBox.cpp
- /Users/acoliver/projects/tlc/src/ModuleStarmap.cpp

Search patterns:
- makecol\(
- makeacol\(
- getr\(
- getg\(
- getb\(
- create_bitmap\(
- load_bitmap\(
- destroy_bitmap\(
- clear_bitmap\(
- clear_to_color\(
- ->w
- ->h

Rules:
1. Replace makecol/makeacol with al_map_rgb/al_map_rgba.
2. Replace create_bitmap/load_bitmap/destroy_bitmap with al_create_bitmap/al_load_bitmap/al_destroy_bitmap.
3. Replace ->w/->h with al_get_bitmap_width/height.
4. For clear operations, use al_set_target_bitmap and al_clear_to_color.
5. Update color macros in Game.h to use al_map_rgb directly.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "makecol" src returns 0 matches
- grep -R "->w" src | wc -l shows no BITMAP member usage

Update progress:
- Mark Phase 4 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify bitmap and color conversions.

Check:
1. No makecol/makeacol usage remains
2. No ->w/->h member access on ALLEGRO_BITMAP
3. Bitmap creation/loading uses A5 functions

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Bitmap/color conversion failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix remaining makecol/bitmap API usage and ->w/->h access.

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 4: Convert bitmap + color APIs to Allegro 5

- Replaced makecol/makeacol with al_map_rgb/al_map_rgba
- Converted bitmap create/load/destroy calls
- Replaced bitmap width/height member access
```

---

## Phase 5: Blit / Stretch / Sprite / Rotation

**Subagent:** `cplusplu-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Replace blit/masked_blit/stretch_blit/draw_sprite/rotate_sprite with Allegro 5 equivalents.

Target files (absolute paths):
- /Users/acoliver/projects/tlc/src/ModulePlanetSurface.cpp
- /Users/acoliver/projects/tlc/src/ModuleCaptainCreation.cpp
- /Users/acoliver/projects/tlc/src/ModuleStarport.cpp
- /Users/acoliver/projects/tlc/src/PlanetTileScroller.cpp
- /Users/acoliver/projects/tlc/src/TileScroller.cpp
- /Users/acoliver/projects/tlc/src/Sprite.cpp
- /Users/acoliver/projects/tlc/src/PlanetSurfaceObject.cpp
- /Users/acoliver/projects/tlc/src/MiniWindow.cpp
- /Users/acoliver/projects/tlc/src/ModuleSolarSystem.cpp

Search patterns:
- blit\(
- masked_blit\(
- stretch_blit\(
- masked_stretch_blit\(
- draw_sprite\(
- rotate_sprite\(

Rules:
1. Replace blit/masked_blit with al_draw_bitmap_region on a target bitmap.
2. Replace stretch_blit/masked_stretch_blit with al_draw_scaled_bitmap.
3. Replace draw_sprite with al_draw_bitmap on current target.
4. Replace rotate_sprite with al_draw_rotated_bitmap using radians.
5. Manage target bitmap explicitly with al_set_target_bitmap where needed.
6. Use ALLEGRO_FLIP_HORIZONTAL / ALLEGRO_FLIP_VERTICAL for flip variants.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "blit(" src returns 0 matches
- grep -R "draw_sprite" src returns 0 matches

Update progress:
- Mark Phase 5 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify blit/sprite conversion.

Check:
1. No blit/masked_blit/stretch_blit/draw_sprite/rotate_sprite calls remain
2. Target bitmap management is explicit and correct
3. Sprite rotation uses radians

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Blit/sprite conversion failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix remaining blit/sprite/rotation calls and target bitmap management.

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 5: Convert blit/stretch/sprite APIs to Allegro 5

- Replaced blit-style calls with al_draw_bitmap* equivalents
- Updated sprite rotation and flip handling
- Added explicit target bitmap management
```

---

## Phase 6: Drawing Primitives

**Subagent:** `cplusplu-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Replace drawing primitives with Allegro 5 primitives.

Target files (examples; expand via grep):
- /Users/acoliver/projects/tlc/src/ModulePlanetSurface.cpp
- /Users/acoliver/projects/tlc/src/ModuleSolarSystem.cpp
- /Users/acoliver/projects/tlc/src/ModuleStarmap.cpp
- /Users/acoliver/projects/tlc/src/alfont_compat.cpp

Search patterns:
- rectfill\(
- rect\(
- line\(
- circle\(
- circlefill\(
- ellipse\(
- triangle\(
- putpixel\(
- getpixel\(

Rules:
1. Use al_draw_filled_rectangle, al_draw_rectangle, al_draw_line, al_draw_circle, al_draw_filled_circle, al_draw_ellipse, al_draw_triangle.
2. Use al_put_pixel / al_get_pixel.
3. Ensure al_init_primitives_addon() is called in init (Phase 3).
4. Manage target bitmaps explicitly before drawing.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "rectfill(" src returns 0 matches

Update progress:
- Mark Phase 6 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify primitive conversions.

Check:
1. No rectfill/line/circle primitives remain
2. al_init_primitives_addon is called
3. Target bitmap management is explicit

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Primitive conversion failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix remaining primitive calls and ensure correct target bitmap usage.

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 6: Convert drawing primitives to Allegro 5

- Replaced primitive calls with al_draw_* equivalents
- Ensured primitives addon initialization
```

---

## Phase 7: Input System Conversion

**Subagent:** `cplusplu-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Replace Allegro 4 input APIs with Allegro 5 input handling.

Target files (examples; expand via grep):
- /Users/acoliver/projects/tlc/src/Game.cpp
- /Users/acoliver/projects/tlc/src/ModulePlanetSurface.cpp
- /Users/acoliver/projects/tlc/src/ModuleInterstellar.cpp
- /Users/acoliver/projects/tlc/src/ModuleStarport.cpp
- /Users/acoliver/projects/tlc/src/ModuleShipConfig.cpp
- /Users/acoliver/projects/tlc/src/ModuleCaptainCreation.cpp

Search patterns:
- key\[
- mouse_x
- mouse_y
- mouse_b
- mouse_z
- poll_keyboard\(
- poll_mouse\(
- KEY_
- install_keyboard\(
- install_mouse\(

Rules:
1. Replace key[] usage with al_key_down() using ALLEGRO_KEY_* constants.
2. Replace mouse_x/y/b/z with ALLEGRO_MOUSE_STATE.
3. Replace poll_keyboard/poll_mouse with al_get_keyboard_state/al_get_mouse_state.
4. Update KEY_* constants to ALLEGRO_KEY_* values (explicit replacement).
5. Ensure input update occurs per frame in Game loop (e.g., before UpdateKeyboard/UpdateMouse).
6. Ensure al_install_keyboard() and al_install_mouse() are called during init (Phase 3), and add if missing.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "key\[" src returns 0 matches
- grep -R "mouse_x" src returns 0 matches

Update progress:
- Mark Phase 7 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify input conversion.

Check:
1. No key[] or mouse_x/mouse_y/mouse_b usage remains
2. ALLEGRO_KEY_* constants used
3. Input state queried via al_get_*_state functions

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Input conversion failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix remaining key[]/mouse globals and ALLEGRO_KEY mappings.

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 7: Convert input handling to Allegro 5

- Replaced key[] and mouse globals with A5 state queries
- Updated keycode constants to ALLEGRO_KEY_*
- Input polling integrated into game loop
```

---

## Phase 8: Timer/System + Misc API Conversion

**Subagent:** `cplusplu-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Replace remaining Allegro 4 system/timer APIs with Allegro 5 equivalents.

Target files (examples; expand via grep):
- /Users/acoliver/projects/tlc/src/Game.cpp (main loop)
- /Users/acoliver/projects/tlc/src/Timer.cpp

Search patterns:
- allegro_message\(
- allegro_exit\(
- install_timer\(
- install_int\(
- install_int_ex\(
- rest\(

Rules:
1. Use al_show_native_message_box for message dialogs.
2. Use al_rest (seconds) for delays.
3. If timer callbacks exist, convert to al_create_timer + ALLEGRO_EVENT_QUEUE:
   - Create queue in Game init.
   - Register timer/display/keyboard/mouse event sources.
   - Drive updates via ALLEGRO_EVENT_TIMER.

Compile:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

Exit criteria (self-check):
- grep -R "install_timer" src returns 0 matches
- grep -R "install_int" src returns 0 matches

Update progress:
- Mark Phase 8 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Verify system/timer conversions.

Check:
1. No allegro_* legacy system calls remain
2. Timers use al_create_timer if needed
3. rest() replaced with al_rest

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
System/timer conversion failed verification.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix remaining system/timer legacy calls and update to A5 APIs.

Rebuild:
cd /Users/acoliver/projects/tlc/build && make -j4
```

### Commit Message Template
```
Phase 8: Convert timer/system APIs to Allegro 5

- Replaced legacy system functions
- Converted timer usage where needed
- Updated message dialogs and rest() calls
```

---

## Phase 9: Final Cleanup + Verification

**Subagent:** `cplusplu-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Final cleanup and verification for Allegro 5 port.

Target files:
- /Users/acoliver/projects/tlc/README.md
- /Users/acoliver/projects/tlc/BUILDING.md
- /Users/acoliver/projects/tlc/src (all)

Search / grep checks:
- grep -R "allegro.h" /Users/acoliver/projects/tlc/src
- grep -R "makecol" /Users/acoliver/projects/tlc/src
- grep -R "blit(" /Users/acoliver/projects/tlc/src
- grep -R "rectfill(" /Users/acoliver/projects/tlc/src
- grep -R "key\[" /Users/acoliver/projects/tlc/src

Tasks:
1. Remove any remaining Allegro 4 includes or identifiers.
2. Delete allegro5_compat.h and allegro5_compat_globals.cpp if still present.
3. Update BUILDING.md and README.md for Allegro 5 requirements.

Full build:
cd /Users/acoliver/projects/tlc && rm -rf build && mkdir build && cd build && cmake .. && make -j4

Update progress:
- Mark Phase 9 complete in /Users/acoliver/projects/tlc/project-plans/allegro5-remediate/progress.md
```

### Verification Prompt (deepthinker)
```
Final verification of A5 migration.

Check:
1. Full clean build succeeds
2. No Allegro 4 identifiers remain
3. Allegro Legacy removed from build
4. Documentation updated

Report PASS or FAIL.
```

### Remediation Prompt (cplusplu-expert)
```
Final verification failed.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix remaining Allegro 4 identifiers and documentation issues, then rebuild clean.

Full rebuild:
cd /Users/acoliver/projects/tlc && rm -rf build && mkdir build && cd build && cmake .. && make -j4
```

### Commit Message Template
```
Phase 9: Finalize Allegro 5 migration (no Legacy)

- Removed remaining A4 references
- Clean build passes on Allegro 5
- Documentation updated
```

---

## Execution Checklist

```
[ ] Phase 0: Preflight Datafile Mapping
    [ ] Execute (codeanalayzer)
    [ ] Compile baseline build
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 1: Datafile Removal + Filesystem Loading
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 2: Remove Allegro Legacy + Shim Artifacts
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 3: Core Init + Display System
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 4: Bitmap + Color Conversion
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 5: Blit/Sprite Conversion
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 6: Drawing Primitives
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 7: Input System
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 8: Timer/System + Misc APIs
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 9: Final Cleanup + Verification
    [ ] Execute (cplusplu-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Final commit
```

---

## Important Notes

1. **No compatibility shim** — all replacements are direct A5 API usage.
2. **Do not reintroduce Allegro Legacy** once Phase 2 completes.
3. **Datafiles are duplicates** — load from extracted files directly.
4. **Always compile after each phase**.
5. **Update progress.md after every phase**.
6. **Only proceed if previous phase passes verification.**
