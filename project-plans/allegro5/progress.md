# Allegro 5 Conversion Progress

## Current Status: IN PROGRESS - Infrastructure Phase

## Completed Phases
- Phase 0: Infrastructure Analysis
- Phase 1: Compatibility Header Created
- Phase 1.5: Build System Updated (USE_ALLEGRO_LEGACY option)
- Phase 1.6: Include Centralization (all files now use env.h)

## Current Phase
Phase 2: Native A5 Migration - BLOCKED (needs extensive API shims)

## What's Been Done
1. Created allegro5_compat.h with:
   - All Allegro 5 headers included
   - BITMAP/ALLEGRO_BITMAP typedef bridge
   - Color system (int-based for compatibility)
   - Drawing primitives macros (rectfill, circle, line, etc.)
   - Blit compatibility macros
   - Sprite drawing macros
   - ASSERT, TRACE, END_OF_MAIN macros
   - Initialization helpers

2. Updated CMakeLists.txt:
   - Added USE_ALLEGRO_LEGACY option (default ON)
   - Conditional compilation for both modes
   - TLC_NATIVE_ALLEGRO5 define when building without Legacy

3. Updated env.h:
   - Conditional include of allegro5_compat.h vs allegro.h
   - Platform detection preserved

4. Centralized includes:
   - All 58 source files now include env.h instead of allegro.h directly
   - This enables future migration by changing just env.h

## Blocking Issues for Full Native A5 Migration

When building with -DUSE_ALLEGRO_LEGACY=OFF, the following are missing:

1. **DATAFILE system** - No A5 equivalent (14 modules use .dat files)
2. **Graphics mode functions** - set_gfx_mode(), GFX_AUTODETECT, etc.
3. **Display info** - get_desktop_resolution(), desktop_color_depth()
4. **BITMAP struct access** - ->w, ->h (A5 uses functions)
5. **Input installation** - install_keyboard(), install_mouse(), install_timer()
6. **File attributes** - FA_ALL, file_exists()
7. **Display management** - set_window_title(), get_refresh_rate()
8. **Blending** - set_alpha_blender()

## Recommended Next Steps

1. **Extract datafiles** - Convert .dat files to individual assets
2. **Add display shims** - Map set_gfx_mode to al_create_display
3. **Add input shims** - Map install_* functions
4. **Add bitmap property macros** - bitmap_width(b) = al_get_bitmap_width(b)

## Phase Completion Log

| Phase | Status | Commit Hash | Date | Notes |
|-------|--------|-------------|------|-------|
| 0 | COMPLETE | 9bfb193 | 2026-02-02 | Infrastructure Analysis |
| 1 | COMPLETE | - | 2026-02-03 | Compatibility Header Created |
| 1.5 | COMPLETE | - | 2026-02-03 | Build System Updated |
| 1.6 | COMPLETE | - | 2026-02-03 | Include Centralization |
| 2 | BLOCKED | - | - | Needs DATAFILE and API shims |
| 1 | COMPLETE | 17d4476 | 2026-02-02 | Compatibility Header |
| 2 | COMPLETE | 64240eb | 2026-02-02 | Color System |
| 3A | COMPLETE | 68d94a8 | 2026-02-02 | blit() Migration |
| 3B | COMPLETE | 245daf7 | 2026-02-02 | Sprite/Rotation |
| 3C | COMPLETE | 9afa5ff | 2026-02-02 | Primitives |
| 4 | COMPLETE | 6f12d15 | 2026-02-02 | Bitmap Management |
| 5 | COMPLETE | 4f1d7a0 | 2026-02-02 | Display System |
| 6 | COMPLETE | 79cd74d | 2026-02-02 | Input System |
| 7 | COMPLETE | eeb1e19 | 2026-02-02 | Timer/System |
| 8 | COMPLETE | 360b5bf | 2026-02-02 | Datafiles |
| 9 | COMPLETE | 6cc1c64 | 2026-02-02 | Final Integration |

## Summary Statistics

- **Total API Calls Analyzed:** 906+
- **Total Files Affected:** 63
- **Documentation Files Created:** 12+
- **Estimated Implementation Effort:** 54-73 hours (1.5-2 weeks)

## Documentation Index

1. `phase0_analysis.md` - Infrastructure analysis
2. `allegro5_compat.h` - Compatibility header (src/)
3. `color_system_migration.md` - Color system documentation
4. `blit_migration.md` - Blitting function analysis
5. `sprite_migration.md` - Sprite/rotation documentation
6. `primitives_migration.md` - Drawing primitives analysis
7. `bitmap_management_migration.md` - Bitmap functions
8. `display_system_migration.md` - Display/screen system
9. `input_system_migration.md` - Input system analysis
10. `timer_system_migration.md` - Timer/system functions
11. `datafile_migration.md` - Datafile system analysis
12. `migration_complete.md` - Final integration summary

---
Last Updated: 2026-02-02
Status: READY FOR IMPLEMENTATION
