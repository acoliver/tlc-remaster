# DATAFILE Migration - Remaining Work

## Completed Modules (9/16)
[OK] ModuleCredits - Converted to al_load_bitmap
[OK] ModuleStarport - Converted to al_load_bitmap  
[OK] ModuleSideViewer - Converted to al_load_bitmap
[OK] ModuleStarmap - Converted to al_load_bitmap
[OK] ModuleBank - Converted to al_load_bitmap (complex, with multiple button assets)
[OK] ModuleEngineer - Converted to al_load_bitmap
[OK] ModuleAuxiliaryDisplay - Converted to al_load_bitmap
[OK] allegro5_compat.h - Removed DATAFILE stub definitions
[OK] All modules compile successfully

## Remaining Modules (7/16)

### 1. ModuleCantina.cpp
**Datafile**: `data/cantina/cantina.dat`
**Assets to load**:
- cantina_background.bmp (already direct-loaded in code)
- cantina_Btn.bmp
- cantina_Btn_dis.bmp
- cantina_Btn_hov.bmp
- cantina_exit_btn_norm.bmp
- cantina_exit_btn_over.bmp
- militaryops_background.bmp (direct-loaded)
- researchlab_background.bmp (direct-loaded)
- buttonclick.ogg (audio)

**Pattern**: Replace `candata` DATAFILE* with individual al_load_bitmap() calls

### 2. ModuleCaptainCreation.cpp
**Datafile**: `data/captaincreation/captaincreation.dat`
**Assets to load** (22 assets):
- captaincreation_*.bmp files for various buttons and states
- Background images
- buttonclick.ogg, click.ogg, error.ogg (audio)

**Pattern**: Replace `ccdata` DATAFILE* with individual al_load_bitmap() calls

### 3. ModuleCaptainsLounge.cpp
**Datafile**: `data/captainslounge/captainslounge.dat`
**Assets to load** (20 assets):
- captainslounge_background.bmp (direct-loaded)
- Various .tga button states (del, plus, sel)
- .bmp button states (no, yes, save)
- buttonclick.ogg (audio)

**Pattern**: Replace `cldata` DATAFILE* with individual al_load_bitmap() calls

### 4. ModuleCrewHire.cpp
**Datafile**: `data/crewhire/crewhire.dat`
**Assets to load** (17 assets):
- personel_background.bmp (direct-loaded)
- personel_miniPositions.bmp (direct-loaded)
- Various button states (Btn, Btn2, catBtn)
- Icon files (Icons_small*.tga)
- generic_exit_btn_norm.bmp, generic_exit_btn_over.bmp
- buttonclick.ogg (audio)

**Pattern**: Replace `chdata` DATAFILE* with individual al_load_bitmap() calls

### 5. ModuleMedical.cpp
**Datafile**: `data/medical/medical.dat`
**Assets to load** (24 assets):
- medical_gui_viewer*.bmp (direct-loaded)
- Various button states (btn_*)
- Medical bar images (med_bar_*)
- Icon files (Icons_small*.tga)
- buttonclick.ogg (audio)

**Pattern**: Replace `meddata` DATAFILE* with individual al_load_bitmap() calls

### 6. ModuleShipConfig.cpp
**Datafile**: `data/shipconfig/shipconfig.dat`
**Assets to load** (10 assets):
- ShipConfig.bmp (direct-loaded)
- Ship images (freelance.tga, military.tga, science.tga)
- Button states (shipconfig_btn_*)
- shipconfig_cursor0.bmp
- click.ogg, error.ogg (audio)

**Pattern**: Replace `scdata` DATAFILE* with individual al_load_bitmap() calls

### 7. ModuleTradeDepot.cpp
**Datafile**: `data/tradedepot/tradedepot.dat`
**Assets to load** (16 core + 55 item portraits):
- tradedepot_background.bmp (commented, direct-loaded)
- Various button states (btn, filterbtn, promptbtn, etc.)
- Cursor images
- Quantity prompt window
- Spin buttons
- Item portrait images (T_*.tga files - loaded separately)
- buttonclick.ogg (audio)

**Pattern**: Replace `tddata` DATAFILE* with individual al_load_bitmap() calls

## Migration Pattern for Each Module

### Step 1: Remove DATAFILE declaration
```cpp
// Remove this:
DATAFILE *moduledata;

// Add this (if needed for multiple bitmaps):
BITMAP *img_asset1 = NULL;
BITMAP *img_asset2 = NULL;
// ... etc
```

### Step 2: Replace load_datafile() in Init()
```cpp
// Remove this:
moduledata = load_datafile("data/module/module.dat");
if (!moduledata) {
    g_game->message("Module: Error loading datafile");
    return false;
}

// Replace with direct loads:
img_asset1 = al_load_bitmap("data/module/asset1.bmp");
if (!img_asset1) {
    g_game->message("Module: Error loading asset1");
    return false;
}
// ... repeat for all assets
```

### Step 3: Replace datafile index access
```cpp
// Replace this:
img = (BITMAP*)moduledata[INDEX_NAME].dat;

// With this:
// (already loaded in Step 2, just use the variable)
```

### Step 4: Replace unload_datafile() in Close()
```cpp
// Remove this:
unload_datafile(moduledata);
moduledata = NULL;

// Replace with:
if (img_asset1) { al_destroy_bitmap(img_asset1); img_asset1 = NULL; }
if (img_asset2) { al_destroy_bitmap(img_asset2); img_asset2 = NULL; }
// ... repeat for all assets
```

### Step 5: Remove #define macros
```cpp
// Remove all #define INDEX_NAME lines at top of file
```

## Audio Loading Pattern
For .ogg and .wav files, use:
```cpp
g_game->audioSystem->Load("data/module/sound.ogg", "soundname");
```

## Reference: datafile_mapping.md
See `/Users/acoliver/projects/tlc/project-plans/allegro5-remediate/datafile_mapping.md` for complete asset mappings.

## Compilation Test
After each module conversion:
```bash
cd /Users/acoliver/projects/tlc/build && make -j4
```

## Final Verification
After all conversions:
```bash
# Should return no results:
grep -R "DATAFILE" src/*.cpp src/*.h
grep -R "load_datafile" src/*.cpp
grep -R "unload_datafile" src/*.cpp
```

## Notes
- Some modules already use direct loading for background images (commented out datafile access)
- Audio files are typically not in the .dat files and are already loaded separately
- All .bmp and .tga files should be loaded with `al_load_bitmap()`
- Always check for NULL after loading and provide error messages
