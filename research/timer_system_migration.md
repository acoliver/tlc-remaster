# Timer and System Functions Migration Guide

## Executive Summary

This document details the timer system and core system initialization/shutdown functions for Allegro 5 migration. The TLC codebase uses a custom `Timer` class for game timing alongside Allegro 4 system functions. The migration requires replacing Allegro 4 interrupt-based timing with Allegro 5's event-driven timer system.

**Good news:** The codebase does NOT use `install_int()` or `install_int_ex()` timer callbacks, avoiding the most complex timer migration scenario.

---

## System Functions Overview

### System Initialization/Shutdown

| Function | File | Line | Allegro 4 | Allegro 5 |
|----------|------|------|-----------|-----------|
| System init | Game.cpp | L898 | `allegro_init()` | `al_init()` |
| System shutdown | Game.cpp | L1074 | `allegro_exit()` | `al_uninstall_system()` |
| Timer init | Game.cpp | L947 | `install_timer()` | Not needed (automatic) |

### Delay/Sleep Functions

| Function | Files | Allegro 4 | Allegro 5 |
|----------|-------|-----------|-----------|
| Sleep/delay | Multiple | `rest(ms)` | `al_rest(seconds)` |
| Already defined | allegro5_compat.h | L324 | `#define rest(ms) al_rest((ms)/1000.0)` |

### Message Boxes

| Function | Files | Allegro 4 | Allegro 5 |
|----------|-------|-----------|-----------|
| Error/info dialog | Game.cpp, Player.cpp | `allegro_message()` | `al_show_native_message_box()` |
| MessageBox macro | env.h | L56 | Wraps `allegro_message()` | Update to use A5 |

---

## System Initialization - Game.cpp

### Current Implementation (L895-955)

```cpp
bool Game::InitAllegro()
{
	debug << p_title << " v" << p_version << endl;

	debug << "Firing up Allegro..." << endl;
	if (allegro_init() != 0) {
		return false;
	}

	debug << "Firing up Alfont..." << endl;
	if (alfont_init() != ALFONT_OK) {
		g_game->message("Error initializing font system");
		return false;
	}

	debug << "Firing up graphics system..." << endl;
    if (!Initialize_Graphics()) {
        g_game->fatalerror("Error initializing graphics\n");
        return false;
    }

	debug << "Firing up keyboard and mouse handlers..." << endl;
	if (install_keyboard() != 0) {
		g_game->message("Error initializing keyboard");
		return false;
	}
	// ... mouse setup ...
	
	debug << "Firing up timers..." << endl;
	if (install_timer() != 0) {
		g_game->message("Error initializing timer system");
		return false;
	}

	debug << "Firing up sound system..." << endl;
	audioSystem = new AudioSystem();
	// ... rest of initialization ...
}
```

### Migrated Implementation

```cpp
bool Game::InitAllegro()
{
	debug << p_title << " v" << p_version << endl;

	debug << "Firing up Allegro..." << endl;
	if (!al_init()) {
		return false;
	}

	// Initialize required addons
	if (!al_init_image_addon()) {
		g_game->message("Error initializing image addon");
		return false;
	}

	if (!al_init_font_addon() || !al_init_ttf_addon()) {
		g_game->message("Error initializing font system");
		return false;
	}

	if (!al_init_primitives_addon()) {
		g_game->message("Error initializing primitives addon");
		return false;
	}

	debug << "Firing up graphics system..." << endl;
    if (!Initialize_Graphics()) {
        g_game->fatalerror("Error initializing graphics\n");
        return false;
    }

	debug << "Firing up keyboard and mouse handlers..." << endl;
	if (!al_install_keyboard()) {
		g_game->message("Error initializing keyboard");
		return false;
	}
	
	if (!al_install_mouse()) {
		g_game->message("Error initializing mouse");
		return false;
	}
	
	// Hide the OS cursor immediately after install_mouse
	al_hide_mouse_cursor(display);
	
	// ... mouse button array setup ...

	// Note: install_timer() is NOT needed in Allegro 5
	// Timers are created on-demand with al_create_timer()
	
	debug << "Firing up sound system..." << endl;
	audioSystem = new AudioSystem();
	// ... rest of initialization ...
}
```

**Key Changes:**
1. `allegro_init()` → `al_init()`
2. `install_keyboard()` → `al_install_keyboard()` (returns bool)
3. `install_mouse()` → `al_install_mouse()` (returns bool, not button count)
4. `show_os_cursor(MOUSE_CURSOR_NONE)` → `al_hide_mouse_cursor(display)`
5. `install_timer()` → **REMOVED** (not needed)
6. Add addon initialization calls

---

## System Shutdown - Game.cpp

### Current Implementation (L1070-1075)

```cpp
void Game::Shutdown()
{
	// ... cleanup code ...

	debug << "\nShutdown completed." << endl;

	allegro_exit();
	alfont_exit();
}
```

### Migrated Implementation

```cpp
void Game::Shutdown()
{
	// ... cleanup code ...

	// Destroy Allegro 5 resources
	if (event_queue) {
		al_destroy_event_queue(event_queue);
		event_queue = nullptr;
	}
	
	if (frame_timer) {
		al_destroy_timer(frame_timer);
		frame_timer = nullptr;
	}
	
	if (display) {
		al_destroy_display(display);
		display = nullptr;
	}

	debug << "\nShutdown completed." << endl;

	// Shutdown addons
	al_shutdown_ttf_addon();
	al_shutdown_font_addon();
	al_shutdown_image_addon();
	al_shutdown_primitives_addon();
	
	// System shutdown (replaces allegro_exit)
	al_uninstall_system();
}
```

**Key Changes:**
1. `allegro_exit()` → `al_uninstall_system()`
2. Add explicit resource cleanup (event queue, timers, display)
3. Add addon shutdown calls (optional, but good practice)
4. `alfont_exit()` can be removed if using native Allegro 5 font API

---

## Timer System Architecture

### Custom Timer Class (Timer.h/Timer.cpp)

The codebase has a **custom `Timer` class** that provides high-resolution timing independent of Allegro. This is GOOD - it means we don't rely on Allegro 4's interrupt-based timer system.

**Timer.h:**
```cpp
class Timer
{
private:
	long timer_start;
	long stopwatch_start;
	#if defined(__APPLE__) || defined(__linux__) || defined(_POSIX_SOURCE)
	timeval initial;
	#endif

public:
	Timer(void);
	~Timer(void);
	long getTimer();              // Get milliseconds since Timer creation
	void setTimer(long value);    // Set start value
	long getStartTimeMillis();    // Get elapsed milliseconds since reset()
	void sleep(long ms);          // Blocking sleep (uses busy-wait)
	void reset();                 // Reset timers
	bool stopwatch(long ms);      // Returns true every ms milliseconds
};
```

**Implementation Notes:**
- Uses `gettimeofday()` on macOS/Linux (POSIX)
- Uses `clock()` on Windows
- Provides millisecond-resolution timing
- **Does NOT depend on Allegro** except for including `<allegro.h>` header

**Migration Impact:** The custom `Timer` class can continue to be used as-is. However, the Allegro 5 timer system should be used for frame timing in the main game loop.

---

## Timer Usage Patterns

### Global Timer (Game.h)

```cpp
class Game {
public:
	Timer globalTimer;  // Global game timer
	// ...
};
```

**Usage (22 occurrences across codebase):**

| File | Purpose | Code Pattern |
|------|---------|--------------|
| Game.cpp | FPS limiting | `if (globalTimer.getTimer() < timeStart + fps_delay)` |
| Game.cpp | Game time calculation | `gameState->getBaseGameTimeSecs() + globalTimer.getStartTimeMillis()/1000.0` |
| Game.cpp | Timed text messages | `TimedText message = {str, color, globalTimer.getTimer()+delay}` |
| Game.cpp | Alien attitude updates | `if (globalTimer.getTimer() > alienAttitudeUpdate + 200000)` |
| GameState.cpp | Alien attitude init | `alienAttitudeUpdate = g_game->globalTimer.getTimer()` |
| GameState.cpp | Timer reset on load | `g_game->globalTimer.reset()` |
| ModuleEncounter.cpp | Weapon fire rate | `if (globalTimer.getTimer() > fireLast + fireRate)` |
| CombatObject.cpp | Expiration tracking | `if (globalTimer.getTimer() > expireStartTime + expireDuration)` |
| Util.cpp | Delay function | `if (globalTimer.getTimer() > start + ms)` |

**Migration Strategy:** Keep the custom `Timer` class for game logic timing. It provides consistent cross-platform timing without Allegro dependency.

### Local Timer Instances

| File | Variable | Purpose |
|------|----------|---------|
| ModuleEncounter.h | `Timer scanTimer` | Scan animation timing |
| ModuleEncounter.cpp | `static Timer countdown` | Combat countdown |
| ModuleEncounter.cpp | `static Timer update` | Update throttling |
| ModuleEncounter.cpp | `static Timer timer` (2x) | Weapon reload timers |
| ModuleInterstellar.cpp | `static Timer timerEncounter` | Encounter trigger delay |
| ModuleInterstellar.cpp | `static Timer timerHelp` | Help display timing |
| ModuleInterstellar.h | `Timer timerEngaged` | Engagement state |
| ModuleSolarSystem.cpp | `static Timer timerHelp` | Help display timing |
| PlayerShipSprite.h | `Timer timer, braking_timer` | Ship control timing |

**Migration Strategy:** These can all remain as-is. The custom Timer class is suitable for these use cases.

---

## rest() Sleep Function

### Current Usage (5 occurrences)

| File | Line | Code | Purpose |
|------|------|------|---------|
| Game.cpp | L1118 | `//rest(1);` | **COMMENTED OUT** - FPS limiter |
| Game.cpp | L1288 | `//rest(1);` | **COMMENTED OUT** - Render delay |
| ModuleInterstellar.cpp | L860 | `rest(2500);` | **ACTIVE** - Pause before encounter (2.5 sec) |
| ModuleGameOver.cpp | L44 | `rest(500);` | **ACTIVE** - Delay before returning to title (0.5 sec) |
| allegro5_compat.h | L324 | `#define rest(ms) al_rest((ms)/1000.0)` | **Already migrated!** |

### Allegro 5 Compatibility (Already Done!)

The `allegro5_compat.h` file already provides a compatibility macro:

```cpp
/* Timer/delay functions */
#define rest(ms) al_rest((ms) / 1000.0)
```

**Important:** Allegro 5's `al_rest()` takes **seconds** (double), not milliseconds!

### Active Usage Examples

**ModuleInterstellar.cpp (L855-865):**
```cpp
//if encounter triggered, then launch it
if (flag_launchEncounter)
{
	//pause for encounter sound clip
	rest(2500);  // 2.5 second pause
	g_game->modeMgr->LoadModule(MODULE_ENCOUNTER);
	return;
}
```

**ModuleGameOver.cpp (L40-50):**
```cpp
void ModuleGameOver::OnKeyReleased(int keyCode)
{
	if (keyCode == KEY_ESC) {
		rest(500);  // 0.5 second pause
		g_game->modeMgr->LoadModule(MODULE_TITLESCREEN);
		return;
	}
}
```

**Migration Notes:**
- The compatibility macro ensures these work correctly
- Consider whether blocking delays are appropriate (they freeze the game)
- Alternative: Use timer-based state changes for better responsiveness

---

## allegro_message() Message Boxes

### Current Usage (3 occurrences + 1 macro)

| File | Line | Code | Purpose |
|------|------|------|---------|
| Game.cpp | L165 | `allegro_message(msg.c_str());` | Game::message() error display |
| Player.cpp | L21 | `allegro_message("Error loading is_ship");` | Fatal sprite load error |
| env.h | L56 | `#define MessageBox(...) allegro_message("%s", text)` | Windows compatibility macro |

### Current Implementation - Game.cpp (L160-170)

```cpp
void Game::message(std::string msg)
{
    debug << msg << endl;
	allegro_message(msg.c_str());
}
```

### Migrated Implementation

```cpp
void Game::message(std::string msg)
{
    debug << msg << endl;
	
	// Allegro 5 native message box
	al_show_native_message_box(
		display,                    // Can be nullptr if display not created yet
		"The Last Colony",          // Title
		"Message",                  // Heading
		msg.c_str(),                // Text
		nullptr,                    // Buttons (nullptr = default OK button)
		ALLEGRO_MESSAGEBOX_WARN     // Flags
	);
}
```

**al_show_native_message_box() Signature:**
```cpp
int al_show_native_message_box(
    ALLEGRO_DISPLAY *display,    // Display (can be NULL)
    const char *title,            // Window title
    const char *heading,          // Bold heading text
    const char *text,             // Message body
    const char *buttons,          // Custom buttons (NULL = OK)
    int flags                     // ALLEGRO_MESSAGEBOX_* flags
);
```

**Flags:**
- `ALLEGRO_MESSAGEBOX_WARN` - Warning icon
- `ALLEGRO_MESSAGEBOX_ERROR` - Error icon
- `ALLEGRO_MESSAGEBOX_QUESTION` - Question icon
- `ALLEGRO_MESSAGEBOX_OK_CANCEL` - OK and Cancel buttons
- `ALLEGRO_MESSAGEBOX_YES_NO` - Yes and No buttons

### Player.cpp Migration (L18-25)

**Before:**
```cpp
this->ship = new Sprite();
this->ship->load("data/is_ship.bmp");
if (!this->ship) {
	allegro_message("Error loading is_ship");
}
```

**After:**
```cpp
this->ship = new Sprite();
this->ship->load("data/is_ship.bmp");
if (!this->ship) {
	al_show_native_message_box(
		nullptr,
		"The Last Colony",
		"Fatal Error",
		"Error loading is_ship",
		nullptr,
		ALLEGRO_MESSAGEBOX_ERROR
	);
}
```

### env.h MessageBox Macro (L52-60)

**Current:**
```cpp
// MessageBox fallback for non-Windows
#ifndef TLC_PLATFORM_WINDOWS
    #define MessageBox(hwnd, text, caption, type) allegro_message("%s", text)
#endif
```

**Migrated:**
```cpp
// MessageBox fallback for non-Windows
#ifndef TLC_PLATFORM_WINDOWS
    #define MessageBox(hwnd, text, caption, type) \
        al_show_native_message_box(nullptr, caption, caption, text, nullptr, ALLEGRO_MESSAGEBOX_WARN)
#endif
```

**Note:** This macro is for Windows API compatibility. The `type` parameter maps to Windows MB_* constants, which could be mapped to Allegro 5 flags if needed.

---

## Allegro 5 Event-Based Timer System

While the custom `Timer` class handles game logic timing, Allegro 5's event-based timer system should be used for the main game loop frame timing.

### Recommended Main Loop Architecture

```cpp
class Game {
private:
    ALLEGRO_TIMER *frame_timer;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_DISPLAY *display;
    
public:
    bool InitAllegro() {
        // ... existing init ...
        
        // Create frame timer (60 FPS)
        frame_timer = al_create_timer(1.0 / 60.0);
        if (!frame_timer) {
            message("Failed to create frame timer");
            return false;
        }
        
        // Create event queue
        event_queue = al_create_event_queue();
        if (!event_queue) {
            message("Failed to create event queue");
            return false;
        }
        
        // Register event sources
        al_register_event_source(event_queue, 
            al_get_display_event_source(display));
        al_register_event_source(event_queue, 
            al_get_keyboard_event_source());
        al_register_event_source(event_queue, 
            al_get_mouse_event_source());
        al_register_event_source(event_queue, 
            al_get_timer_event_source(frame_timer));
        
        // Start the timer
        al_start_timer(frame_timer);
        
        return true;
    }
    
    void MainLoop() {
        bool running = true;
        bool redraw = false;
        
        while (running) {
            ALLEGRO_EVENT event;
            al_wait_for_event(event_queue, &event);
            
            switch (event.type) {
                case ALLEGRO_EVENT_TIMER:
                    // Frame update
                    Update();
                    redraw = true;
                    break;
                    
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    running = false;
                    break;
                    
                case ALLEGRO_EVENT_KEY_DOWN:
                case ALLEGRO_EVENT_KEY_UP:
                    HandleKeyboard(event);
                    break;
                    
                case ALLEGRO_EVENT_MOUSE_AXES:
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                    HandleMouse(event);
                    break;
            }
            
            // Only render when timer fires and event queue is empty
            if (redraw && al_is_event_queue_empty(event_queue)) {
                redraw = false;
                Render();
                al_flip_display();
            }
        }
    }
};
```

### Benefits of Event-Based Timing

1. **Consistent frame rate** - Timer events fire at exact intervals
2. **No busy waiting** - `al_wait_for_event()` blocks efficiently
3. **Decoupled update/render** - Can skip frames if needed
4. **Platform independent** - Allegro handles OS differences
5. **Lower CPU usage** - No spinning in tight loops

### Integration with Existing Code

The current code uses `globalTimer.getTimer()` for FPS limiting (Game.cpp L1115-1129):

**Current Pattern:**
```cpp
static int timeStart = globalTimer.getTimer();

if (globalTimer.getTimer() < timeStart + (int)fps_delay)
{
    //slow down core loop
    //rest(1);  // Commented out
    fps_delay = 1000.0f / frameRate;
}

// ... update/render ...

timeStart = globalTimer.getTimer();
```

**Migrated Pattern:**
```cpp
// This entire block can be REMOVED when using event-based timing
// The ALLEGRO_EVENT_TIMER events provide frame timing

// In Update():
// Just process one frame of game logic

// In Render():
// Just draw one frame
```

---

## No Timer Callbacks (Good News!)

The codebase was checked for `install_int()` and `install_int_ex()` - these are Allegro 4 functions that install timer interrupt callbacks.

**Search Results:** 
- `install_int()`: **0 occurrences**
- `install_int_ex()`: **0 occurrences**

**This is excellent news!** These would have been the most complex migration:

### What We Avoided (Comparison)

**Allegro 4 Pattern (NOT USED):**
```cpp
// This pattern is NOT in the codebase - shown for reference only
volatile int frame_counter = 0;

void timer_callback() {
    frame_counter++;
}
END_OF_FUNCTION(timer_callback);

// In init:
LOCK_VARIABLE(frame_counter);
LOCK_FUNCTION(timer_callback);
install_int_ex(timer_callback, BPS_TO_TIMER(60));

// In main loop:
while (frame_counter > 0) {
    frame_counter--;
    update_game();
}
```

**Allegro 5 Equivalent:**
```cpp
// Use event-based timers (shown in previous section)
case ALLEGRO_EVENT_TIMER:
    update_game();
    break;
```

Since the codebase doesn't use timer callbacks, the migration is straightforward.

---

## Timer.cpp Analysis

### Platform-Specific Timing Implementation

**Constructor (L9-17):**
```cpp
Timer::Timer(void)
{
	#if defined(__APPLE__) || defined(__linux__) || defined(_POSIX_SOURCE)
	gettimeofday(&initial, NULL);
	#endif

	reset();
}
```

**getTimer() Implementation (L20-36):**
```cpp
long Timer::getTimer()
{
	#if defined(_WIN32) || defined(_WIN64)
	return (long) clock();

	#elif defined(__APPLE__) || defined(__linux__) || defined(_POSIX_SOURCE)

	timeval current, delta;
	gettimeofday(&current, NULL);
	timersub(&current, &initial, &delta);
	return (long) (delta.tv_sec*1000 + delta.tv_usec/1000);

	#else
		#error Could not determine the function to get wall-clock time

	#endif
}
```

**Key Points:**
- Returns milliseconds elapsed since Timer creation
- Uses high-resolution timing:
  - macOS/Linux: `gettimeofday()` (~1μs resolution)
  - Windows: `clock()` (lower resolution)
- **Does NOT use Allegro timing functions**
- Can be used in migrated code without changes

### sleep() Warning (L50-55)

```cpp
//warning: this is a blocking sleep
void Timer::sleep(long ms)
{
	long start = getTimer();
	while (start + ms > getTimer());
}
```

**This is a BUSY WAIT!** It will consume 100% CPU during the delay. The comment even warns about this.

**Recommendation:** Replace usage with:
- Allegro 5: `al_rest(seconds)`
- C++11: `std::this_thread::sleep_for(std::chrono::milliseconds(ms))`

**Current Usage:** Not directly used in search results, but check if called anywhere.

### stopwatch() Method (L62-70)

```cpp
bool Timer::stopwatch(long ms)
{
	if ( getTimer() > stopwatch_start + ms ) {
		stopwatch_start = getTimer();
		return true;
	}
	else return false;
}
```

**Usage Pattern:** Returns true every `ms` milliseconds, used for periodic updates.

**Example from PlayerShipSprite.cpp (L120):**
```cpp
if (!timer.stopwatch(TIMER_RATE)) return;
// Update ship state
```

**This is good design** - decouples update frequency from frame rate. Keep this pattern.

---

## Migration Checklist

### Phase 1: System Functions
- [ ] Replace `allegro_init()` with `al_init()` (Game.cpp L898)
- [ ] Add addon initialization calls
- [ ] Replace `install_keyboard()` with `al_install_keyboard()`
- [ ] Replace `install_mouse()` with `al_install_mouse()`
- [ ] Remove `install_timer()` call (Game.cpp L947)
- [ ] Replace `show_os_cursor(MOUSE_CURSOR_NONE)` with `al_hide_mouse_cursor(display)`
- [ ] Replace `allegro_exit()` with `al_uninstall_system()` (Game.cpp L1074)
- [ ] Add resource cleanup (display, event queue, timers)

### Phase 2: Message Boxes
- [ ] Update `Game::message()` to use `al_show_native_message_box()` (Game.cpp L165)
- [ ] Update Player.cpp error message (L21)
- [ ] Update env.h MessageBox macro (L56)

### Phase 3: Timer System
- [ ] Create ALLEGRO_TIMER for frame timing
- [ ] Create ALLEGRO_EVENT_QUEUE
- [ ] Register event sources (display, keyboard, mouse, timer)
- [ ] Implement event-based main loop
- [ ] Keep custom Timer class for game logic timing
- [ ] Verify rest() compatibility macro works (already in allegro5_compat.h)

### Phase 4: Testing
- [ ] Test system initialization/shutdown
- [ ] Test message box display (pre-display and post-display creation)
- [ ] Test frame timing consistency
- [ ] Test game timing (globalTimer behavior)
- [ ] Test rest() delays (encounter transition, game over)
- [ ] Verify no timer callback dependencies
- [ ] Profile CPU usage (ensure no busy waits)

---

## Risk Assessment

### Low Risk
[OK] **System init/shutdown** - Straightforward API replacement  
[OK] **No timer callbacks** - Avoids most complex migration scenario  
[OK] **rest() already handled** - Compatibility macro exists  
[OK] **Custom Timer class** - Independent of Allegro, can keep as-is  

### Medium Risk
WARNING: **Message boxes** - Need to test pre-display initialization  
WARNING: **Event-based main loop** - Architectural change, needs careful testing  
WARNING: **FPS limiting** - Current manual timing to be replaced  

### Testing Priority
1. **System initialization order** - Ensure all addons load correctly
2. **Early error messages** - Test message boxes before display creation
3. **Frame timing** - Verify 60 FPS consistency
4. **Game timing** - Ensure globalTimer still works correctly
5. **Module transitions** - Test rest() delays don't freeze input

---

## Performance Considerations

### Current FPS Limiting (Game.cpp L1115-1129)

The current code uses manual timing with busy-wait checking:

```cpp
if (globalTimer.getTimer() < timeStart + (int)fps_delay) {
    //rest(1);  // Commented out, so it's BUSY WAITING!
}
```

**Problem:** With rest(1) commented out, this is a busy loop that wastes CPU.

**Solution:** Allegro 5 event-based timing with `al_wait_for_event()` efficiently blocks until the next timer event, using **zero CPU while waiting**.

### CPU Usage Comparison

| Method | CPU Usage While Waiting | Precision |
|--------|-------------------------|-----------|
| Busy wait (current) | 100% on one core | Very high |
| `rest(1)` | ~0%, but 1ms minimum | Low (1ms resolution) |
| `al_rest(0.001)` | ~0% | Low (OS scheduler) |
| `al_wait_for_event()` | ~0% | High (timer precision) |

**Recommendation:** The event-based approach provides both high precision and low CPU usage.

---

## allegro5_compat.h Integration

### Current Timer Definitions (L323-324)

```cpp
/*===========================================================================*
 * SYSTEM COMPATIBILITY
 *===========================================================================*/

/* Timer/delay functions */
#define rest(ms) al_rest((ms) / 1000.0)
```

### Additional Compatibility Macros Needed

```cpp
/* System initialization (add to allegro5_compat.h) */
#define allegro_init() al_init()
#define allegro_exit() al_uninstall_system()
#define install_timer() (0)  // No-op, returns success

/* Message boxes */
#define allegro_message(fmt, ...) \
    al_show_native_message_box(nullptr, "The Last Colony", "Message", \
        (fmt), nullptr, ALLEGRO_MESSAGEBOX_WARN)
```

**Note:** The message box macro is simplified. For full printf-style formatting, a wrapper function is needed:

```cpp
// In allegro5_compat.cpp
void allegro_message(const char *fmt, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    al_show_native_message_box(
        nullptr,
        "The Last Colony",
        "Message",
        buffer,
        nullptr,
        ALLEGRO_MESSAGEBOX_WARN
    );
}
```

---

## Code Locations Reference

### System Functions
- **allegro_init()**: Game.cpp L898
- **allegro_exit()**: Game.cpp L1074, hello_allegro.cpp L13
- **install_timer()**: Game.cpp L947
- **rest()**: 
  - ModuleInterstellar.cpp L860 (2500ms)
  - ModuleGameOver.cpp L44 (500ms)
  - Game.cpp L1118, L1288 (commented out)

### Message Boxes
- **allegro_message()**: 
  - Game.cpp L165 (in Game::message())
  - Player.cpp L21
- **MessageBox macro**: env.h L56

### Custom Timer Class
- **Definition**: Timer.h (34 lines)
- **Implementation**: Timer.cpp (71 lines)
- **Global instance**: Game.h L188 (globalTimer)
- **Local instances**:
  - ModuleEncounter.h L173
  - ModuleInterstellar.cpp L489, L851
  - ModuleInterstellar.h L52
  - ModuleSolarSystem.cpp L414
  - PlayerShipSprite.h L25
  - ModuleEncounter.cpp L1590, L1591, L2959, L2990

---

## Recommended Migration Order

1. **Update allegro5_compat.h** - Add system function macros
2. **Update Game.cpp initialization** - Replace system init calls
3. **Update Game.cpp shutdown** - Replace system shutdown calls
4. **Add event system** - Create timer, event queue
5. **Update main loop** - Implement event-based timing
6. **Update message boxes** - Replace allegro_message() calls
7. **Test thoroughly** - Verify timing, messages, system lifecycle

---

## Testing Strategy

### Unit Tests
- [ ] Test allegro5_compat.h macros compile correctly
- [ ] Test rest() conversion (ms to seconds)
- [ ] Test Timer class still works (no regression)

### Integration Tests
- [ ] System init/shutdown cycles
- [ ] Message box display at various points
- [ ] Frame timing consistency (60 FPS ±1)
- [ ] Game time calculation accuracy

### Regression Tests
- [ ] Encounter transition delay (rest 2500ms)
- [ ] Game over delay (rest 500ms)
- [ ] Weapon fire rates (use globalTimer)
- [ ] Timed messages (use globalTimer)
- [ ] Ship control timing (use Timer instances)

### Performance Tests
- [ ] CPU usage at idle (should be near 0%)
- [ ] CPU usage during gameplay
- [ ] Frame time consistency
- [ ] Input responsiveness

---

## Conclusion

The timer and system migration is relatively straightforward:

[OK] **Good news:**
- No timer callbacks to migrate
- Custom Timer class is Allegro-independent
- rest() already has compatibility macro
- No complex timing interdependencies

WARNING: **Attention needed:**
- Message box API change
- Event-based main loop architecture
- FPS limiting mechanism replacement
- System initialization order

The migration can proceed in phases without breaking existing functionality. The custom Timer class provides a good abstraction that isolates game logic from the Allegro migration.
