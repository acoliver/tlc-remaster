# Allegro Legacy - Quick Reference

Source: https://github.com/NewCreature/Allegro-Legacy

## What It Is

Allegro Legacy lets you compile Allegro 4 programs against Allegro 5. You keep your A4 API calls, it runs on modern systems via A5 backend.

## Building Allegro Legacy

Requires Allegro 5 with audio addon enabled.

```bash
mkdir build && cd build
cmake <path_to_allegro_legacy_source>
make
make install
```

## Detection Macro

```cpp
#ifdef ALLEGRO_LEGACY
// Code is being built with Allegro Legacy
#endif
```

## Additional Functions (Beyond A4 API)

| Function | Purpose |
|----------|---------|
| `all_disable_threaded_display()` | Disable emulated display (call before `set_gfx_mode()`) |
| `all_get_display()` | Get the underlying `ALLEGRO_DISPLAY*` |
| `all_get_a5_bitmap(BITMAP*)` | Convert A4 `BITMAP*` to A5 `ALLEGRO_BITMAP*` |
| `all_render_a5_bitmap(BITMAP*, ALLEGRO_BITMAP*)` | Render A4 bitmap to A5 bitmap |
| `all_render_screen()` | Render `screen` to display |
| `all_set_display_transform(ALLEGRO_TRANSFORM*)` | Apply transform when rendering screen |
| `all_adjust_int(void* proc, double speed)` | Adjust timer speed for better precision |
| `all_wait_for_int(void* proc)` | Idle until timer ticks (reduces CPU usage) |

## Timer Loop Improvement Example

```cpp
install_int_ex(increment_tick, BPS_TO_TIMER(60));
#ifdef ALLEGRO_LEGACY
  all_adjust_int(increment_tick, 1.0 / 60.0);
#endif

while(ticks > 0) {
  #ifdef ALLEGRO_LEGACY
    all_wait_for_int(increment_tick);  // Sleep until timer ticks
  #endif
  process_logic();
  ticks--;
}
```

## Compatibility Notes

- Full Allegro 4 API compatibility
- Does NOT support hacked graphics modes (like ex12bit example)
- Licensed under gift-ware (same as Allegro 4)

## Dependencies

- Allegro 5 (with audio addon)
- CMake
