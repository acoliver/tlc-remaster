# Allegro 4 to Allegro 5 Conversion Plan

## Overview

This plan converts the TLC codebase from Allegro-Legacy (Allegro 4 API compatibility layer) to native Allegro 5 APIs. The conversion is broken into phases, each handled by subagents with verification and remediation loops.

**Branch:** `allegro5-conversion`

**Key Principle:** Every phase MUST compile successfully before proceeding. No "preexisting" excuses - fix all compilation errors.

---

## Coordinator Instructions

### Todo Management

For each phase, the coordinator (main agent) MUST use `todo_write` to create items:
1. One item per phase execution
2. One item per verification  
3. One item per remediation (if needed)

**Status tracking:**
- `pending` - Not started
- `in_progress` - Currently executing
- `completed` - Done successfully

**CRITICAL:** Do NOT stop for "good progress" updates. Complete ALL tasks in the todo list before reporting to user.

### Verification Loop Pattern

```
For each phase:
  1. Execute phase (subagent task)
  2. Verify compilation: `cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4`
  3. If compilation fails → Remediate (subagent task) → Return to step 2
  4. Verify work (reviewer subagent)
  5. If verification fails → Remediate (subagent task) → Return to step 2
  6. Commit changes
  7. Mark phase complete
```

### Subagent Assignment Matrix

| Task Type | Subagent | Profile |
|-----------|----------|---------|
| Code Migration | `cplusplus-expert` | C++ coding, careful changes |
| Code Search/Analysis | `codeanalayzer` | Pattern finding |
| Work Verification | `deepthinker` | Deep analysis, quality assurance |
| Documentation | `docwriter` | Update docs |

---

## Phase 0: Infrastructure Setup

**Subagent:** `codeanalayzer`
**Timeout:** 300 seconds

### Task Prompt
```
Analyze the TLC codebase at /Users/acoliver/projects/tlc to identify:

1. The main include structure - where is allegro.h included?
2. Find the env.h file and understand how Allegro headers are managed
3. Identify any existing Allegro 5 includes (allegro5/*.h)
4. Check CMakeLists.txt for Allegro linking configuration

Write your findings to /Users/acoliver/projects/tlc/project-plans/allegro5/phase0_analysis.md

List every file that includes allegro.h directly.
```

### Verification Prompt (deepthinker)
```
Read /Users/acoliver/projects/tlc/project-plans/allegro5/phase0_analysis.md

Verify:
1. The analysis file exists and is comprehensive
2. All allegro.h includes are identified
3. CMake configuration is documented

Report PASS or FAIL with specific reasons.
```

### Commit Message Template
```
Phase 0: Infrastructure analysis for Allegro 5 conversion

- Documented current Allegro include structure
- Identified all files using Allegro 4 APIs
- Analyzed CMake configuration
```

---

## Phase 1: Create Allegro 5 Compatibility Header

**Subagent:** `cplusplus-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Create a compatibility header for the TLC codebase Allegro 5 migration.

Read:
- /Users/acoliver/projects/tlc/research/allegro_porting_research.md
- /Users/acoliver/projects/tlc/research/allegro_patterns_catalog.md

Create file: /Users/acoliver/projects/tlc/src/allegro5_compat.h

This header should:
1. Include all necessary Allegro 5 headers:
   - allegro5/allegro.h
   - allegro5/allegro_image.h
   - allegro5/allegro_font.h
   - allegro5/allegro_ttf.h
   - allegro5/allegro_primitives.h
   - allegro5/allegro_audio.h
   - allegro5/allegro_acodec.h

2. Create typedef/macro bridges for gradual migration:
   - typedef ALLEGRO_BITMAP BITMAP; (temporary compatibility)
   - Macro wrappers for common functions

3. Include proper header guards

4. Add comments explaining migration status

The goal is to allow incremental migration - this header lets old code compile while we migrate function by function.

After creating the file, verify it compiles by running:
cd /Users/acoliver/projects/tlc && mkdir -p build && cd build && cmake .. && make -j4

If compilation fails, fix the errors.
```

### Verification Prompt (deepthinker)
```
Review the Allegro 5 compatibility header at /Users/acoliver/projects/tlc/src/allegro5_compat.h

Verify:
1. File exists with proper header guards
2. All essential Allegro 5 headers are included
3. Compatibility typedefs/macros are present
4. The project compiles successfully

Run compilation check:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL with specific issues.
```

### Remediation Prompt (cplusplus-expert)
```
The Allegro 5 compatibility header has issues that need fixing.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix the issues in /Users/acoliver/projects/tlc/src/allegro5_compat.h

After fixing, verify compilation:
cd /Users/acoliver/projects/tlc && cd build && cmake .. && make -j4

Do not stop until compilation succeeds.
```

### Commit Message Template
```
Phase 1: Create Allegro 5 compatibility header

- Added src/allegro5_compat.h with Allegro 5 includes
- Created compatibility typedefs for gradual migration
- Maintains backward compatibility with existing code
```

---

## Phase 2: Color System Migration

**Subagent:** `cplusplus-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Migrate the TLC color system from Allegro 4 to Allegro 5.

Reference: /Users/acoliver/projects/tlc/research/allegro_transitions.md

Target files:
1. /Users/acoliver/projects/tlc/src/Game.h - Contains 27 color macros (lines 33-60)
2. All files using makecol() (~190 occurrences)

Migration tasks:
1. In Game.h, update color macros from:
   #define BLACK makecol(0,0,0)
   To use a compatibility approach that works with both systems

2. Create a helper function or macro in allegro5_compat.h:
   - al_map_rgb_compat() that returns proper ALLEGRO_COLOR
   - Or keep makecol() working via macro

3. Search for all makecol() calls and ensure they work with the new system

4. Handle transparent color (255,0,255) - this needs al_convert_mask_to_alpha()

Files with heavy makecol usage:
- ModulePlanetSurface.cpp
- ModuleEngineer.cpp
- ModuleBank.cpp
- ScrollBox.cpp
- ModuleStarmap.cpp

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4

Fix any compilation errors before completing.
```

### Verification Prompt (deepthinker)
```
Verify the color system migration in TLC codebase.

Check:
1. Game.h color macros are updated (lines 33-60)
2. allegro5_compat.h has color compatibility helpers
3. No remaining raw makecol() calls that would fail
4. Transparent color handling is addressed

Run compilation:
cd /Users/acoliver/projects/tlc/build && make -j4

Search for any unconverted patterns:
grep -r "makecol" /Users/acoliver/projects/tlc/src --include="*.cpp" --include="*.h" | head -20

Report PASS or FAIL with specific issues.
```

### Remediation Prompt (cplusplus-expert)
```
Color system migration has issues.

Previous verification failed with: {VERIFICATION_FAILURE_REASON}

Fix the color system migration in TLC codebase. Ensure:
1. All makecol() calls compile correctly
2. Color macros in Game.h work
3. No compilation errors

After fixing, verify:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Commit Message Template
```
Phase 2: Migrate color system to Allegro 5

- Updated color macros in Game.h for A5 compatibility
- Added color conversion helpers to allegro5_compat.h
- Migrated ~190 makecol() occurrences
- Transparent color (255,0,255) handling updated
```

---

## Phase 3A: Graphics Functions - blit() Migration

**Subagent:** `cplusplus-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Migrate blit() and masked_blit() functions from Allegro 4 to Allegro 5.

Reference: /Users/acoliver/projects/tlc/research/allegro_transitions.md

Target: ~140 blit/masked_blit occurrences

Allegro 4 → Allegro 5 mapping:
- blit(src, dest, sx, sy, dx, dy, w, h) → al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0)
- masked_blit() → al_draw_bitmap_region() with proper blending

Strategy:
1. Add compatibility macros to allegro5_compat.h:
```c
#define blit(src, dest, sx, sy, dx, dy, w, h) \
    do { \
        ALLEGRO_BITMAP* _prev = al_get_target_bitmap(); \
        al_set_target_bitmap(dest); \
        al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0); \
        al_set_target_bitmap(_prev); \
    } while(0)
```

2. Handle the target bitmap concept - Allegro 5 draws to current target, not dest parameter

3. Key files to verify work:
   - ModulePlanetSurface.cpp (L678, L2175, L2297)
   - ModuleCaptainCreation.cpp (L378-L421)
   - ModuleStarport.cpp (L543-L549)
   - PlanetTileScroller.cpp (L152, L184-L211)

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4

Fix any compilation errors.
```

### Verification Prompt (deepthinker)
```
Verify blit() migration in TLC codebase.

Check:
1. allegro5_compat.h has blit/masked_blit compatibility macros
2. Target bitmap handling is correct
3. Key files compile: ModulePlanetSurface.cpp, ModuleCaptainCreation.cpp

Run compilation:
cd /Users/acoliver/projects/tlc/build && make -j4

Test grep for patterns:
grep -r "blit(" /Users/acoliver/projects/tlc/src --include="*.cpp" | wc -l

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 3A: Migrate blit()/masked_blit() to Allegro 5

- Added compatibility macros for blit functions
- Implemented target bitmap management
- Migrated ~140 blit occurrences
- All files compile successfully
```

---

## Phase 3B: Graphics Functions - Sprite and Rotation

**Subagent:** `cplusplus-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Migrate draw_sprite() and rotate_sprite() functions to Allegro 5.

Target: ~20 occurrences

Allegro 4 → Allegro 5 mapping:
- draw_sprite(dest, src, x, y) → al_draw_bitmap(src, x, y, 0) on target
- rotate_sprite(dest, src, x, y, angle) → al_draw_rotated_bitmap()
- stretch_blit() → al_draw_scaled_bitmap()
- masked_stretch_blit() → al_draw_scaled_bitmap() with blending

Key differences:
1. Allegro 4 uses fixed-point angles (256 = full circle)
2. Allegro 5 uses radians
3. Conversion: a5_angle = a4_angle * ALLEGRO_PI / 128.0

Files to update:
- Sprite.cpp (L167-L185, L265-L268)
- PlanetSurfaceObject.cpp (L354-L407)
- MiniWindow.cpp (L93-L135) - includes flip variants
- ModuleSolarSystem.cpp (L1001)

Add to allegro5_compat.h:
- draw_sprite() macro
- rotate_sprite() macro with angle conversion
- stretch_blit() macro
- draw_sprite_v_flip, draw_sprite_h_flip, draw_sprite_vh_flip macros

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify sprite/rotation migration.

Check:
1. draw_sprite compatibility macro exists
2. rotate_sprite handles angle conversion (fixed-point to radians)
3. Flip variants are implemented
4. Sprite.cpp, PlanetSurfaceObject.cpp compile

Run compilation:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 3B: Migrate sprite/rotation functions to Allegro 5

- Added draw_sprite() compatibility macro
- Implemented rotate_sprite() with angle conversion
- Added flip variants (v_flip, h_flip, vh_flip)
- stretch_blit compatibility added
```

---

## Phase 3C: Graphics Functions - Primitives

**Subagent:** `cplusplus-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Migrate drawing primitives from Allegro 4 to Allegro 5.

Target functions:
- rectfill() → al_draw_filled_rectangle()
- rect() → al_draw_rectangle()
- line() → al_draw_line()
- circle/circlefill() → al_draw_circle/al_draw_filled_circle()
- ellipse() → al_draw_ellipse()
- putpixel/getpixel() → al_put_pixel/al_get_pixel()

Add compatibility macros to allegro5_compat.h.

Note: Allegro 5 primitives require allegro_primitives addon.

Key files:
- ModulePlanetSurface.cpp (rectfill)
- ModuleSolarSystem.cpp (ellipse L764, rectfill L751)
- ModuleStarmap.cpp (circlefill L392, L408)
- alfont_compat.cpp (putpixel L234)

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify primitives migration.

Check:
1. rectfill, line, circle macros exist in allegro5_compat.h
2. Primitives addon is included
3. ModuleSolarSystem.cpp, ModuleStarmap.cpp compile

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 3C: Migrate drawing primitives to Allegro 5

- Added rectfill, rect, line, circle compatibility macros
- Added putpixel/getpixel compatibility
- Integrated allegro_primitives addon
```

---

## Phase 4: Bitmap Management Functions

**Subagent:** `cplusplus-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Migrate bitmap management functions to Allegro 5.

Target functions:
- create_bitmap() → al_create_bitmap()
- load_bitmap() → al_load_bitmap()
- destroy_bitmap() → al_destroy_bitmap()
- create_sub_bitmap() → al_create_sub_bitmap()
- clear_bitmap() → al_clear_to_color() on target
- clear_to_color() → al_clear_to_color() on target
- bitmap->w, bitmap->h → al_get_bitmap_width/height()

~100 occurrences total

Add compatibility macros to allegro5_compat.h:
```c
#define create_bitmap(w, h) al_create_bitmap(w, h)
#define load_bitmap(f, p) al_load_bitmap(f)
#define destroy_bitmap(b) al_destroy_bitmap(b)
```

For clear functions, need target bitmap management:
```c
#define clear_bitmap(bmp) do { \
    ALLEGRO_BITMAP* _p = al_get_target_bitmap(); \
    al_set_target_bitmap(bmp); \
    al_clear_to_color(al_map_rgb(0,0,0)); \
    al_set_target_bitmap(_p); \
} while(0)
```

Also need to handle bitmap dimension access - this is trickier as it changes from struct member to function call.

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify bitmap management migration.

Check:
1. create_bitmap, load_bitmap, destroy_bitmap macros work
2. clear_bitmap/clear_to_color handle target bitmap
3. All modules compile

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 4: Migrate bitmap management to Allegro 5

- Added create/load/destroy_bitmap compatibility macros
- Implemented clear_bitmap with target bitmap handling
- Note: bitmap->w/h access may need per-file updates
```

---

## Phase 5: Display System Migration

**Subagent:** `cplusplus-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Migrate display system from Allegro 4 to Allegro 5.

Target:
- set_gfx_mode() → al_create_display()
- screen global → al_get_backbuffer(display)
- Screen dimension handling

Key file: /Users/acoliver/projects/tlc/src/Game.cpp
- Lines 783-801: set_gfx_mode() calls
- Line 826: m_backbuffer = create_bitmap()

Read the existing code carefully. The game already has:
- m_backbuffer member
- Custom SCREEN_WIDTH/SCREEN_HEIGHT constants

Strategy:
1. In Game.cpp Init_Graphics():
   - Replace set_gfx_mode() with al_create_display()
   - Store display pointer
   - Update m_backbuffer to use al_get_backbuffer()

2. Add to allegro5_compat.h:
   - screen macro that returns current display backbuffer
   - set_gfx_mode compatibility macro

3. Handle the blit to screen at end of frame (Game.cpp ~L1285)

The game uses stretch_blit to screen for scaling - this needs updating.

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify display system migration.

Check:
1. Game.cpp Init_Graphics() uses Allegro 5 display creation
2. screen global is properly defined/redirected
3. Frame presentation works (stretch_blit to screen)
4. Compilation succeeds

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 5: Migrate display system to Allegro 5

- Replaced set_gfx_mode() with al_create_display()
- Updated backbuffer management
- Screen global compatibility maintained
- Frame presentation updated
```

---

## Phase 6: Input System Migration

**Subagent:** `cplusplus-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Migrate input system from Allegro 4 to Allegro 5.

Target:
- key[] array → al_key_down() or event-driven
- mouse_x, mouse_y, mouse_b → al_get_mouse_state()
- install_keyboard/mouse → al_install_keyboard/mouse

~56 input references

The codebase likely has a custom input handling layer. Find it first:
grep -r "key\[" /Users/acoliver/projects/tlc/src --include="*.cpp"
grep -r "mouse_x\|mouse_y\|mouse_b" /Users/acoliver/projects/tlc/src --include="*.cpp"

Strategy:
1. Add to allegro5_compat.h:
```c
// Key array compatibility
extern ALLEGRO_KEYBOARD_STATE _al_key_state;
#define key _al_key_state_array
// Need to update key state each frame
```

2. For mouse:
```c
extern int mouse_x, mouse_y, mouse_b;
// Update these from ALLEGRO_MOUSE_STATE each frame
```

3. Find where input is polled and add state update calls

Key file likely: Game.cpp game loop

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify input system migration.

Check:
1. key[] array compatibility exists
2. mouse_x/y/b globals are maintained
3. Input state is updated each frame
4. Compilation succeeds

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 6: Migrate input system to Allegro 5

- Added key[] array compatibility layer
- Mouse globals maintained via state polling
- Input state updates integrated into game loop
```

---

## Phase 7: Timer and System Functions

**Subagent:** `cplusplus-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Migrate timer and system functions to Allegro 5.

Target:
- allegro_init() → al_init()
- allegro_exit() → al_uninstall_system()
- install_timer() → (not needed in A5, timers work differently)
- rest() → al_rest()
- allegro_message() → al_show_native_message_box()

Key file: Game.cpp
- L898: allegro_init()
- L1074: allegro_exit()

Also check for:
- install_int / install_int_ex (timer callbacks)

Allegro 5 uses event-based timers instead of interrupt callbacks.
If timer callbacks exist, they need significant rework.

Search first:
grep -r "install_int\|install_timer" /Users/acoliver/projects/tlc/src --include="*.cpp"

Add compatibility to allegro5_compat.h:
```c
#define allegro_init() al_init()
#define allegro_exit() al_uninstall_system()
#define rest(ms) al_rest((ms) / 1000.0)
```

After changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify timer/system migration.

Check:
1. allegro_init/exit compatibility
2. rest() works
3. Timer callbacks handled (if any exist)
4. Compilation succeeds

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 7: Migrate timer and system functions to Allegro 5

- allegro_init/exit compatibility added
- rest() converted to al_rest()
- Timer system evaluated and updated as needed
```

---

## Phase 8: Datafile System Migration

**Subagent:** `cplusplus-expert`
**Timeout:** 600 seconds

### Task Prompt
```
Migrate datafile system from Allegro 4.

Target: ~44 datafile operations
- load_datafile() → No direct equivalent in A5
- unload_datafile() → No direct equivalent

Allegro 5 removed the datafile system. Options:
1. Keep using Allegro-Legacy's datafile support
2. Convert .dat files to individual files
3. Create a compatibility layer that loads individual files

Investigate current usage:
grep -r "load_datafile\|unload_datafile" /Users/acoliver/projects/tlc/src --include="*.cpp"

For now, maintain compatibility by:
1. Checking if Allegro-Legacy provides datafile support
2. If not, create stub functions or convert data loading

This phase may require significant analysis. Document findings in:
/Users/acoliver/projects/tlc/project-plans/allegro5/phase8_datafile_analysis.md

After any changes, verify compilation:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4
```

### Verification Prompt (deepthinker)
```
Verify datafile handling.

Check:
1. Datafile strategy is documented
2. Compilation succeeds
3. No broken datafile references

Run:
cd /Users/acoliver/projects/tlc/build && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 8: Address datafile system for Allegro 5

- Analyzed datafile usage (~44 occurrences)
- [Strategy implemented: compatibility/conversion/kept legacy]
- Documentation updated
```

---

## Phase 9: Final Integration and Cleanup

**Subagent:** `cplusplus-expert`
**Timeout:** 900 seconds

### Task Prompt
```
Final integration and cleanup for Allegro 5 migration.

Tasks:
1. Review allegro5_compat.h for completeness
2. Remove any remaining Allegro 4 direct includes
3. Update CMakeLists.txt if needed for Allegro 5 linking
4. Ensure all addons are properly initialized:
   - al_init_image_addon()
   - al_init_font_addon()
   - al_init_ttf_addon()
   - al_init_primitives_addon()
   - al_install_audio()
   - al_init_acodec_addon()

5. Run full compilation
6. Document any remaining migration items

Check initialization order in Game.cpp - Allegro 5 requires specific init sequence.

After all changes:
cd /Users/acoliver/projects/tlc/build && cmake .. && make -j4

Create final migration status document:
/Users/acoliver/projects/tlc/project-plans/allegro5/migration_complete.md
```

### Verification Prompt (deepthinker)
```
Final verification of Allegro 5 migration.

Check:
1. Full compilation succeeds with no warnings about Allegro
2. All Allegro 5 addons are initialized
3. allegro5_compat.h is complete
4. Migration status documented

Run full build:
cd /Users/acoliver/projects/tlc && rm -rf build && mkdir build && cd build && cmake .. && make -j4

Report PASS or FAIL.
```

### Commit Message Template
```
Phase 9: Complete Allegro 5 migration

- Finalized allegro5_compat.h
- All Allegro 5 addons properly initialized
- CMake configuration verified
- Migration documentation complete

This completes the Allegro 4 to Allegro 5 API migration.
Codebase now uses native Allegro 5 with compatibility layer.
```

---

## Execution Checklist

The coordinator MUST execute phases in order, using todo_write to track:

```
[ ] Phase 0: Infrastructure Setup
    [ ] Execute (codeanalayzer)
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 1: Compatibility Header
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 2: Color System
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 3A: blit() Migration
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 3B: Sprite/Rotation
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 3C: Primitives
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 4: Bitmap Management
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 5: Display System
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 6: Input System
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 7: Timer/System
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 8: Datafiles
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Commit

[ ] Phase 9: Final Integration
    [ ] Execute (cplusplus-expert)
    [ ] Compile check
    [ ] Verify (deepthinker)
    [ ] Remediate if needed
    [ ] Final commit
```

---

## Important Notes

1. **NEVER skip compilation checks** - Every phase must compile
2. **NEVER accept "preexisting errors"** - Fix all compilation errors
3. **NEVER stop for progress updates** - Complete all tasks
4. **ALWAYS use todo_write** - Track every task
5. **ALWAYS commit after successful phase** - Atomic commits
6. **Remediation loops until pass** - Don't proceed with failures
