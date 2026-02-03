# Bitmap Management Migration - Allegro 4 to Allegro 5

## Document Purpose

This document catalogs all bitmap creation, destruction, manipulation, and dimension access patterns in the TLC codebase to facilitate migration from Allegro 4 API to Allegro 5 API.

**Status:** Analysis Complete  
**Total Bitmap Operations:** ~500+ occurrences  
**Date:** 2026-02-02  
**Related Documents:**
- `allegro_transitions.md` - Overall migration strategy
- `src/allegro5_compat.h` - Compatibility layer implementation

---

## Executive Summary

### Occurrence Counts

| Operation | Count | Complexity | Notes |
|-----------|-------|------------|-------|
| `create_bitmap()` | 47 | Medium | Temporary buffers, UI elements |
| `load_bitmap()` | 203 | Low | File loading, already has macro |
| `destroy_bitmap()` | 79 | Low | Memory cleanup |
| `create_sub_bitmap()` | 1 | Low | Single usage in ModuleMiniGame |
| `clear_bitmap()` | 7 | Low | Simple clearing operations |
| `clear_to_color()` | 21 | Medium | Transparency handling required |
| `->w` dimension access | 197 | **HIGH** | Requires function call conversion |
| `->h` dimension access | 185 | **HIGH** | Requires function call conversion |

**CRITICAL:** Bitmap dimension access (`->w`, `->h`) is the most pervasive pattern requiring migration, with **~380 occurrences** that must be converted from struct member access to function calls.

---

## Part 1: Bitmap Creation Functions

### 1.1 create_bitmap() - 47 Occurrences

#### Migration Pattern

```cpp
// Allegro 4 (Current)
BITMAP *buffer = create_bitmap(width, height);

// Allegro 5 (Target)
ALLEGRO_BITMAP *buffer = al_create_bitmap(width, height);

// The allegro5_compat.h already provides:
#define create_bitmap(w, h) al_create_bitmap(w, h)
```

#### Key Files and Usage Patterns

##### High-Impact Files

**ModulePlanetSurface.cpp** (5 occurrences)
- L1092: `minimap = create_bitmap(asw, ash);` - Planet minimap rendering
- Context: Minimap created at asteroid width/height dimensions

**PlanetSurfaceObject.cpp** (7 occurrences)
- L335, L345, L347, L367, L374, L388, L392, L396
- **Complex rotation/scaling pipeline:**
  ```cpp
  scrapFrame = create_bitmap(frameWidth, frameHeight);
  finalFrame = create_bitmap((int)(frameWidth * scale), (int)(frameHeight * scale));
  ```
- Multi-stage rendering: extract frame → rotate → scale → composite
- **Migration Risk:** High - complex temporary bitmap lifecycle

**PlanetTileScroller.cpp** (4 occurrences)
- L181-182: Tile generation with scratch buffer
  ```cpp
  tile = create_bitmap(tileWidth, tileHeight);
  BITMAP *scratch = create_bitmap(tileWidth, tileHeight);
  ```
- L263: Main scroll buffer `scrollbuffer = create_bitmap(Width + (tileWidth * 2), Height + (tileHeight * 2));`
- **Pattern:** Create → Blit/composite → Cache → Destroy

**ScrollBox.cpp** (7 occurrences)
- L53-69: Creates three state buffers (normal, hover, selected) plus main buffer
  ```cpp
  sbBuffer = create_bitmap(sbWidth, sbHeight);
  sbNormal = create_bitmap(sbWidth - 16, sbFontHeight);
  sbHover = create_bitmap(sbWidth - 16, sbFontHeight);
  sbSelected = create_bitmap(sbWidth - 16, sbFontHeight);
  ```
- UI rendering pattern with state-based graphics

##### Medium-Impact Files

**ModuleSolarSystem.cpp** (3 occurrences)
- L974: `planetImage = (BITMAP*)create_bitmap(256,256);` - Planet rendering
- L992: `BITMAP* scratch = (BITMAP*)create_bitmap(256,256);` - Temporary for compositing
- Pattern: Create → Clear to transparent → Render → Copy → Destroy

**ModuleStarmap.cpp** (3 occurrences)
- L185-191: Three viewer bitmaps
  ```cpp
  starview = create_bitmap(MAP_WIDTH, MAP_HEIGHT);
  flux_view = create_bitmap(MAP_WIDTH, MAP_HEIGHT);
  text = create_bitmap(VIEWER_WIDTH, VIEWER_HEIGHT);
  ```

**Sprite.cpp** (3 occurrences)
- L180, L222, L259: Temporary buffers for rotation/scaling
- **Complex Pattern:**
  ```cpp
  BITMAP* temp = create_bitmap(this->getWidth(), this->getHeight());
  clear_to_color(temp, makecol(255,0,255));  // Transparent magenta
  rotate_sprite(temp, this->image, 0, 0, itofix((int)(angle / 0.7f / 2.0f)));
  ```

##### Low-Impact Files

- **Game.cpp** (L826): `m_backbuffer = create_bitmap(SCREEN_WIDTH,SCREEN_HEIGHT);`
- **ModuleEngineer.cpp** (L135): `text = create_bitmap(VIEWER_WIDTH, VIEWER_HEIGHT);`
- **MessageBoxWindow.cpp** (L197): `BITMAP *temp = create_bitmap(width, height);`
- **ModuleEncounter.cpp** (L622): `minimap = create_bitmap(asw, ash);`
- **PlanetaryBody.cpp** (L136, L161, L177, L192): Planet textures and maps
- **ModuleCrewHire.cpp** (L896, L903, L906, L909): Position icons and temp buffers
- **TileScroller.cpp** (L96): Scroll buffer
- **MiniWindow.cpp** (L87): Window buffer
- **Label.cpp** (L27): Label rendering buffer
- **ModuleStartup.cpp** (L46): Fader bitmap

#### Migration Considerations

1. **Memory Location:**
   - Allegro 5 creates GPU-backed bitmaps by default
   - For CPU-side pixel manipulation, call before creation:
     ```cpp
     al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
     ALLEGRO_BITMAP *cpu_bmp = al_create_bitmap(w, h);
     ```

2. **Initialization State:**
   - Allegro 4 bitmaps are uninitialized (random data)
   - Allegro 5 bitmaps may be cleared to transparent black
   - Pattern: Always clear after creation if specific color needed

3. **Error Handling:**
   ```cpp
   // Allegro 5 - always check for NULL
   ALLEGRO_BITMAP *bmp = al_create_bitmap(w, h);
   if (!bmp) {
       // Handle error
   }
   ```

---

### 1.2 load_bitmap() - 203 Occurrences

#### Migration Pattern

```cpp
// Allegro 4 (Current)
BITMAP *img = load_bitmap("file.bmp", NULL);  // NULL = palette (unused in 32-bit)

// Allegro 5 (Target)
ALLEGRO_BITMAP *img = al_load_bitmap("file.bmp");

// The allegro5_compat.h already provides:
#define load_bitmap(filename, pal) al_load_bitmap(filename)
```

#### Distribution by Module

| Module | Count | Primary Assets |
|--------|-------|----------------|
| ModulePlanetSurface.cpp | 80+ | Tilesets, UI gauges, buttons |
| ModuleEncounter.cpp | 20+ | Weapons, explosions, powerups |
| ModuleTitleScreen.cpp | 12 | Menu buttons (normal/mouseover) |
| ModuleCaptainCreation.cpp | 16 | UI buttons, backgrounds |
| ModuleMedical.cpp | 16 | Skill bars, viewers |
| ModuleControlPanel.cpp | 10 | Button states, icons |
| ModuleTradeDepot.cpp | 7 | Item portraits |
| Button.cpp | 6 | Generic button loading |
| Others | 36+ | Various module assets |

#### Typical Patterns

**UI Button Loading (3-state pattern):**
```cpp
// ModuleTitleScreen.cpp L58-59
imgNormal = (BITMAP*)load_bitmap("data/titlescreen/title_normal.tga",NULL);
imgMouseOver = (BITMAP*)load_bitmap("data/titlescreen/title_over.tga",NULL);
```

**Tileset Loading (ModulePlanetSurface.cpp):**
```cpp
// L1315 - Example of many similar calls
if (!scroller->LoadTileSet( (BITMAP*)load_bitmap("data/planetsurface/TileSet_Ash.tga",NULL), 16))
```
- **Pattern:** Load → Pass to LoadTileSet → Cache internally
- Over 60 tileset loads for different planet types

**Resource Loading with Casting:**
```cpp
// Common pattern throughout codebase
starport = (BITMAP*)load_bitmap("data/starport/starport.bmp",NULL);
```
- **Note:** Casting to `(BITMAP*)` is unnecessary and should be removed during migration

#### File Format Support

**Current Usage:**
- `.bmp` - Most common, basic format
- `.tga` - Common for UI elements with alpha
- `.png` - Some usage (requires Allegro image addon)

**Migration Requirements:**
```cpp
// Must initialize image addon for non-BMP formats
al_init_image_addon();
```

#### Transparency Handling

**Critical Pattern - Magenta Transparency:**
```cpp
// Many assets use magenta (255,0,255) as transparent color
// After loading in Allegro 5, convert mask to alpha:
ALLEGRO_BITMAP *sprite = al_load_bitmap("sprite.bmp");
al_convert_mask_to_alpha(sprite, al_map_rgb(255, 0, 255));
```

**Files Affected:** Most `.bmp` sprite files (scanning comments suggests heavy magenta usage)

#### Error Handling

Current code rarely checks for NULL after loading. Allegro 5 migration should add checks:
```cpp
ALLEGRO_BITMAP *img = al_load_bitmap(filename);
if (!img) {
    g_game->message("Failed to load: " + std::string(filename));
    return false;
}
```

---

### 1.3 destroy_bitmap() - 79 Occurrences

#### Migration Pattern

```cpp
// Allegro 4 (Current)
destroy_bitmap(bitmap);

// Allegro 5 (Target)
al_destroy_bitmap(bitmap);

// The allegro5_compat.h already provides:
#define destroy_bitmap(bmp) al_destroy_bitmap(bmp)
```

#### Usage Patterns by Context

##### Cleanup in Destructors/Shutdown

**PlanetSurfaceObject.cpp** (L675)
```cpp
// Cleanup of frame cache
for(it = frames.begin(); it != frames.end(); ++it) {
    destroy_bitmap(it->second);
}
```

**Game.cpp** (L133-134, L822, L1050)
```cpp
// Shutdown cleanup
destroy_bitmap(MessageBoxWindow::bg);
destroy_bitmap(MessageBoxWindow::bar);
destroy_bitmap(m_backbuffer);
```

##### Temporary Buffer Pattern

**Very Common Pattern:**
```cpp
BITMAP *temp = create_bitmap(w, h);
// ... use temp ...
destroy_bitmap(temp);
```

**Examples:**
- **Sprite.cpp** L180-199: Create temp → rotate → draw → destroy
- **PlanetSurfaceObject.cpp** L335-340: Create scrap → composite → destroy
- **MessageBoxWindow.cpp** L197-207: Create temp → stretch → destroy
- **ModuleSolarSystem.cpp** L992-1003: Create scratch → render → destroy

##### Lifecycle Management

**ScrollBox.cpp** (L131-151)
```cpp
// Component cleanup in destructor
destroy_bitmap(sbNormal);
destroy_bitmap(sbHover);
destroy_bitmap(sbSelected);
destroy_bitmap(sbScrollBar);
destroy_bitmap(sbBuffer);
```

**ModuleCrewHire.cpp** (L617-619)
```cpp
// Array cleanup
for(i=0; i<8; i++) {
    destroy_bitmap(posNormImages[i]);
    destroy_bitmap(posOverImages[i]);
    destroy_bitmap(posDisImages[i]);
}
```

#### Migration Considerations

1. **NULL Safety:**
   - Allegro 5's `al_destroy_bitmap()` handles NULL safely
   - Current code sometimes checks, sometimes doesn't
   - Allegro 5 pattern: Just call, no check needed

2. **Sub-bitmap Warning:**
   - Sub-bitmaps must be destroyed BEFORE parent
   - Allegro 5 doesn't enforce this, can cause crashes
   - **Action:** Document sub-bitmap relationships

3. **Double-Free Protection:**
   - Allegro 5 doesn't protect against double-free
   - Pattern: Set to NULL after destroy
     ```cpp
     al_destroy_bitmap(bmp);
     bmp = NULL;
     ```

---

### 1.4 create_sub_bitmap() - 1 Occurrence

#### Single Usage

**ModuleMiniGame.cpp** (L101)
```cpp
window = create_sub_bitmap(g_game->GetBackBuffer(), 192, 144, 640, 480);
```

#### Migration Pattern

```cpp
// Allegro 4 (Current)
BITMAP *sub = create_sub_bitmap(parent, x, y, w, h);

// Allegro 5 (Target)
ALLEGRO_BITMAP *sub = al_create_sub_bitmap(parent, x, y, w, h);
```

#### Critical Differences

1. **Lifetime Management:**
   - Sub-bitmap MUST be destroyed before parent
   - Allegro 5 doesn't track this automatically
   - Destroy order is critical

2. **Clipping Region:**
   - Sub-bitmaps create a clipping region on parent
   - Drawing to sub-bitmap affects parent
   - More efficient than creating new bitmap and copying

3. **Memory:**
   - No additional memory allocated
   - Just a view into parent bitmap's memory

#### Migration Note

This single usage appears to be creating a window into the backbuffer. In Allegro 5, consider using clipping instead:

```cpp
// Alternative approach in Allegro 5
al_set_clipping_rectangle(x, y, w, h);
// ... draw ...
al_reset_clipping_rectangle();
```

---

## Part 2: Bitmap Clearing and Color Fill

### 2.1 clear_bitmap() - 7 Occurrences

#### Migration Pattern

```cpp
// Allegro 4 (Current)
clear_bitmap(bmp);  // Clears to black (0,0,0)

// Allegro 5 (Target)
al_set_target_bitmap(bmp);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));  // Clear to transparent black

// The allegro5_compat.h provides macro that handles target switching
```

#### All Occurrences

| File | Line | Context |
|------|------|---------|
| ModulePlanetSurface.cpp | L2249 | `clear_bitmap(minimap);` |
| PlanetSurfaceObject.cpp | L401 | `clear_bitmap(finalFrame);` - Before rotation |
| PlanetaryBody.cpp | L178 | `clear_bitmap(this->planetTopography);` |
| ScrollBox.cpp | L100 | `clear_bitmap(sbNormal);` - Before rendering state |
| ScrollBox.cpp | L107 | `clear_bitmap(sbHover);` |
| ScrollBox.cpp | L114 | `clear_bitmap(sbSelected);` |
| allegro5_compat.h | L109 | Macro definition |

#### Usage Pattern Analysis

**Typical Pattern:**
```cpp
BITMAP *temp = create_bitmap(w, h);
clear_bitmap(temp);  // Ensure clean slate
// ... render to temp ...
```

**Migration Consideration:**
- Allegro 5 bitmaps may not need explicit clearing (often initialized to transparent)
- If specific color needed, use `clear_to_color()` instead

---

### 2.2 clear_to_color() - 21 Occurrences

#### Migration Pattern

```cpp
// Allegro 4 (Current)
clear_to_color(bmp, makecol(r, g, b));

// Allegro 5 (Target)
al_set_target_bitmap(bmp);
al_clear_to_color(al_map_rgb(r, g, b));

// With alpha:
al_clear_to_color(al_map_rgba(r, g, b, a));
```

#### Transparency Pattern (CRITICAL)

**The Magenta Transparency Problem:**

Many files use `clear_to_color(bmp, makecol(255,0,255))` to create transparent areas.

**Occurrences:**
- ModuleEngineer.cpp L136, L349
- ModuleSolarSystem.cpp L979, L993
- ModuleStarmap.cpp L189, L192, L362
- Label.cpp L41
- Sprite.cpp L181
- MiniWindow.cpp L88

**Allegro 4 Pattern:**
```cpp
BITMAP *temp = create_bitmap(w, h);
clear_to_color(temp, makecol(255, 0, 255));  // Magenta = transparent
// ... draw on temp ...
masked_blit(temp, dest, ...);  // Magenta pixels not drawn
```

**Allegro 5 Migration:**
```cpp
ALLEGRO_BITMAP *temp = al_create_bitmap(w, h);
al_set_target_bitmap(temp);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));  // Proper transparency
// ... draw on temp ...
al_draw_bitmap(temp, x, y, 0);  // Alpha automatically handled
```

#### Solid Color Clearing

**Black Clear:**
- ModulePlanetSurface.cpp L1971: `clear_to_color(g_game->GetBackBuffer(), BLACK);`
- ModuleEncounter.cpp L387, L2890: `clear_to_color(..., BLACK);`
- ModuleStarmap.cpp L186, L197: `clear_to_color(starview, BLACK);`
- ScrollBox.cpp L179: `clear_to_color(sbBuffer, BLACK);`

**Other Colors:**
- PlanetTileScroller.cpp L276: `clear_to_color(scrollbuffer, BLUE);` - Debug visualization

#### Complete Listing

| File | Line | Color | Purpose |
|------|------|-------|---------|
| ModuleEngineer.cpp | L136 | Magenta | Transparent text buffer |
| ModuleEngineer.cpp | L349 | Magenta | Transparent text buffer |
| ModulePlanetSurface.cpp | L1971 | BLACK | Clear backbuffer |
| ModuleEncounter.cpp | L387 | BLACK | Clear backbuffer |
| ModuleEncounter.cpp | L2890 | BLACK | Clear minimap |
| ModuleStarport.cpp | L537 | BLACK | Clear backbuffer |
| ModuleSolarSystem.cpp | L979 | Magenta | Transparent planet image |
| ModuleSolarSystem.cpp | L993 | Magenta | Transparent scratch |
| ModuleStarmap.cpp | L186 | BLACK | Clear starview |
| ModuleStarmap.cpp | L189 | Magenta | Transparent flux view |
| ModuleStarmap.cpp | L192 | Magenta | Transparent text |
| ModuleStarmap.cpp | L197 | BLACK | Clear starview |
| ModuleStarmap.cpp | L362 | Magenta | Transparent text |
| Label.cpp | L41 | Magenta | Transparent label |
| Sprite.cpp | L181 | Magenta | Transparent rotation buffer |
| MiniWindow.cpp | L88 | PINK (Magenta) | Transparent window |
| ScrollBox.cpp | L179 | BLACK | Clear scroll buffer |
| PlanetTileScroller.cpp | L276 | BLUE | Debug background |

#### Migration Action Items

1. **Replace all magenta clears with proper alpha:**
   ```cpp
   al_clear_to_color(al_map_rgba(0, 0, 0, 0));
   ```

2. **Verify blend modes are set correctly:**
   ```cpp
   al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
   ```

3. **Test all UI elements with transparency** to ensure proper rendering

---

## Part 3: Bitmap Dimension Access (CRITICAL SECTION)

### 3.1 Bitmap->w Access - 197 Occurrences

This is the **most pervasive** migration challenge. Direct struct member access must become function calls.

#### Migration Pattern

```cpp
// Allegro 4 (Current)
int width = bitmap->w;
int half_width = bitmap->w / 2;
if (x + bitmap->w > screen_width) { ... }

// Allegro 5 (Target)
int width = al_get_bitmap_width(bitmap);
int half_width = al_get_bitmap_width(bitmap) / 2;
if (x + al_get_bitmap_width(bitmap) > screen_width) { ... }
```

#### High-Frequency Files

| File | Occurrences | Complexity |
|------|-------------|------------|
| ModulePlanetSurface.cpp | 35+ | Very High - Complex UI layout |
| ModuleCaptainCreation.cpp | 45+ | Very High - Button positioning |
| ModuleMedical.cpp | 12 | High - Skill bar rendering |
| ModuleEngineer.cpp | 14 | Medium - Gauge bars |
| ModuleTopGUI.cpp | 5 | Low - Simple gauge fills |
| MiniWindow.cpp | 20+ | High - Window border tiling |
| Sprite.cpp | 5 | Medium - Rotation/scaling |
| Button.cpp | 2 | Low - Width getter, hit detection |

#### Usage Pattern Categories

##### Category 1: Simple Property Access

**Pattern:** Read dimension once, use in calculation

```cpp
// Current
int w = background->w;
int h = background->h;
blit(background, dest, 0, 0, 0, 0, background->w, background->h);

// Migrated
int w = al_get_bitmap_width(background);
int h = al_get_bitmap_height(background);
al_draw_bitmap(background, 0, 0, 0);  // Note: Allegro 5 draws full bitmap by default
```

**Examples:**
- ModuleCredits.cpp L139
- ModuleCantina.cpp L374
- ModuleTitleScreen.cpp L119

##### Category 2: Percentage/Ratio Calculations

**Pattern:** Multiply dimension by percentage for progress bars

```cpp
// Current - ModuleEngineer.cpp L298
masked_blit(img_bar_laser, g_game->GetBackBuffer(), 0, 0, 580, 135, 
            img_bar_laser->w * percentage, img_bar_base->h);

// Migrated
al_set_target_backbuffer(display);
al_draw_bitmap_region(img_bar_laser, 0, 0,
                      al_get_bitmap_width(img_bar_laser) * percentage,
                      al_get_bitmap_height(img_bar_base),
                      580, 135, 0);
```

**Files with this pattern:**
- ModuleEngineer.cpp (L298, L301, L304, L311, L314, L317) - 6 gauge bars
- ModulePlanetSurface.cpp (L2147, L2151, L2155, L2160, L2163, L2201, L2207) - 7 bars
- ModuleTopGUI.cpp (L117-120) - 4 gauge bars
- ModuleMedical.cpp (L901, L909, L917, L925, L933, L941, L949, L957, L965) - 9 skill bars

**Migration Consideration:** Cache width if used multiple times in tight loop
```cpp
int bar_width = al_get_bitmap_width(img_bar);
// Use bar_width multiple times
```

##### Category 3: Boundary Checking / Collision

**Pattern:** Check if position + width exceeds bounds

```cpp
// Current - ModuleStarport.cpp L131
if (g_game->gameState->player->posStarport.x + SCREEN_WIDTH >= starport->w)

// Migrated
if (g_game->gameState->player->posStarport.x + SCREEN_WIDTH >= 
    al_get_bitmap_width(starport))
```

**Examples:**
- ModuleStarport.cpp L131, L160, L170, L171, L424, L543, L546, L549, L570

##### Category 4: Layout/Positioning

**Pattern:** Position elements relative to bitmap dimensions

```cpp
// Current - ModulePlanetSurface.cpp L1002
cbx += btnNormal->w;  // Increment X position by button width

// Migrated
cbx += al_get_bitmap_width(btnNormal);
```

**Examples:**
- ModulePlanetSurface.cpp L1002, L1004, L1010, L1012, L1018, L1020, L1053
- ModuleCaptainCreation.cpp (45+ occurrences for button grid layout)

##### Category 5: Centering Calculations

**Pattern:** Calculate center position

```cpp
// Current - PauseMenu.cpp L26
x = 512 - bg->w/2;

// Migrated
x = 512 - al_get_bitmap_width(bg)/2;
```

**Examples:**
- PauseMenu.cpp L26
- ModuleEngineer.cpp L411, L413 (center text over gauge)

##### Category 6: Looping/Tiling

**Pattern:** Tile bitmap across width

```cpp
// Current - MiniWindow.cpp L91
for (int a = mwCorner->w; a < mwWidth; a += mwSide->w)
    draw_sprite_v_flip(buffer, mwSide, a, 0);

// Migrated
int corner_w = al_get_bitmap_width(mwCorner);
int side_w = al_get_bitmap_width(mwSide);
for (int a = corner_w; a < mwWidth; a += side_w) {
    al_draw_bitmap(mwSide, a, 0, ALLEGRO_FLIP_VERTICAL);
}
```

**Files:** MiniWindow.cpp L91, L94, L98, L101, L124, L126

##### Category 7: Minimap/Scaling

**Pattern:** Scale coordinates to minimap dimensions

```cpp
// Current - ModulePlanetSurface.cpp L2259
float x = playerShip->getX() / (scroller->getTilesAcross() * scroller->getTileWidth()) * minimap->w;

// Migrated
float x = playerShip->getX() / (scroller->getTilesAcross() * scroller->getTileWidth()) 
          * al_get_bitmap_width(minimap);
```

**Examples:**
- ModulePlanetSurface.cpp L2259, L2267, L2290
- ModulePlanetOrbit.cpp L658

##### Category 8: Hit Detection

**Pattern:** Check if mouse is within bitmap bounds

```cpp
// Current - Button.cpp L270
if ((initX >= x) && (initX < (x + imgNormal->w)) && 
    (initY >= y) && (initY < (y + imgNormal->h)))

// Migrated
if ((initX >= x) && (initX < (x + al_get_bitmap_width(imgNormal))) && 
    (initY >= y) && (initY < (y + al_get_bitmap_height(imgNormal))))
```

**Files:** 
- Button.cpp L270
- ModuleCaptainCreation.cpp (20+ hit detection blocks)
- ModuleControlPanel.cpp L874, L875

##### Category 9: Width/Height in Create/Copy

**Pattern:** Create new bitmap same size as another

```cpp
// Current - ModuleCrewHire.cpp L903
posNormImages[i] = create_bitmap(btnNorm->w, btnNorm->h);

// Migrated
posNormImages[i] = al_create_bitmap(al_get_bitmap_width(btnNorm), 
                                     al_get_bitmap_height(btnNorm));
```

**Examples:**
- ModuleCrewHire.cpp L903, L906, L909

##### Category 10: Texture Coordinate Mapping

**Pattern:** Map texture coordinates

```cpp
// Current - TexturedSphere.cpp L153
x = i * bmp->w / TEX_SIZE;

// Migrated
x = i * al_get_bitmap_width(bmp) / TEX_SIZE;
```

#### Migration Strategy for ->w

**Approach 1: Simple Find/Replace (NOT RECOMMENDED)**
- Too error-prone
- May catch non-bitmap `->w` patterns
- Requires manual verification of every instance

**Approach 2: Cached Dimensions (RECOMMENDED)**

For functions using dimension multiple times:
```cpp
void RenderGauges() {
    // Cache dimensions at function start
    int bar_w = al_get_bitmap_width(img_bar);
    int bar_h = al_get_bitmap_height(img_bar);
    
    // Use cached values
    al_draw_bitmap_region(img_bar, 0, 0, bar_w * percentage, bar_h, x, y, 0);
}
```

**Approach 3: Helper Macros (TEMPORARY BRIDGE)**

In allegro5_compat.h, could add:
```cpp
// TEMPORARY - Remove after full migration
#define BMP_W(bmp) al_get_bitmap_width(bmp)
#define BMP_H(bmp) al_get_bitmap_height(bmp)
```

But this just delays the inevitable. Better to migrate properly.

---

### 3.2 Bitmap->h Access - 185 Occurrences

Same patterns as `->w`, just for height dimension. See above categories.

#### Unique ->h Patterns

**Vertical Scrolling Boundary:**
```cpp
// ModulePlanetOrbit.cpp L659
int ph = pbody->planetTopography->h-7;
```

**Vertical Tiling:**
```cpp
// MiniWindow.cpp L98
for (int a = mwCorner->h; a < mwHeight; a += mwSide->h)
```

**Vertical Positioning:**
```cpp
// ModuleSettings.cpp L71
10, SCREEN_HEIGHT-imgNorm->h-10,
```

---

### 3.3 Combined w/h Access Patterns

**Very Common Pattern:** Both dimensions accessed together

```cpp
// Current
blit(background, dest, 0, 0, 0, 0, background->w, background->h);

// Migrated Option 1: Cache both
int w = al_get_bitmap_width(background);
int h = al_get_bitmap_height(background);
al_draw_bitmap_region(background, 0, 0, w, h, 0, 0, 0);

// Migrated Option 2: Just draw full bitmap
al_draw_bitmap(background, 0, 0, 0);
```

#### Files with Heavy Combined Access

| File | Combined w+h Access | Context |
|------|---------------------|---------|
| ModulePlanetSurface.cpp | 25+ pairs | UI layout, gauge rendering |
| ModuleEngineer.cpp | 14 pairs | Gauge bars |
| ModuleCaptainCreation.cpp | 30+ pairs | Button grid |
| MiniWindow.cpp | 12 pairs | Window border tiling |
| Sprite.cpp | 4 pairs | Rotation/scaling pipeline |

---

## Part 4: Complex Usage Patterns

### 4.1 Rotation Pipeline (Sprite.cpp, PlanetSurfaceObject.cpp)

**Current Complex Pattern:**
```cpp
// Sprite.cpp L177-199
void Sprite::DrawScaledRotated(BITMAP *dest, double scaling, int angle)
{
    BITMAP* temp = create_bitmap(this->getWidth(), this->getHeight());
    clear_to_color(temp, makecol(255,0,255));
    
    rotate_sprite(temp, this->image, 0, 0, itofix((int)(angle / 0.7f / 2.0f)));
    
    int w = (int)(temp->w * scaling);
    int h = (int)(temp->h * scaling);
    masked_stretch_blit(temp, dest, 0, 0, temp->w, temp->h, 
                        (int)this->x, (int)this->y, w, h);
    
    destroy_bitmap(temp);
}
```

**Migration Challenges:**
1. Create temp buffer ([OK] straightforward)
2. Clear to magenta (→ clear to transparent alpha)
3. Rotate with fixed-point angle (→ convert to radians)
4. Access `temp->w` and `temp->h` (→ function calls)
5. Masked stretch blit (→ al_draw_scaled_rotated_bitmap)

**Allegro 5 Simplified:**
```cpp
void Sprite::DrawScaledRotated(ALLEGRO_BITMAP *dest, double scaling, int angle)
{
    float radians = angle * ALLEGRO_PI / 180.0f;
    int w = this->getWidth();
    int h = this->getHeight();
    
    al_set_target_bitmap(dest);
    al_draw_scaled_rotated_bitmap(
        this->image,
        w / 2.0f, h / 2.0f,           // Center of rotation on source
        this->x + w/2, this->y + h/2, // Destination position
        scaling, scaling,              // X and Y scale
        radians,                       // Angle in radians
        0                              // Flags
    );
}
```

**Key Advantage:** Allegro 5 can do this in ONE call, no temp buffer needed!

### 4.2 Tile Caching (PlanetTileScroller.cpp)

**Current Pattern:**
```cpp
// L181-215
tile = create_bitmap(tileWidth, tileHeight);
BITMAP *scratch = create_bitmap(tileWidth, tileHeight);

blit(tiles[BaseTileSet]->getTiles(), tile, sx, sy, 0, 0, tileWidth, tileHeight);

// Composite accessories
blit(tiles[pdValues[i]]->getTiles(), scratch, ax, ay, 0, 0, tileWidth, tileHeight);
draw_trans_sprite(tile, scratch, 0, 0);

destroy_bitmap(scratch);
tileImageCache[key] = tile;  // Cache for reuse
```

**Migration Considerations:**
1. Two temporary bitmaps created
2. Multiple blit operations
3. Transparency compositing (`draw_trans_sprite`)
4. Cached result (tile lifetime managed by cache)

**Allegro 5 Pattern:**
```cpp
ALLEGRO_BITMAP *tile = al_create_bitmap(tileWidth, tileHeight);
ALLEGRO_BITMAP *scratch = al_create_bitmap(tileWidth, tileHeight);

al_set_target_bitmap(tile);
al_draw_bitmap_region(tiles[BaseTileSet]->getTiles(), sx, sy, 
                      tileWidth, tileHeight, 0, 0, 0);

// Composite accessories
al_set_target_bitmap(scratch);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));
al_draw_bitmap_region(tiles[pdValues[i]]->getTiles(), ax, ay,
                      tileWidth, tileHeight, 0, 0, 0);

al_set_target_bitmap(tile);
al_draw_bitmap(scratch, 0, 0, 0);  // Alpha blend automatically

al_destroy_bitmap(scratch);
tileImageCache[key] = tile;
```

### 4.3 UI State Buffers (ScrollBox.cpp)

**Pattern:** Pre-render UI states for fast switching

```cpp
// L53-116
sbBuffer = create_bitmap(sbWidth, sbHeight);
sbNormal = create_bitmap(sbWidth - 16, sbFontHeight);
sbHover = create_bitmap(sbWidth - 16, sbFontHeight);
sbSelected = create_bitmap(sbWidth - 16, sbFontHeight);

clear_bitmap(sbNormal);
rect(sbNormal, 0, 0, sbNormal->w-1, sbNormal->h-1, ColorItemBorder);
rectfill(sbNormal, 1, 1, sbNormal->w - 2, sbNormal->h - 2, ColorBackground);

clear_bitmap(sbHover);
rect(sbHover, 0, 0, sbHover->w-1, sbHover->h-1, ColorItemBorder);
rectfill(sbHover, 1, 1, sbHover->w - 2, sbHover->h - 2, ColorControls);

clear_bitmap(sbSelected);
rect(sbSelected, 0, 0, sbSelected->w-1, sbSelected->h-1, ColorSelectedHighlight);
rectfill(sbSelected, 1, 1, sbSelected->w - 2, sbSelected->h - 2, ColorSelectedBackground);
```

**Migration:**
```cpp
sbBuffer = al_create_bitmap(sbWidth, sbHeight);
sbNormal = al_create_bitmap(sbWidth - 16, sbFontHeight);
sbHover = al_create_bitmap(sbWidth - 16, sbFontHeight);
sbSelected = al_create_bitmap(sbWidth - 16, sbFontHeight);

// Cache dimensions (used 3x each)
int item_w = al_get_bitmap_width(sbNormal);
int item_h = al_get_bitmap_height(sbNormal);

al_set_target_bitmap(sbNormal);
al_clear_to_color(ColorBackground);
al_draw_rectangle(0, 0, item_w-1, item_h-1, ColorItemBorder, 1.0f);
al_draw_filled_rectangle(1, 1, item_w - 2, item_h - 2, ColorBackground);

// Similar for hover and selected...
```

### 4.4 Minimap Generation (ModulePlanetSurface.cpp)

**Pattern:** Stretch planet texture to minimap size

```cpp
// L2249-2256
clear_bitmap(minimap);
masked_stretch_blit(
    this->pbody->planetTexture500,
    minimap,
    0, 0,
    this->pbody->planetTexture500->w, this->pbody->planetTexture500->h,
    0, 0,
    minimap->w, minimap->h
);
```

**Migration:**
```cpp
int src_w = al_get_bitmap_width(this->pbody->planetTexture500);
int src_h = al_get_bitmap_height(this->pbody->planetTexture500);
int dst_w = al_get_bitmap_width(minimap);
int dst_h = al_get_bitmap_height(minimap);

al_set_target_bitmap(minimap);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));
al_draw_scaled_bitmap(
    this->pbody->planetTexture500,
    0, 0, src_w, src_h,
    0, 0, dst_w, dst_h,
    0
);
```

---

## Part 5: Migration Priorities and Risks

### 5.1 High-Risk Files

These files have complex bitmap manipulation requiring careful migration:

1. **PlanetSurfaceObject.cpp**
   - Risk: Multi-stage rotation/scaling pipeline
   - Bitmap Access: 14 occurrences of ->w/->h
   - Temp Buffers: 7 create/destroy pairs
   - **Action:** Simplify using `al_draw_scaled_rotated_bitmap()`

2. **Sprite.cpp**
   - Risk: Core rendering class, affects all game entities
   - Rotation logic with fixed-point math
   - **Action:** Convert to native Allegro 5 rotation, test thoroughly

3. **PlanetTileScroller.cpp**
   - Risk: Complex tile generation and caching
   - Performance critical (called frequently)
   - **Action:** Profile before/after, may need optimization

4. **MiniWindow.cpp**
   - Risk: Complex tiling algorithm for window borders
   - 20+ bitmap dimension accesses in tight loops
   - **Action:** Cache dimensions, test edge cases

5. **ModulePlanetSurface.cpp**
   - Risk: Largest file, most bitmap operations
   - 80+ load_bitmap calls
   - 35+ dimension accesses
   - **Action:** Split into phases, extensive testing

### 5.2 Medium-Risk Files

Standard patterns, lower complexity:

- ModuleEngineer.cpp - Gauge bars (straightforward)
- ModuleMedical.cpp - Skill bars (straightforward)
- ModuleCaptainCreation.cpp - Button grid (tedious but simple)
- ModuleTopGUI.cpp - Status gauges (straightforward)
- ScrollBox.cpp - State buffers (moderate complexity)

### 5.3 Low-Risk Files

Simple load/draw patterns:

- ModuleCredits.cpp
- ModuleTitleScreen.cpp
- ModuleStartup.cpp
- ModuleCantina.cpp
- Button.cpp
- Label.cpp

---

## Part 6: Recommended Migration Approach

### Phase 1: Preparation (Week 1)

1. **Add dimension helper macros** (temporary)
   ```cpp
   // In allegro5_compat.h
   #define BMP_WIDTH(bmp) al_get_bitmap_width(bmp)
   #define BMP_HEIGHT(bmp) al_get_bitmap_height(bmp)
   ```

2. **Create test suite**
   - Screenshot reference images
   - Automated visual diff
   - Performance benchmarks

3. **Document sub-bitmap usage**
   - Only 1 occurrence, but verify destroy order

### Phase 2: Low-Hanging Fruit (Week 2)

Migrate low-risk files first:

1. **Label.cpp** - Simple bitmap with magenta transparency
2. **Button.cpp** - Generic button class
3. **ModuleCredits.cpp** - Simple background blit
4. **ModuleStartup.cpp** - Fade effects

**Deliverable:** 4 files fully migrated, verify rendering

### Phase 3: Moderate Complexity (Week 3-4)

1. **ScrollBox.cpp** - State buffer pattern
2. **ModuleTopGUI.cpp** - Gauge bars
3. **ModuleEngineer.cpp** - More gauge bars
4. **ModuleMedical.cpp** - Skill bars

**Deliverable:** All gauge/bar rendering migrated

### Phase 4: High Complexity (Week 5-6)

1. **Sprite.cpp** - Core rotation/scaling
   - Simplify using native Allegro 5 functions
   - Eliminate temp buffers where possible

2. **PlanetSurfaceObject.cpp** - Complex rotation pipeline
   - Test various scale/rotation combinations
   - Verify transparency handling

3. **PlanetTileScroller.cpp** - Tile generation
   - Profile performance
   - May need caching optimization

### Phase 5: Large Modules (Week 7-9)

1. **ModuleCaptainCreation.cpp** - Button grid
   - 45+ dimension accesses
   - Tedious but straightforward

2. **ModulePlanetSurface.cpp** - Main game screen
   - Break into subsections
   - Test each subsection thoroughly

3. **MiniWindow.cpp** - Window tiling
   - Cache dimensions in loops
   - Test various window sizes

### Phase 6: Cleanup (Week 10)

1. **Remove temporary macros**
   - Convert all BMP_WIDTH/BMP_HEIGHT to al_get_bitmap_*
   
2. **Remove magenta transparency**
   - Convert all `makecol(255,0,255)` to proper alpha

3. **Performance optimization**
   - Identify bottlenecks
   - Add dimension caching where beneficial

4. **Final testing**
   - Full playthrough
   - Visual regression testing
   - Memory leak detection

---

## Part 7: Code Generation Helpers

### 7.1 Dimension Access Conversion Script

**sed/awk pattern to assist (NOT FULLY AUTOMATIC):**

```bash
# Find all ->w accesses for manual review
grep -rn "->w\b" src/*.cpp | grep -v "allegro5_compat.h"

# Find all ->h accesses
grep -rn "->h\b" src/*.cpp | grep -v "allegro5_compat.h"

# Find create_bitmap calls
grep -rn "create_bitmap" src/*.cpp
```

### 7.2 Common Replacement Patterns

**Pattern 1: Simple dimension read**
```
Search:  (\w+)->w
Replace: al_get_bitmap_width(\1)

Search:  (\w+)->h
Replace: al_get_bitmap_height(\1)
```

**Pattern 2: In function call**
```
Search:  blit\((.*?), (.*?), 0, 0, 0, 0, (.*?)->w, \3->h\)
Replace: al_draw_bitmap(\1, 0, 0, 0)
```

**Pattern 3: Cached dimensions**
```cpp
// Manual pattern - add at function start
int w = al_get_bitmap_width(bitmap);
int h = al_get_bitmap_height(bitmap);
// Then use w and h throughout function
```

### 7.3 Testing Checklist

Per file migrated:

- [ ] Compiles without errors
- [ ] Compiles without warnings
- [ ] Module loads without crash
- [ ] Visual rendering matches reference
- [ ] No performance regression
- [ ] No memory leaks (valgrind)
- [ ] Transparency renders correctly
- [ ] Scaling/rotation works correctly (if applicable)

---

## Part 8: Quick Reference

### Allegro 4 → Allegro 5 Bitmap API Map

| Allegro 4 | Allegro 5 | Notes |
|-----------|-----------|-------|
| `create_bitmap(w,h)` | `al_create_bitmap(w,h)` | [OK] Macro exists |
| `load_bitmap(file,pal)` | `al_load_bitmap(file)` | [OK] Macro exists, ignore palette |
| `destroy_bitmap(bmp)` | `al_destroy_bitmap(bmp)` | [OK] Macro exists |
| `create_sub_bitmap(p,x,y,w,h)` | `al_create_sub_bitmap(p,x,y,w,h)` | 1 occurrence |
| `clear_bitmap(bmp)` | `al_clear_to_color(al_map_rgba(0,0,0,0))` | [OK] Macro exists |
| `clear_to_color(bmp,c)` | `al_clear_to_color(c)` | [OK] Macro exists |
| `bmp->w` | `al_get_bitmap_width(bmp)` | [ERROR] 197 occurrences |
| `bmp->h` | `al_get_bitmap_height(bmp)` | [ERROR] 185 occurrences |

### Common Magenta Patterns to Replace

```cpp
// OLD: Magenta transparency
clear_to_color(temp, makecol(255, 0, 255));

// NEW: Proper alpha
al_set_target_bitmap(temp);
al_clear_to_color(al_map_rgba(0, 0, 0, 0));

// OLD: Masked blit with magenta
masked_blit(src, dest, sx, sy, dx, dy, w, h);

// NEW: Alpha blend (after loading, convert mask)
al_convert_mask_to_alpha(src, al_map_rgb(255, 0, 255));
al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0);
```

### Performance Considerations

**Cache dimensions in loops:**
```cpp
// BAD - calls function 1000 times
for (int i = 0; i < al_get_bitmap_width(bmp); i++) { ... }

// GOOD - calls function once
int w = al_get_bitmap_width(bmp);
for (int i = 0; i < w; i++) { ... }
```

**Use appropriate bitmap flags:**
```cpp
// For frequent CPU access (pixel manipulation):
al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
ALLEGRO_BITMAP *cpu_bmp = al_create_bitmap(w, h);

// For display/drawing (GPU-accelerated):
al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);  // Default
ALLEGRO_BITMAP *gpu_bmp = al_create_bitmap(w, h);
```

---

## Part 9: Special Cases and Gotchas

### 9.1 Sub-Bitmap Lifetime

**CRITICAL:** Sub-bitmap MUST be destroyed before parent

```cpp
// Allegro 4 & 5 - correct order
ALLEGRO_BITMAP *parent = al_create_bitmap(800, 600);
ALLEGRO_BITMAP *sub = al_create_sub_bitmap(parent, 100, 100, 200, 200);

// Use sub...

al_destroy_bitmap(sub);     // MUST destroy sub first
al_destroy_bitmap(parent);  // Then parent

// WRONG - will crash or corrupt memory
al_destroy_bitmap(parent);  // [ERROR]
al_destroy_bitmap(sub);     // [ERROR] Too late, parent gone
```

### 9.2 Memory vs Video Bitmaps

**Allegro 4:** 
- `create_bitmap()` → memory bitmap
- `create_video_bitmap()` → video bitmap (rarely used)

**Allegro 5:**
- Default: Video bitmap (GPU memory)
- Much faster for drawing
- Slower for pixel access

**Migration Pattern:**
```cpp
// If code does frequent getpixel/putpixel:
al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
ALLEGRO_BITMAP *bmp = al_create_bitmap(w, h);

// For drawing/blitting (most cases):
al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);  // Usually default
ALLEGRO_BITMAP *bmp = al_create_bitmap(w, h);
```

### 9.3 Target Bitmap Concept

**Allegro 4:** Drawing functions take destination parameter
```cpp
blit(src, dest, sx, sy, dx, dy, w, h);
rectfill(dest, x1, y1, x2, y2, color);
```

**Allegro 5:** Must set target bitmap first
```cpp
al_set_target_bitmap(dest);
al_draw_bitmap_region(src, sx, sy, w, h, dx, dy, 0);
al_draw_filled_rectangle(x1, y1, x2, y2, color);
```

**Pattern:** Save/restore target for temporary drawing
```cpp
ALLEGRO_BITMAP *old_target = al_get_target_bitmap();
al_set_target_bitmap(temp);
// ... draw to temp ...
al_set_target_bitmap(old_target);  // Restore
```

The `allegro5_compat.h` macros handle this automatically, but native code should do it explicitly.

### 9.4 Transparency Migration

**Allegro 4 Magenta Mask:**
```cpp
// Load sprite with magenta background
BITMAP *spr = load_bitmap("sprite.bmp", NULL);
// Magenta (255,0,255) automatically treated as transparent by masked_blit()
masked_blit(spr, dest, 0, 0, x, y, w, h);
```

**Allegro 5 Alpha Channel:**
```cpp
// Load sprite
ALLEGRO_BITMAP *spr = al_load_bitmap("sprite.bmp");

// Convert magenta to alpha (one-time operation after loading)
al_convert_mask_to_alpha(spr, al_map_rgb(255, 0, 255));

// Now just draw normally, alpha automatically handled
al_draw_bitmap(spr, x, y, 0);
```

**For new art assets:** Use PNG with proper alpha channel instead of BMP with magenta.

---

## Part 10: Summary Statistics

### Total Work Estimate

| Task | Occurrences | Estimated Hours |
|------|-------------|-----------------|
| Manual `->w` conversions | 197 | 20 hours |
| Manual `->h` conversions | 185 | 18 hours |
| `create_bitmap` verification | 47 | 5 hours |
| `load_bitmap` verification | 203 | 10 hours (verify transparency) |
| `destroy_bitmap` verification | 79 | 3 hours |
| `clear_to_color` transparency fixes | 21 | 4 hours |
| Complex rotation pipeline (Sprite.cpp) | 3 functions | 8 hours |
| Tile caching (PlanetTileScroller.cpp) | 1 system | 6 hours |
| Testing per file | 50 files | 50 hours |
| **TOTAL** | | **~124 hours** |

### Files Requiring Attention (Sorted by Effort)

| Rank | File | Estimated Hours | Reason |
|------|------|-----------------|--------|
| 1 | ModulePlanetSurface.cpp | 15 | Largest, most complex, 80+ loads, 35+ dims |
| 2 | ModuleCaptainCreation.cpp | 10 | 45+ dimension accesses in button grid |
| 3 | PlanetSurfaceObject.cpp | 8 | Complex rotation/scaling pipeline |
| 4 | Sprite.cpp | 8 | Core rendering, needs simplification |
| 5 | PlanetTileScroller.cpp | 6 | Tile generation performance critical |
| 6 | MiniWindow.cpp | 5 | Window tiling algorithm |
| 7 | ModuleEngineer.cpp | 4 | Gauge bars |
| 8 | ModuleMedical.cpp | 4 | Skill bars |
| 9 | ScrollBox.cpp | 4 | State buffers |
| 10 | ModuleEncounter.cpp | 4 | Combat screen |
| 11-50 | Remaining files | 56 | Various simple patterns |

### Success Criteria

[OK] Migration Complete When:
- All `->w` and `->h` accesses converted to function calls
- All magenta transparency converted to proper alpha
- All temporary macros removed
- All modules tested and working
- No visual regressions
- No performance regressions
- No memory leaks

---

## Appendix A: File-by-File Bitmap Operation Inventory

### Complete Listing

| File | create | load | destroy | sub | clear | clear_to | ->w | ->h |
|------|--------|------|---------|-----|-------|----------|-----|-----|
| ModulePlanetSurface.cpp | 1 | 80+ | 1 | 0 | 1 | 1 | 35+ | 35+ |
| ModuleCaptainCreation.cpp | 0 | 16 | 0 | 0 | 0 | 0 | 45+ | 45+ |
| PlanetSurfaceObject.cpp | 7 | 1 | 7 | 0 | 1 | 0 | 3 | 3 |
| Sprite.cpp | 3 | 1 | 4 | 0 | 0 | 1 | 5 | 5 |
| MiniWindow.cpp | 1 | 3 | 1 | 0 | 0 | 1 | 20+ | 12 |
| ModuleEngineer.cpp | 1 | 0 | 1 | 0 | 0 | 2 | 14 | 14 |
| ModuleMedical.cpp | 0 | 16 | 0 | 0 | 0 | 0 | 12 | 12 |
| ScrollBox.cpp | 7 | 0 | 5 | 0 | 3 | 1 | 8 | 8 |
| ModuleEncounter.cpp | 1 | 20+ | 0 | 0 | 0 | 2 | 10 | 10 |
| ModuleSolarSystem.cpp | 3 | 1 | 1 | 0 | 0 | 2 | 3 | 3 |
| ModuleStarmap.cpp | 3 | 0 | 4 | 0 | 0 | 5 | 5 | 5 |
| PlanetTileScroller.cpp | 4 | 1 | 4 | 0 | 0 | 1 | 3 | 3 |
| ModuleTopGUI.cpp | 0 | 5 | 0 | 0 | 0 | 0 | 5 | 5 |
| ModuleCrewHire.cpp | 5 | 2 | 4 | 0 | 0 | 0 | 8 | 8 |
| Game.cpp | 1 | 0 | 4 | 0 | 0 | 0 | 4 | 4 |
| (Others) | 10 | 58 | 42 | 1 | 2 | 5 | 57 | 55 |
| **TOTALS** | **47** | **203** | **79** | **1** | **7** | **21** | **197** | **185** |

---

## Appendix B: Related Migration Documents

- **allegro_transitions.md** - Overall Allegro 4 → 5 migration strategy
- **src/allegro5_compat.h** - Compatibility layer implementation
- Future documents (to be created):
  - `color_system_migration.md` - makecol() and color handling
  - `drawing_primitives_migration.md` - lines, rectangles, circles
  - `input_system_migration.md` - keyboard and mouse
  - `blitting_migration.md` - blit, masked_blit, stretch_blit

---

## Document Metadata

**Author:** AI Code Analysis  
**Date:** 2026-02-02  
**Version:** 1.0  
**Status:** Complete Analysis  
**Next Steps:** Begin Phase 1 migration (preparation)

**Questions/Issues:** Contact development team

---

*End of Bitmap Management Migration Documentation*
