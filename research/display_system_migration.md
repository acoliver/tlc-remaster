# Display System Migration Documentation - Allegro 4 to Allegro 5

## Executive Summary

This document analyzes the display initialization, backbuffer management, and frame presentation system in TLC (The Lost Colony) for migration from Allegro 4 (via Allegro Legacy) to native Allegro 5 APIs.

**Current System:**
- Allegro 4 Legacy with `set_gfx_mode()` and global `screen` variable
- Fixed internal resolution (1024x768) with runtime scaling to user-selected display resolution
- Double buffering using manually created `m_backbuffer`
- Single frame presentation via `stretch_blit()` from backbuffer to screen

**Target System:**
- Allegro 5 native with `al_create_display()` and display pointer management
- Same fixed internal resolution with scaling
- Built-in double buffering via `al_get_backbuffer(display)`
- Frame presentation via `al_flip_display()`

---

## Current Display Architecture Analysis

### 1. Constants and Fixed Resolution (Game.h L29-30)

```cpp
//DO NOT MODIFY THESE
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
```

**Purpose:** Fixed internal rendering resolution. All game content is rendered to this resolution, then scaled to match the user's actual display resolution.

**Usage:** 87 references throughout codebase for layout calculations, viewport definitions, and coordinate systems.

**Migration Impact:** **LOW** - These constants remain unchanged. The internal render resolution stays fixed at 1024x768.

---

### 2. Display Initialization (Game.cpp L708-883)

#### Function: `Game::Initialize_Graphics()`

**Called from:**
- `InitGame()` (L909) - Initial startup
- Settings screen - When user changes resolution/fullscreen mode

**Key Responsibilities:**
1. Detect desktop resolution and color depth
2. Parse user-selected resolution from `RESOLUTION` global script variable
3. Set graphics mode (DirectX on Windows, autodetect elsewhere)
4. Create backbuffer at fixed 1024x768 resolution
5. Enumerate available video modes for Settings UI

#### Current Implementation Flow:

```cpp
// Lines 715-723: Desktop detection (runs once)
get_desktop_resolution(&desktop_width, &desktop_height);
desktop_colordepth = desktop_color_depth();
set_color_depth(desktop_colordepth);
set_alpha_blender();

// Lines 729-750: Parse user resolution from settings
string resolution = g_game->getGlobalString("RESOLUTION");
// Parse to actual_width, actual_height
// Default to desktop resolution if not set

// Lines 752-767: Determine graphics mode
bool fullscreen = g_game->getGlobalBoolean("FULLSCREEN");
#ifdef TLC_PLATFORM_WINDOWS
    if (fullscreen) {
        gfxmode = GFX_DIRECTX_ACCEL;
    } else {
        gfxmode = GFX_DIRECTX_WIN;
    }
#else
    gfxmode = GFX_AUTODETECT;
#endif

// Lines 782-806: Set graphics mode with fallback
set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);  // Reset to text mode first
if (set_gfx_mode(gfxmode, actual_width, actual_height, 0, 0) != 0) {
    // Try fallback mode
    // If that fails, try default 1024x768
    // If that fails, fatal error
}

// Lines 819-830: Create backbuffer at fixed resolution
if (m_backbuffer) {
    destroy_bitmap(m_backbuffer);
}
m_backbuffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);

// Lines 839-880: Enumerate video modes for Settings UI (Windows only)
GFX_MODE_LIST *list = get_gfx_mode_list(GFX_DIRECTX_ACCEL);
// Populate videomodes vector with modes >= 1024x768
```

---

### 3. The "screen" Global Variable

**Type:** `BITMAP*` - Allegro 4's global display surface
**Defined by:** Allegro 4 library (`<allegro.h>`)
**Scope:** Global, automatically created by `set_gfx_mode()`

#### Usage Analysis (45 references across codebase):

**Primary Usage - Final Frame Presentation (Game.cpp L1285):**
```cpp
stretch_blit(m_backbuffer, screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 
             cx, 0, scale_width, scale_height);
```
This is THE critical line - the only place where rendering is presented to the user.

**Secondary Usages - Screen Dimension Queries:**

1. **Scaling calculation (Game.cpp L1234):**
   ```cpp
   screen_scaling = (double)screen->h / (double)SCREEN_HEIGHT;
   scale_height = (int)((double)m_backbuffer->h * screen_scaling);
   scale_width = (int)((double)m_backbuffer->w * screen_scaling);
   ```

2. **Direct dimension access (various files):**
   ```cpp
   // ModuleStarport.cpp L424, 543, 546, 549
   screen->w  // Actual display width
   screen->h  // Actual display height
   
   // ModuleCaptainsLounge.cpp L409
   blit(m_background, g_game->GetBackBuffer(), 0, 0, 0, 0, screen->w, screen->h);
   
   // ModuleTradeDepot.cpp L590
   blit(m_background, canvas, 0, 0, 0, 0, screen->w, screen->h);
   
   // ModuleCaptainCreation.cpp L378
   blit(m_professionChoiceBackground, g_game->GetBackBuffer(), 0, 0, 0, 0, 
        screen->w, screen->h);
   ```

3. **Font rendering (alfont_compat.cpp L29):**
   ```cpp
   const int bpp = bitmap_color_depth(screen);
   ```

**Key Insight:** Most code uses `SCREEN_WIDTH` and `SCREEN_HEIGHT` constants, not `screen->w/h`. The `screen` global is primarily used for:
- Final frame presentation via `stretch_blit()`
- Calculating runtime scaling factor
- A few legacy dimension queries (can be replaced with `actual_width/actual_height`)

---

### 4. Double Buffering System

#### Current Architecture:

```
Rendering Flow:
┌─────────────────────────────────────────┐
│ Game Loop (Game::RunGame L1105-1289)    │
├─────────────────────────────────────────┤
│ 1. Update input/logic                   │
│ 2. modeMgr->Draw() → draws to          │
│    m_backbuffer (1024x768)              │
│ 3. Draw UI overlays to m_backbuffer     │
│ 4. Draw cursor to m_backbuffer          │
│ 5. stretch_blit(m_backbuffer, screen)  │ ← Frame presentation
│    - Scales from 1024x768 to actual    │
│      display resolution                 │
│    - Centers output if aspect mismatch  │
└─────────────────────────────────────────┘
```

#### Backbuffer Member (Game.h L239, Game.cpp L826):
```cpp
// In Game.h
private:
    BITMAP *m_backbuffer;  // Primary drawing surface for all modules

// In Game.cpp Initialize_Graphics()
m_backbuffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);

// Accessor used by all modules
BITMAP *GetBackBuffer() { return m_backbuffer; }
```

**Usage Pattern:** Every module draws to `g_game->GetBackBuffer()`, never directly to `screen`.

---

### 5. Frame Presentation Mechanism (Game.cpp L1278-1285)

#### The Critical Rendering Code:

```cpp
// L1278-1279: Screen shake effect
if (vibration) v = Util::Random(0, vibration); else v = 0;

// L1230-1236: Prepare scaling values
screen_scaling = (double)screen->h / (double)SCREEN_HEIGHT;
scale_height = (int)((double)m_backbuffer->h * screen_scaling);
scale_width = (int)((double)m_backbuffer->w * screen_scaling);

// L1284-1285: Center and scale to display
int cx = (actual_width - scale_width) / 2;  // Horizontal centering
stretch_blit(m_backbuffer, screen, 
             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,  // Source rect (full backbuffer)
             cx, 0, scale_width, scale_height);   // Dest rect (scaled, centered)
```

#### Scaling Strategy:

1. **Aspect Ratio Preservation:**
   - Source: 1024x768 (4:3 ratio)
   - Destination: User-selected resolution (various ratios)
   - Strategy: Scale to fill height, letterbox horizontally if needed

2. **Centering Calculation:**
   ```cpp
   cx = (actual_width - scale_width) / 2
   ```
   If display is wider than 4:3, black bars appear on sides.

3. **Examples:**
   - 1024x768 display: No scaling, cx=0, 1:1 pixel mapping
   - 1920x1080 display: Scales to 1440x1080 (1.4x), cx=240, black bars on sides
   - 2560x1440 display: Scales to 1920x1440 (1.875x), cx=320, black bars on sides

---

### 6. Resolution Scaling Variables (Game.h L246)

```cpp
double screen_scaling;  // Ratio of display height to internal height
double scale_width;     // Scaled backbuffer width in pixels
double scale_height;    // Scaled backbuffer height in pixels
```

**Calculated in:** `RunGame()` L1234-1236 (every frame)
**Used for:**
- Debug overlay text (L1250)
- Mouse coordinate unscaling (L1252-1254)
- Frame presentation (L1285)

---

### 7. Mouse Coordinate Scaling

#### The Mouse Coordinate Problem:

Allegro reports mouse coordinates in **actual display space** (e.g., 1920x1080), but game logic expects **internal resolution space** (1024x768). Requires conversion:

```cpp
// Game.cpp L1252-1254 (debug output)
int scalemx = (int)((double)mouse_x / screen_scaling);
int scalemy = (int)((double)mouse_y / screen_scaling);
```

**Current System:** Mouse events are dispatched with display-space coordinates. Each module must convert if needed.

**Potential Issue:** Not all mouse handling code may be correctly unscaling coordinates. This could be a source of bugs when running at non-native resolutions.

---

### 8. Frame Rate and Timing

#### Frame Rate Target (Game.cpp L1110, L1115-1127):

```cpp
float fps_delay = 1000.0f / 60.0f;  // Target 60 FPS (16.67ms per frame)

// Frame limiter
if (globalTimer.getTimer() < timeStart + (int)fps_delay) {
    if (g_game->getGlobalBoolean("UNLIMITED_FRAMERATE") == false) {
        return;  // Skip frame, wait for next tick
    }
}
```

**Timing Strategy:**
- Target: 60 FPS
- Method: Busy-wait loop with early return if frame budget not met
- Override: `UNLIMITED_FRAMERATE` setting for benchmarking

**Note:** Timing is NOT tied to vsync or display refresh rate. This could cause tearing.

---

## Allegro 5 Migration Plan

### Phase 1: Display Creation (Game.cpp L782-806)

#### Replace `set_gfx_mode()` with `al_create_display()`

**Current Code:**
```cpp
set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);  // Reset
if (set_gfx_mode(gfxmode, actual_width, actual_height, 0, 0) != 0) {
    // Error handling
}
```

**Allegro 5 Equivalent:**
```cpp
// Store display pointer as Game member
ALLEGRO_DISPLAY *m_display;  // Add to Game.h private members

// In Initialize_Graphics():
// Destroy old display if reinitializing
if (m_display) {
    al_destroy_display(m_display);
    m_display = NULL;
}

// Configure display flags before creation
int flags = ALLEGRO_WINDOWED;  // Default to windowed
if (fullscreen) {
    flags = ALLEGRO_FULLSCREEN_WINDOW;  // Borderless fullscreen
}
al_set_new_display_flags(flags);

// Create display at user-selected resolution
m_display = al_create_display(actual_width, actual_height);
if (!m_display) {
    debug << "Failed to create display at " << actual_width << "x" << actual_height << endl;
    
    // Fallback to default resolution
    actual_width = SCREEN_WIDTH;
    actual_height = SCREEN_HEIGHT;
    m_display = al_create_display(actual_width, actual_height);
    
    if (!m_display) {
        debug << "Fatal Error: Unable to create display" << endl;
        return false;
    }
}

debug << "Display created: " << al_get_display_width(m_display) 
      << "x" << al_get_display_height(m_display) << endl;
```

**Key Differences:**
- No separate "reset to text mode" step needed
- Display flags set before creation (windowed vs fullscreen)
- Returns pointer instead of error code
- Must store pointer for later use

---

### Phase 2: Backbuffer Management (Game.cpp L819-830)

#### Option A: Keep Manual Backbuffer (Minimal Change)

**Current:**
```cpp
m_backbuffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
```

**Allegro 5:**
```cpp
// Set bitmap creation flags
al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);  // GPU-accelerated

m_backbuffer = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
if (!m_backbuffer) {
    debug << "Error creating back buffer" << endl;
    return false;
}
```

**Pros:**
- Minimal code changes
- All existing `GetBackBuffer()` calls work unchanged
- Same double-buffering flow

**Cons:**
- Extra bitmap allocation
- Two blits per frame (modules→m_backbuffer, m_backbuffer→display)

---

#### Option B: Use Display Backbuffer (Recommended)

**Replace manual backbuffer with Allegro 5's built-in backbuffer:**

```cpp
// Remove m_backbuffer member entirely
// Add to Game.h:
ALLEGRO_DISPLAY *m_display;

// Update GetBackBuffer() accessor (Game.h L113):
ALLEGRO_BITMAP *GetBackBuffer() { 
    return al_get_backbuffer(m_display); 
}
```

**Pros:**
- No extra bitmap allocation
- One less blit per frame
- Native Allegro 5 pattern
- Automatic integration with vsync/flip

**Cons:**
- Can't draw to backbuffer while display is being flipped (shouldn't matter with current frame pacing)
- Backbuffer contents undefined after flip (shouldn't matter - full redraw each frame)

**Recommendation:** Use Option B. The codebase already does full-screen redraws every frame, so preserving backbuffer contents is unnecessary.

---

### Phase 3: Frame Presentation (Game.cpp L1285)

#### Replace `stretch_blit()` with `al_draw_scaled_bitmap()` + `al_flip_display()`

**Current:**
```cpp
int cx = (actual_width - scale_width) / 2;
stretch_blit(m_backbuffer, screen, 
             0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
             cx, 0, scale_width, scale_height);
```

**Allegro 5 (Option A - Manual Backbuffer):**
```cpp
// Set draw target to display backbuffer
al_set_target_backbuffer(m_display);

// Clear to black (for letterbox bars)
al_clear_to_color(al_map_rgb(0, 0, 0));

// Draw scaled game backbuffer to display backbuffer
int cx = (actual_width - scale_width) / 2;
al_draw_scaled_bitmap(m_backbuffer,
                      0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,  // Source rect
                      cx, 0, scale_width, scale_height,    // Dest rect
                      0);                                  // Flags

// Present to screen
al_flip_display();
```

**Allegro 5 (Option B - Direct to Backbuffer, Recommended):**

If using display backbuffer directly, no intermediate blit needed. All module drawing goes directly to display backbuffer, scaled at presentation:

```cpp
// NO INTERMEDIATE BLIT!
// Modules already drew to al_get_backbuffer(m_display) at 1024x768

// Wait - this won't work! The scaling happens during the final blit.
// We need a different approach...
```

**Critical Realization:** The scaling happens during the `stretch_blit()`. If we eliminate the manual backbuffer, we need a different scaling strategy.

---

#### Scaling Strategy Options for Option B:

**Strategy 1: Use Allegro 5's Display Scaling (Recommended)**

Allegro 5 has built-in display scaling via transformations:

```cpp
// In Initialize_Graphics(), after creating display:
// Set up a transform that scales 1024x768 to display size
ALLEGRO_TRANSFORM transform;
al_identity_transform(&transform);

// Calculate scaling to preserve aspect ratio
float scale_x = (float)actual_width / SCREEN_WIDTH;
float scale_y = (float)actual_height / SCREEN_HEIGHT;
float scale = (scale_x < scale_y) ? scale_x : scale_y;  // Fit to smallest dimension

// Calculate centering offset
float offset_x = (actual_width - SCREEN_WIDTH * scale) / 2.0f;
float offset_y = (actual_height - SCREEN_HEIGHT * scale) / 2.0f;

// Apply scale and translation
al_translate_transform(&transform, offset_x, offset_y);
al_scale_transform(&transform, scale, scale);
al_use_transform(&transform);

// Now all drawing at 1024x768 coordinates is automatically scaled!
```

**In RunGame():**
```cpp
// Clear display (for letterbox bars)
al_set_target_backbuffer(m_display);
al_clear_to_color(al_map_rgb(0, 0, 0));

// All module drawing happens here at 1024x768 coordinates
modeMgr->Draw();
// ... UI, cursor, etc ...

// Present to screen with automatic scaling
al_flip_display();
```

**Pros:**
- No intermediate backbuffer needed
- Single flip per frame
- Automatic scaling via GPU
- Can enable vsync easily: `al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST)`

**Cons:**
- All drawing code must respect transform (should be automatic)
- Mouse coordinates need untransform: `al_transform_coordinates()`

---

**Strategy 2: Keep Manual Backbuffer + Scaling Blit**

Stick with Option A approach. Simpler migration, proven to work.

---

### Recommended Migration Path: Hybrid Approach

**Best of both worlds:**

1. **Keep manual backbuffer initially** for safe migration
2. **Update display creation** to Allegro 5
3. **Update frame presentation** to use `al_draw_scaled_bitmap()` + `al_flip_display()`
4. **Later optimization:** Switch to display backbuffer + transform if needed

**Rationale:** Minimize risk by changing one thing at a time. The extra blit is negligible performance-wise (single quad on GPU).

---

### Phase 4: Replace "screen" Global References

All 45 references to the `screen` global must be updated:

#### Category 1: Frame Presentation (1 reference)
**File:** Game.cpp L1285
**Action:** Replace with `al_draw_scaled_bitmap()` as shown above

#### Category 2: Scaling Calculations (1 reference)
**File:** Game.cpp L1234
**Current:**
```cpp
screen_scaling = (double)screen->h / (double)SCREEN_HEIGHT;
```
**Allegro 5:**
```cpp
screen_scaling = (double)al_get_display_height(m_display) / (double)SCREEN_HEIGHT;
```

#### Category 3: Dimension Queries (10 references)
**Files:** ModuleStarport.cpp, ModuleCaptainsLounge.cpp, ModuleTradeDepot.cpp, ModuleCaptainCreation.cpp

**Current pattern:**
```cpp
screen->w  // Actual display width
screen->h  // Actual display height
```

**Migration options:**

**Option A: Use display dimensions:**
```cpp
al_get_display_width(g_game->GetDisplay())
al_get_display_height(g_game->GetDisplay())
```

**Option B: Use stored members:**
```cpp
g_game->actual_width   // Already exists!
g_game->actual_height  // Already exists!
```

**Recommendation:** Use Option B. The values are already tracked in `Game::actual_width/actual_height`.

**Example fix (ModuleStarport.cpp L543):**
```cpp
// Before
blit(starport, g_game->GetBackBuffer(), 
     g_game->gameState->player->posStarport.x, 0, 0, 0, screen->w, 348);

// After
blit(starport, g_game->GetBackBuffer(), 
     g_game->gameState->player->posStarport.x, 0, 0, 0, 
     g_game->actual_width, 348);

// Better yet (these are drawing to backbuffer, which is always 1024x768):
blit(starport, g_game->GetBackBuffer(), 
     g_game->gameState->player->posStarport.x, 0, 0, 0, 
     SCREEN_WIDTH, 348);
```

**Actually... WAIT!** Looking at this code more carefully:

```cpp
blit(starport, g_game->GetBackBuffer(), ..., screen->w, 348);
```

This is blitting TO `GetBackBuffer()`, which is the 1024x768 internal buffer. Why would it use `screen->w`? 

**Analysis:** This looks like a bug or legacy code. The backbuffer is ALWAYS 1024x768, so dimension should be `SCREEN_WIDTH` (1024), not `screen->w` (which could be 1920 or whatever).

**Correct fix:**
```cpp
blit(starport, g_game->GetBackBuffer(), 
     g_game->gameState->player->posStarport.x, 0, 0, 0, 
     SCREEN_WIDTH, 348);
```

**Verify all 10 references** for this pattern and fix accordingly.

---

#### Category 4: Color Depth Query (1 reference)
**File:** alfont_compat.cpp L29
**Current:**
```cpp
const int bpp = bitmap_color_depth(screen);
```

**Allegro 5:**
```cpp
// Allegro 5 always uses 32-bit color internally
const int bpp = 32;

// Or query display format:
ALLEGRO_PIXEL_FORMAT format = al_get_display_format(display);
// But format is an enum, not a BPP value
// For compatibility, just use 32
```

**Recommendation:** Hardcode to 32. Allegro 5 standardizes on 32-bit RGBA.

---

### Phase 5: Display Accessor (Game.h)

Add public accessor for display pointer:

```cpp
// In Game.h public section:
ALLEGRO_DISPLAY *GetDisplay() { return m_display; }

// Update GetBackBuffer():
ALLEGRO_BITMAP *GetBackBuffer() { 
    if (m_use_manual_backbuffer) {
        return m_backbuffer;  // Manual backbuffer option
    } else {
        return al_get_backbuffer(m_display);  // Display backbuffer option
    }
}
```

---

### Phase 6: Fullscreen Toggle

**Current:** Requires full graphics reset via `Initialize_Graphics()`
**Allegro 5:** Can toggle at runtime

```cpp
// In settings or pause menu:
bool SetFullscreen(bool enable) {
    if (al_set_display_flag(m_display, ALLEGRO_FULLSCREEN_WINDOW, enable)) {
        debug << "Fullscreen mode: " << (enable ? "ON" : "OFF") << endl;
        return true;
    } else {
        debug << "Failed to toggle fullscreen mode" << endl;
        return false;
    }
}
```

**Note:** May still need to adjust transforms/scaling after mode change.

---

### Phase 7: Video Mode Enumeration (Game.cpp L839-880)

**Current:** Uses DirectX-specific `get_gfx_mode_list()`
**Allegro 5:** Use `al_get_num_display_modes()` and `al_get_display_mode()`

```cpp
// In Initialize_Graphics():
if (videomodes.size() == 0) {
    int num_modes = al_get_num_display_modes();
    debug << "Detected " << num_modes << " display modes" << endl;
    
    for (int i = 0; i < num_modes; i++) {
        ALLEGRO_DISPLAY_MODE mode;
        if (al_get_display_mode(i, &mode)) {
            // Filter for minimum resolution
            if (mode.width >= 1024 && mode.height >= 768) {
                VideoMode vm;
                vm.width = mode.width;
                vm.height = mode.height;
                vm.bpp = 32;  // A5 standardizes on 32-bit
                videomodes.push_back(vm);
                
                debug << mode.width << "x" << mode.height 
                      << " @ " << mode.refresh_rate << "Hz" << endl;
            }
        }
    }
}
```

---

## Mouse Coordinate Transformation

### Current System Issues

The current system has **potential scaling bugs**:

```cpp
// Game.cpp L1252-1254
int scalemx = (int)((double)mouse_x / screen_scaling);
int scalemy = (int)((double)mouse_y / screen_scaling);
```

This calculation assumes `mouse_x/y` are in display space, but:
1. Not all mouse event handlers use these scaled values
2. The centering offset (`cx`) is not accounted for
3. Letterboxing breaks coordinate mapping

**Correct formula:**
```cpp
int scalemx = (int)((double)(mouse_x - cx) / screen_scaling);
int scalemy = (int)((double)mouse_y / screen_scaling);
```

---

### Allegro 5 Solution: Automatic Transforms

If using display backbuffer + transform approach:

```cpp
// In mouse event handler:
ALLEGRO_TRANSFORM transform;
al_copy_transform(&transform, al_get_current_transform());
al_invert_transform(&transform);

float game_x = mouse_x;
float game_y = mouse_y;
al_transform_coordinates(&transform, &game_x, &game_y);

// Now game_x/game_y are in 1024x768 space
OnMouseMove((int)game_x, (int)game_y);
```

This automatically handles scaling AND centering!

---

## Vsync and Frame Timing

### Current System: Software Limiter

```cpp
// Game.cpp L1115-1127
if (globalTimer.getTimer() < timeStart + (int)fps_delay) {
    if (g_game->getGlobalBoolean("UNLIMITED_FRAMERATE") == false) {
        return;  // Skip frame
    }
}
```

**Issues:**
- No vsync = screen tearing
- Busy-wait wastes CPU
- Not synchronized with display refresh

---

### Allegro 5 Solution: Hardware Vsync

**Enable at display creation:**
```cpp
al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
m_display = al_create_display(actual_width, actual_height);
```

**In game loop:**
```cpp
// Remove manual frame limiter
// Let al_flip_display() wait for vsync
al_flip_display();  // Blocks until vsync
```

**Alternative: Event-driven timing:**
```cpp
// Use Allegro 5 timer events instead of busy-wait
ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
al_register_event_source(queue, al_get_timer_event_source(timer));
al_start_timer(timer);

// In main loop:
ALLEGRO_EVENT event;
al_wait_for_event(queue, &event);
if (event.type == ALLEGRO_EVENT_TIMER) {
    RunGame();  // Update and render
}
```

**Recommendation:** Enable vsync + event-driven timing for proper frame pacing.

---

## Implementation Checklist

### Minimal Migration (Phase 1)

- [ ] Add `ALLEGRO_DISPLAY *m_display` member to Game.h
- [ ] Replace `set_gfx_mode()` with `al_create_display()` in `Initialize_Graphics()`
- [ ] Update backbuffer creation to `al_create_bitmap()`
- [ ] Replace `stretch_blit(m_backbuffer, screen, ...)` with:
  - `al_set_target_backbuffer(m_display)`
  - `al_clear_to_color(al_map_rgb(0,0,0))`
  - `al_draw_scaled_bitmap(m_backbuffer, ...)`
  - `al_flip_display()`
- [ ] Update `screen_scaling` calculation to use `al_get_display_height(m_display)`
- [ ] Replace all `screen->w` / `screen->h` with `actual_width` / `actual_height` or `SCREEN_WIDTH` / `SCREEN_HEIGHT`
- [ ] Update `bitmap_color_depth(screen)` to `32` in alfont_compat.cpp
- [ ] Update video mode enumeration to use `al_get_num_display_modes()`
- [ ] Test at multiple resolutions (1024x768, 1920x1080, 2560x1440)
- [ ] Test fullscreen/windowed toggle

### Optimization Phase (Optional)

- [ ] Switch to display backbuffer (remove manual `m_backbuffer`)
- [ ] Implement display transform for automatic scaling
- [ ] Update mouse coordinate handling to use `al_transform_coordinates()`
- [ ] Enable vsync: `al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST)`
- [ ] Consider event-driven timing with `ALLEGRO_TIMER`
- [ ] Remove manual frame limiter if using vsync

---

## Risk Assessment

### High Risk Areas

1. **Mouse coordinate scaling** (L1252-1254)
   - Current code may not handle letterboxing correctly
   - Needs thorough testing at non-4:3 resolutions

2. **Modules using `screen->w/h`** (10 references)
   - Some may be bugs (drawing to backbuffer but using display dimensions)
   - Need case-by-case analysis

3. **Settings screen reinit** 
   - `Initialize_Graphics()` is called to change resolution
   - Must properly destroy/recreate display
   - Must preserve game state across reinit

### Medium Risk Areas

1. **Fullscreen toggle**
   - Current implementation requires full reinit
   - A5 can toggle dynamically but may need adjustment

2. **Video mode enumeration**
   - DirectX-specific code on Windows
   - Cross-platform replacement needed

3. **Frame timing**
   - Switching from software limiter to vsync changes behavior
   - Need to ensure stable 60 FPS

### Low Risk Areas

1. **Backbuffer creation**
   - Straightforward API swap
   - Same dimensions, same usage pattern

2. **Final blit operation**
   - Single point of change
   - Well-defined replacement API

---

## Testing Plan

### Resolution Testing Matrix

| Resolution | Aspect | Scaling | Expected Behavior |
|------------|--------|---------|-------------------|
| 1024x768   | 4:3    | 1.0x    | Pixel-perfect, no letterbox |
| 1280x720   | 16:9   | 0.94x   | Slight downscale, letterbox |
| 1280x1024  | 5:4    | 1.33x   | Vertical stretch or pillarbox |
| 1920x1080  | 16:9   | 1.41x   | 2x-ish scale, letterbox |
| 2560x1440  | 16:9   | 1.875x  | Large scale, letterbox |
| 3840x2160  | 16:9   | 2.8x    | 4K, heavy letterbox |

### Test Cases

1. **Display Creation**
   - [ ] Window mode at various resolutions
   - [ ] Fullscreen mode at native resolution
   - [ ] Fullscreen mode at non-native resolution
   - [ ] Toggle windowed↔fullscreen
   - [ ] Alt-Tab in fullscreen

2. **Rendering**
   - [ ] No visual glitches at all test resolutions
   - [ ] Letterbox bars are solid black
   - [ ] No tearing (with vsync)
   - [ ] Smooth 60 FPS

3. **Mouse Input**
   - [ ] Cursor tracks mouse accurately at all resolutions
   - [ ] Clicks register at correct game coordinates
   - [ ] UI buttons respond correctly
   - [ ] No coordinate drift in letterbox areas

4. **Settings Changes**
   - [ ] Change resolution → reinitialization works
   - [ ] Game state preserved across reinit
   - [ ] No crashes or memory leaks

5. **Edge Cases**
   - [ ] Minimum resolution (1024x768)
   - [ ] Ultra-wide displays (21:9)
   - [ ] Portrait orientation (rare but possible)
   - [ ] Multi-monitor setups

---

## Code Examples

### Complete Minimal Migration

**Game.h additions:**
```cpp
// In public section:
ALLEGRO_DISPLAY *GetDisplay() { return m_display; }

// In private section:
ALLEGRO_DISPLAY *m_display;
```

**Game.cpp Initialize_Graphics() replacement:**
```cpp
bool Game::Initialize_Graphics()
{
    // Desktop detection (unchanged logic)
    if (desktop_width == 0) {
        get_desktop_resolution(&desktop_width, &desktop_height);
        desktop_colordepth = desktop_color_depth();
        debug << "Desktop: " << desktop_width << "x" << desktop_height 
              << " @ " << desktop_colordepth << "bpp" << endl;
    }
    
    // Parse resolution settings (unchanged logic)
    string resolution = g_game->getGlobalString("RESOLUTION");
    if (resolution == "") {
        actual_width = desktop_width;
        actual_height = desktop_height;
    } else {
        // Parse "WIDTHxHEIGHT" format
        std::size_t div = resolution.find_first_of("xX,");
        if (div != string::npos) {
            actual_width = Util::StringToInt(resolution.substr(0, div));
            actual_height = Util::StringToInt(resolution.substr(div+1));
            if (actual_width < 1024) actual_width = 1024;
            if (actual_height < 768) actual_height = 768;
        } else {
            actual_width = desktop_width;
            actual_height = desktop_height;
        }
    }
    debug << "Target resolution: " << actual_width << "x" << actual_height << endl;
    
    // Fullscreen setting
    bool fullscreen = g_game->getGlobalBoolean("FULLSCREEN");
    
    // Destroy old display if reinitializing
    if (m_display) {
        debug << "Destroying previous display..." << endl;
        al_destroy_display(m_display);
        m_display = NULL;
    }
    
    // Configure display creation
    int flags = ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE;
    if (fullscreen) {
        flags = ALLEGRO_FULLSCREEN_WINDOW;
    }
    al_set_new_display_flags(flags);
    al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_SUGGEST);
    
    // Create display
    m_display = al_create_display(actual_width, actual_height);
    if (!m_display) {
        debug << "Failed to create display at " << actual_width << "x" << actual_height << endl;
        
        // Fallback to default
        actual_width = SCREEN_WIDTH;
        actual_height = SCREEN_HEIGHT;
        al_set_new_display_flags(ALLEGRO_WINDOWED);
        m_display = al_create_display(actual_width, actual_height);
        
        if (!m_display) {
            debug << "Fatal Error: Unable to create display" << endl;
            return false;
        }
    }
    
    int display_w = al_get_display_width(m_display);
    int display_h = al_get_display_height(m_display);
    int refresh = al_get_display_refresh_rate(m_display);
    debug << "Display created: " << display_w << "x" << display_h 
          << " @ " << refresh << "Hz" << endl;
    
    // Create backbuffer
    if (m_backbuffer) {
        debug << "Destroying old backbuffer..." << endl;
        al_destroy_bitmap(m_backbuffer);
        m_backbuffer = NULL;
    }
    
    debug << "Creating backbuffer: " << SCREEN_WIDTH << "x" << SCREEN_HEIGHT << endl;
    al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
    m_backbuffer = al_create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!m_backbuffer) {
        debug << "Error creating backbuffer" << endl;
        return false;
    }
    
    // Enumerate video modes (for Settings UI)
    if (videomodes.size() == 0) {
        int num_modes = al_get_num_display_modes();
        debug << "Enumerating " << num_modes << " display modes..." << endl;
        
        for (int i = 0; i < num_modes; i++) {
            ALLEGRO_DISPLAY_MODE mode;
            if (al_get_display_mode(i, &mode)) {
                if (mode.width >= 1024 && mode.height >= 768) {
                    VideoMode vm;
                    vm.width = mode.width;
                    vm.height = mode.height;
                    vm.bpp = 32;
                    videomodes.push_back(vm);
                    
                    debug << "  " << mode.width << "x" << mode.height 
                          << " @ " << mode.refresh_rate << "Hz" << endl;
                }
            }
        }
    }
    
    return true;
}
```

**Game.cpp RunGame() frame presentation replacement:**
```cpp
void Game::RunGame()
{
    // ... (existing update logic unchanged) ...
    
    // Calculate scaling (update screen->h reference)
    screen_scaling = (double)al_get_display_height(m_display) / (double)SCREEN_HEIGHT;
    scale_height = (int)((double)m_backbuffer->h * screen_scaling);
    scale_width = (int)((double)m_backbuffer->w * screen_scaling);
    
    // ... (existing debug output unchanged) ...
    
    // Frame presentation - REPLACE stretch_blit()
    int cx = (actual_width - scale_width) / 2;  // Horizontal centering
    
    al_set_target_backbuffer(m_display);
    al_clear_to_color(al_map_rgb(0, 0, 0));  // Black letterbox bars
    
    al_draw_scaled_bitmap(m_backbuffer,
                          0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,  // Source
                          cx, 0, scale_width, scale_height,    // Dest
                          0);                                  // Flags
    
    al_flip_display();
}
```

---

## Summary

The display system migration from Allegro 4 to Allegro 5 is **moderate complexity** with **well-defined scope**:

**Key Changes:**
1. Replace `set_gfx_mode()` → `al_create_display()`
2. Replace `screen` global → `al_get_backbuffer(display)` or display queries
3. Replace `stretch_blit()` → `al_draw_scaled_bitmap()` + `al_flip_display()`
4. Replace DirectX mode enumeration → `al_get_display_mode()`

**Critical Success Factors:**
1. Correct mouse coordinate transformation accounting for letterboxing
2. Proper display reinit for settings changes
3. Thorough resolution/aspect ratio testing

**Recommended Approach:**
1. Start with minimal migration (keep manual backbuffer)
2. Test extensively at multiple resolutions
3. Optimize later if needed (display backbuffer + transforms)

**Estimated Effort:** 1-2 days for core migration + 1-2 days testing/debugging = **2-4 days total**

---

## Next Steps

After display system migration is complete and tested:

1. **Bitmap operations migration** - All the `blit()`, `masked_blit()`, `draw_sprite()` calls
2. **Drawing primitives migration** - `rectfill()`, `line()`, `circle()`, etc.
3. **Color system migration** - Convert all color macros to ALLEGRO_COLOR
4. **Input system migration** - Event-driven keyboard/mouse instead of polling

Each can build upon the display foundation established here.
