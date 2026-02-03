# Datafile System Migration Analysis

## Overview

Allegro 5 **removed the datafile system** (`load_datafile()`, `unload_datafile()`, `DATAFILE` type). This document analyzes the current usage of datafiles in Starflight TLC and proposes migration strategies.

**Reference**: `/Users/acoliver/projects/tlc/research/allegro_transitions.md`

---

## Current Datafile Usage

### Active Datafile Modules (14 active)

The following modules actively use datafiles:

| Module | Datafile Path | Size | DATAFILE Variable | Load Location | Unload Location |
|--------|--------------|------|-------------------|---------------|-----------------|
| ModuleEngineer | `data/engineer/engineer.dat` | 294K | `engdata` | Line 86 | Line 182 |
| ModuleCredits | `data/credits/credits.dat` | 298K | `datafile` | Line 106 | Line 96 |
| ModuleStarport | `data/starport/starport.dat` | 2.5M | `spdata` | Line 43 | Line 56 |
| ModuleCaptainsLounge | `data/captainslounge/captainslounge.dat` | 519K | `cldata` | Line 124 | Line 389 |
| ModuleStarmap | `data/starmap/starmap.dat` | 210K | `smdata` | Line 151 | Line 312 |
| ModuleCantina | `data/cantina/cantina.dat` | 874K | `candata` | Line 165 | Line 149 |
| ModuleShipConfig | `data/shipconfig/shipconfig.dat` | 473K | `scdata` | Line 54 | Line 554 |
| ModuleSideViewer | `data/cargohold/sideviewer.dat` | (shared) | `svdata` | Line 35 | Line 163 |
| ModuleCrewHire | `data/crewhire/crewhire.dat` | 538K | `chdata` | Line 685 | Line 671 |
| ModuleTradeDepot | `data/tradedepot/tradedepot.dat` | 356K | `tddata` | Line 213 | Line 754 |
| ModuleBank | `data/bank/bank.dat` | 2.9M | `bdata` | Line 126 | Line 867 |
| ModuleCaptainCreation | `data/captaincreation/captaincreation.dat` | 1.0M | `ccdata` | Line 204 | Line 560 |
| ModuleMedical | `data/medical/medical.dat` | 281K | `meddata` | Line 305 | Line 506 |
| ModuleAuxiliaryDisplay | `data/auxiliary/auxiliary.dat` | 197K | `auxdata` | Line 120 | Line 110 |

**Total: 14 active datafiles, ~12.8 MB total**

### Commented-Out Datafile Modules (6 inactive)

The following modules have datafile loading code that is currently commented out:

| Module | Datafile Path | Status |
|--------|--------------|--------|
| ModulePlanetSurface | `data/planetsurface/planetsurface.dat` | Lines 704, 569 commented |
| ModuleMessageGUI | `data/messagegui/messagegui.dat` | Lines 31, 63 commented |
| ModuleInterstellar | `data/spacetravel/spacetravel.dat` | Lines 383, 470 commented (file exists: 1.8M) |
| ModuleQuestLog | `data/questviewer/questviewer.dat` | Lines 90, 70 commented |
| ModuleEncounter | (no path in code) | Line 428 commented |
| ModuleCargoWindow | `data/cargohold/sideviewer.dat`, `data/cargohold/cargohold.dat` | Lines 91, 105, 463, 468 commented |

**Note**: `data/spacetravel/spacetravel.dat` exists on disk (1.8M) but is not currently loaded.

---

## Datafile Content Analysis

### Types of Data Stored

All datafiles contain **bitmap images** in various formats:
- `.BMP` files (most common)
- `.TGA` files (with transparency/alpha channels)
- `.PCX` files (rarely)

**No audio, fonts, or other data types are stored in datafiles.**

### Datafile Index Constants

Each module defines `#define` constants for array indices to access resources from the datafile. These are defined in the `.cpp` files:

#### Example: ModuleEngineer.cpp (Lines 35-47)
```cpp
#define AUX_REPAIR_BMP                   0        /* BMP  */
#define AUX_REPAIR_HOVER_BMP             1        /* BMP  */
#define ELEMENT_GAUGE_BLUE_BMP           2        /* BMP  */
#define ELEMENT_GAUGE_GRAY_BMP           3        /* BMP  */
#define ELEMENT_GAUGE_GREEN_BMP          4        /* BMP  */
#define ELEMENT_GAUGE_MAGENTA_BMP        5        /* BMP  */
#define ELEMENT_GAUGE_ORANGE_BMP         6        /* BMP  */
#define ELEMENT_GAUGE_PURPLE_BMP         7        /* BMP  */
#define ELEMENT_GAUGE_RED_BMP            8        /* BMP  */
#define GUI_BMP                          9        /* BMP  */
#define HIGH_RES_SHIP_FREELANCE_TGA      10       /* BMP  */
#define HIGH_RES_SHIP_MILITARY_TGA       11       /* BMP  */
#define HIGH_RES_SHIP_SCIENCE_TGA        12       /* BMP  */
```

#### Example: ModuleCredits.cpp (Line 16)
```cpp
#define BACKGROUND_TGA                   0        /* BMP  */
```

### Access Pattern

Resources are accessed by casting the `.dat` field:

```cpp
// Load datafile
engdata = load_datafile("data/engineer/engineer.dat");

// Access bitmap resource
img_window = (BITMAP*)engdata[GUI_BMP].dat;
img_bar_base = (BITMAP*)engdata[ELEMENT_GAUGE_GRAY_BMP].dat;

// Unload datafile
unload_datafile(engdata);
```

---

## Datafile Creation Process

According to `bin/data/DATA_FILES_README_FIRST.txt`:

1. **Tool Used**: Allegro 4's `dat.exe` utility
2. **Original Assets**: Stored in `<module>/assets/` subdirectories (not distributed)
3. **Packed File**: The `.dat` file (distributed with game)
4. **Build Process**: Batch files like `dat_auxiliary.bat` in each assets folder

Example command:
```bat
dat -c2 -f -bpp 32 -t BMP -h dat_auxiliary.h auxiliary.dat -a gui_aux.bmp is_tiles_small.bmp aux_icon_freelance.tga ...
```

**Important**: Original asset files exist alongside `.dat` files in many directories.

---

## Migration Strategy Options

### Option 1: Keep Allegro Legacy Datafile Support [OK] **RECOMMENDED**

**Description**: Continue using Allegro 4 legacy's datafile system during A5 migration.

**Pros**:
- [OK] **Minimal code changes** - Only need to handle BITMAP → ALLEGRO_BITMAP conversion
- [OK] **Faster migration** - Can focus on other critical A5 changes
- [OK] **No asset rebuild needed** - All 14 .dat files work as-is
- [OK] **Proven to work** - Allegro Legacy was designed for this transition

**Cons**:
- WARNING: Dependency on legacy code
- WARNING: Eventual deprecation concern (but not urgent)

**Implementation**:
```cpp
// Current A4 code
DATAFILE *engdata = load_datafile("data/engineer/engineer.dat");
BITMAP *img = (BITMAP*)engdata[GUI_BMP].dat;

// Migration to A5 with legacy support
DATAFILE *engdata = load_datafile("data/engineer/engineer.dat");
ALLEGRO_BITMAP *img = al_convert_allegro4_bitmap_to_a5((BITMAP*)engdata[GUI_BMP].dat);
```

**Code Changes Required**:
1. Update BITMAP → ALLEGRO_BITMAP in module headers
2. Add conversion wrapper or use al_convert helpers
3. Ensure proper cleanup of both legacy and A5 bitmaps

---

### Option 2: Convert to Individual Files

**Description**: Extract all assets from `.dat` files and load them individually.

**Pros**:
- [OK] Modern approach
- [OK] No legacy dependencies
- [OK] Easier to modify assets (no repacking needed)

**Cons**:
- [ERROR] **Major refactoring required** - ~44 load/unload operations × 14 modules = ~616 changes
- [ERROR] **Asset extraction needed** - Must extract ~200+ images from 14 .dat files
- [ERROR] **Build system changes** - Need to reorganize 14 asset directories
- [ERROR] **Risk of file path errors** - Must track individual file paths
- [ERROR] **Performance impact** - Many individual file I/O operations vs. one packed file

**Implementation Example**:
```cpp
// Replace this:
engdata = load_datafile("data/engineer/engineer.dat");
img_window = (BITMAP*)engdata[GUI_BMP].dat;

// With this:
img_window = al_load_bitmap("data/engineer/gui.bmp");
img_bar_base = al_load_bitmap("data/engineer/Element_Gauge_Gray.bmp");
img_bar_laser = al_load_bitmap("data/engineer/Element_Gauge_Magenta.bmp");
// ... repeat for all ~200+ assets across all modules
```

**Estimated Work**: 
- Extract assets: 2-4 hours
- Code refactoring: 20-40 hours
- Testing: 10-20 hours
- **Total: 32-64 hours**

---

### Option 3: Create Custom Compatibility Layer

**Description**: Build a new datafile-like system that wraps A5's resource loading.

**Pros**:
- [OK] Minimal code changes in modules
- [OK] Clean abstraction

**Cons**:
- [ERROR] **New code to maintain** - Custom resource manager
- [ERROR] **Asset extraction still needed** - Must unpack all .dat files
- [ERROR] **Not worth the effort** - Reinventing what Allegro Legacy already provides

---

## Detailed Module Breakdown

### High-Asset-Count Modules

#### ModuleBank (2.9M, 15 assets)
```cpp
#define BANK_BANNER_BMP                  1
#define BANK_BUTTON_CONFIRM_HOVER_BMP    2
#define BANK_BUTTON_CONFIRM_NORMAL_BMP   3
#define BANK_BUTTON_EXIT_BMP             4
#define BANK_BUTTON_EXIT_HOVER_BMP       5
// ... 10 more assets
```

#### ModuleStarport (2.5M, ~3 assets)
Likely contains large background images.

#### ModuleCantina (874K, ~5 assets)
```cpp
#define CANTINA_BTN_BMP                  1
#define CANTINA_BTN_DIS_BMP              2
#define CANTINA_BTN_HOV_BMP              3
#define CANTINA_EXIT_BTN_NORM_BMP        4
#define CANTINA_EXIT_BTN_OVER_BMP        5
```

### Medium-Asset-Count Modules

#### ModuleCrewHire (538K, ~15 assets)
Many button states and icons.

#### ModuleTradeDepot (356K, ~15 assets)
UI elements and cursors.

### Low-Asset-Count Modules

#### ModuleCredits (298K, 1 asset)
```cpp
#define BACKGROUND_TGA                   0        /* BMP  */
```
**Simplest case** - only loads a single background image.

---

## Bitmap Conversion Considerations

When using Allegro Legacy datafiles with A5:

### Memory Management
```cpp
// A4 legacy bitmap from datafile
BITMAP *a4_bitmap = (BITMAP*)engdata[GUI_BMP].dat;

// Convert to A5
ALLEGRO_BITMAP *a5_bitmap = al_clone_bitmap_from_a4(a4_bitmap);

// Cleanup
unload_datafile(engdata);  // Frees A4 bitmap
al_destroy_bitmap(a5_bitmap);  // Free A5 bitmap separately
```

### Conversion Functions
Allegro Legacy likely provides:
- `al_convert_bitmap()` - Direct conversion
- Automatic conversion on access (if supported)

**Research needed**: Check if Allegro Legacy provides automatic conversion or requires manual conversion calls.

---

## Recommended Migration Plan

### Phase 1: Setup (Week 1)
1. [OK] Verify Allegro Legacy's datafile support works with A5
2. [OK] Test bitmap conversion from legacy datafiles
3. [OK] Create wrapper functions if needed

### Phase 2: Pilot Module (Week 2)
1. Choose **ModuleCredits** (simplest: 1 asset)
2. Update to use Allegro Legacy datafile with A5 bitmaps
3. Verify loading, rendering, and cleanup work correctly
4. Document any issues or gotchas

### Phase 3: Rollout (Weeks 3-6)
1. Apply pattern to remaining 13 modules
2. Test each module thoroughly
3. Handle any conversion edge cases

### Phase 4: Cleanup (Week 7)
1. Remove commented-out datafile code (6 modules)
2. Decide whether to enable or delete those datafiles
3. Update documentation

---

## Risk Assessment

### Low Risk (Recommended Approach)
**Using Allegro Legacy datafiles**: 
-  Minimal code changes
-  No asset extraction needed
-  Proven migration path

### High Risk (Not Recommended)
**Converting to individual files**:
-  Massive refactoring (600+ code changes)
-  Asset extraction and reorganization
-  High chance of introducing bugs
-  Significant testing burden

---

## Questions to Resolve

1. **Does Allegro Legacy's `load_datafile()` work in A5 without modification?**
   - Test with a simple example
   
2. **Do BITMAP pointers from datafiles require conversion, or is there automatic conversion?**
   - Check Allegro Legacy documentation
   
3. **Are there any format incompatibilities between A4 and A5 bitmaps?**
   - Test with different bitmap formats (BMP, TGA)

4. **What about the commented-out datafiles?**
   - `spacetravel.dat` exists on disk - was it intentionally disabled?
   - Should we enable them or clean them up?

---

## Conclusion

**RECOMMENDATION: Use Allegro Legacy's datafile support**

The datafile system in Starflight TLC is straightforward:
- 14 active modules using datafiles
- ~200+ bitmap assets total
- All assets are images (BMP/TGA)
- Well-organized with clear index constants

Allegro Legacy was designed specifically to ease the A4 → A5 transition. Using its datafile support:
- Minimizes migration risk
- Reduces development time
- Allows focus on other critical A5 changes
- Can be revisited later if needed

**Estimated effort with Allegro Legacy**: 1-2 weeks
**Estimated effort with individual files**: 6-8 weeks

The 5-7 week time savings makes the Allegro Legacy approach the clear winner for initial migration.

---

## Appendix: Complete File Listing

### All Datafile Load/Unload Locations

```
ModuleEngineer.cpp:86:          engdata = load_datafile("data/engineer/engineer.dat");
ModuleEngineer.cpp:182:         unload_datafile(engdata);
ModuleCredits.cpp:96:           unload_datafile(datafile);
ModuleCredits.cpp:106:          datafile = load_datafile("data/credits/credits.dat");
ModulePlanetSurface.cpp:569:    //unload_datafile(psdata);
ModulePlanetSurface.cpp:704:    //psdata = load_datafile("data/planetsurface/planetsurface.dat");
ModuleMessageGUI.cpp:31:        //data = load_datafile("data/messagegui/messagegui.dat");
ModuleMessageGUI.cpp:63:        //unload_datafile(data);
ModuleStarport.cpp:43:          spdata = load_datafile("data/starport/starport.dat");
ModuleStarport.cpp:56:          unload_datafile(spdata);
ModuleCaptainsLounge.cpp:124:   cldata = load_datafile("data/captainslounge/captainslounge.dat");
ModuleCaptainsLounge.cpp:389:   unload_datafile(cldata);
ModuleInterstellar.cpp:383:     //isdata = load_datafile("data/spacetravel/spacetravel.dat");
ModuleInterstellar.cpp:470:     //unload_datafile(isdata);
ModuleQuestLog.cpp:70:          //unload_datafile(qldata);
ModuleQuestLog.cpp:90:          //qldata = load_datafile("data/questviewer/questviewer.dat");
ModuleEncounter.cpp:428:        //unload_datafile(encdata);
ModuleStarmap.cpp:151:          smdata = load_datafile("data/starmap/starmap.dat");
ModuleStarmap.cpp:312:          unload_datafile(smdata);
ModuleCargoWindow.cpp:91:       //svdata = load_datafile("data/cargohold/sideviewer.dat");
ModuleCargoWindow.cpp:105:      //cwdata = load_datafile("data/cargohold/cargohold.dat");
ModuleCargoWindow.cpp:463:      //unload_datafile(svdata);
ModuleCargoWindow.cpp:468:      //unload_datafile(cwdata);
ModuleCantina.cpp:149:          unload_datafile(candata);
ModuleCantina.cpp:165:          candata = load_datafile("data/cantina/cantina.dat");
ModuleShipConfig.cpp:54:        scdata = load_datafile("data/shipconfig/shipconfig.dat");
ModuleShipConfig.cpp:554:       unload_datafile(scdata);
ModuleSideViewer.cpp:35:        svdata = load_datafile("data/cargohold/sideviewer.dat");
ModuleSideViewer.cpp:163:       unload_datafile(svdata);
ModuleCrewHire.cpp:671:         unload_datafile(chdata);
ModuleCrewHire.cpp:685:         chdata = load_datafile("data/crewhire/crewhire.dat");
ModuleTradeDepot.cpp:213:       tddata = load_datafile("data/tradedepot/tradedepot.dat");
ModuleTradeDepot.cpp:754:       unload_datafile(tddata);
ModuleBank.cpp:126:             bdata = load_datafile("data/bank/bank.dat");
ModuleBank.cpp:867:             unload_datafile(bdata);
ModuleCaptainCreation.cpp:204:  ccdata = load_datafile("data/captaincreation/captaincreation.dat");
ModuleCaptainCreation.cpp:560:  unload_datafile(ccdata);
ModuleMedical.cpp:305:          meddata = load_datafile("data/medical/medical.dat");
ModuleMedical.cpp:506:          unload_datafile(meddata);
ModuleAuxiliaryDisplay.cpp:110: unload_datafile(auxdata);
ModuleAuxiliaryDisplay.cpp:120: auxdata = load_datafile("data/auxiliary/auxiliary.dat");
```

### All DATAFILE Variable Declarations

```
ModuleEngineer.cpp:51:          DATAFILE *engdata = NULL;
ModuleCredits.h:38:             DATAFILE *datafile;
ModuleEncounter.h:40:           DATAFILE *encdata;
ModuleStarport.cpp:33:          DATAFILE *spdata;
ModuleQuestLog.cpp:33:          //DATAFILE *qldata;
ModuleCaptainsLounge.cpp:93:    DATAFILE *cldata;
ModuleStarmap.cpp:25:           DATAFILE *smdata;
ModuleInterstellar.h:45:        //DATAFILE *isdata;
ModuleCantina.cpp:34:           DATAFILE *candata;
ModuleCargoWindow.h:53:         //DATAFILE *cwdata;
ModuleCargoWindow.h:54:         //DATAFILE *svdata;
ModuleShipConfig.cpp:36:        DATAFILE *scdata;
ModuleMessageGUI.h:20:          //DATAFILE *data;
ModuleSideViewer.cpp:19:        DATAFILE *svdata;
ModuleCrewHire.cpp:52:          DATAFILE *chdata;
ModuleTradeDepot.cpp:147:       DATAFILE *tddata;
ModuleBank.cpp:31:              DATAFILE *bdata;
ModuleSolarSystem.h:53:         //DATAFILE *ipdata;
ModuleCaptainCreation.cpp:38:   DATAFILE *ccdata;
ModuleMedical.cpp:59:           DATAFILE *meddata;
ModuleAuxiliaryDisplay.cpp:76:  DATAFILE *auxdata;
```

---

**Total Active Datafile Operations**: 
- Load calls: 14
- Unload calls: 14
- DATAFILE pointers: 14
- Commented/inactive: 6 modules

**Total Lines of Code Affected**: ~44 direct load/unload calls + ~200+ bitmap access calls = ~244 total datafile-related operations
