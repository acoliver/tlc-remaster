# Dependencies Plan: Replace Stubs with Real Implementations

**Goal:** Replace the temporary stub implementations with real, working libraries.

**Current State:** The game compiles and links, but audio, fonts, and procedural noise are all stubbed out (non-functional).

---

## 1. AlFont → Allegro 5 Font Addon

### Current Situation
- `src/alfont.h` is a stub with empty function implementations
- Game uses AlFont API: `alfont_load_font()`, `alfont_textprintf_ex()`, `alfont_set_font_size()`, etc.
- Fonts don't render (all alfont functions are no-ops)

### Solution: Use Allegro 5 Font Addon

Allegro 5 includes built-in font support that's already installed via Homebrew:
- `allegro_font` - Core font routines
- `allegro_ttf` - TrueType font support via FreeType

### Implementation Steps

#### 1.1 Create Compatibility Wrapper

Create `src/alfont_compat.h` and `src/alfont_compat.cpp` that wraps Allegro 5 font API to match AlFont API:

```cpp
// alfont_compat.h
#ifndef ALFONT_COMPAT_H
#define ALFONT_COMPAT_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// ALFONT_FONT is now a wrapper around ALLEGRO_FONT
typedef struct ALFONT_FONT {
    ALLEGRO_FONT *a5_font;
    char *filename;      // Store filename for reloading at different sizes
    int current_size;
} ALFONT_FONT;

#define ALFONT_OK 0
#define ALFONT_ERROR -1

int alfont_init(void);
void alfont_exit(void);

ALFONT_FONT *alfont_load_font(const char *filename);
void alfont_destroy_font(ALFONT_FONT *font);

void alfont_set_font_size(ALFONT_FONT *font, int size);
int alfont_text_height(ALFONT_FONT *font);
int alfont_text_length(ALFONT_FONT *font, const char *text);

void alfont_textout(BITMAP *bmp, ALFONT_FONT *font, const char *text, int x, int y, int color);
void alfont_textout_centre(BITMAP *bmp, ALFONT_FONT *font, const char *text, int x, int y, int color);
void alfont_textout_right(BITMAP *bmp, ALFONT_FONT *font, const char *text, int x, int y, int color);
void alfont_textprintf(BITMAP *bmp, ALFONT_FONT *font, int x, int y, int color, const char *format, ...);
void alfont_textprintf_ex(BITMAP *bmp, ALFONT_FONT *font, int x, int y, int color, int bg, const char *format, ...);

#endif
```

#### 1.2 Implementation Notes

The tricky part is that AlFont draws to Allegro 4 `BITMAP*` while Allegro 5 fonts draw to `ALLEGRO_BITMAP*`. 

With Allegro Legacy, we can use:
- `all_get_a5_bitmap(BITMAP*)` - Convert A4 bitmap to A5 bitmap
- Draw text using A5 font routines
- Changes automatically reflect in the A4 bitmap

#### 1.3 CMake Changes

```cmake
# Find Allegro 5 font addons
find_library(ALLEGRO_FONT_LIB allegro_font PATHS /opt/homebrew/opt/allegro/lib)
find_library(ALLEGRO_TTF_LIB allegro_ttf PATHS /opt/homebrew/opt/allegro/lib)

target_link_libraries(starflighttlc PRIVATE
  ${ALLEGRO_FONT_LIB}
  ${ALLEGRO_TTF_LIB}
)
```

#### 1.4 Files to Modify
- [ ] Delete `src/alfont.h` (stub)
- [ ] Create `src/alfont_compat.h`
- [ ] Create `src/alfont_compat.cpp`
- [ ] Update `src/CMakeLists.txt` to compile new file and link font libraries
- [ ] Update includes in source files if needed

---

## 2. libnoise → Real Implementation

### Current Situation
- Headers exist in `src/build/include/noise/`
- Created a shim library `tlc_noise_shim` with placeholder implementations
- Procedural terrain/planet generation doesn't work properly

### Solution: Build Real libnoise

Use `eXpl0it3r/libnoise` fork (most maintained, has CMake support).

### Implementation Steps

#### 2.1 Clone and Build libnoise

```bash
cd /Users/acoliver/projects/tlc-remaster/deps
git clone https://github.com/eXpl0it3r/libnoise.git
cd libnoise
mkdir build && cd build
cmake ..
make
# Creates libnoise.a or libnoise.dylib
```

#### 2.2 Update CMake

```cmake
# Option A: Use as subdirectory
add_subdirectory(deps/libnoise)
target_link_libraries(starflighttlc PRIVATE noise)

# Option B: Use pre-built library
find_library(LIBNOISE_LIB noise PATHS ${CMAKE_SOURCE_DIR}/deps/libnoise/build)
target_include_directories(starflighttlc PRIVATE ${CMAKE_SOURCE_DIR}/deps/libnoise/include)
target_link_libraries(starflighttlc PRIVATE ${LIBNOISE_LIB})
```

#### 2.3 Files to Modify
- [ ] Clone `eXpl0it3r/libnoise` into `deps/libnoise/`
- [ ] Build libnoise library
- [ ] Remove `tlc_noise_shim` from CMake
- [ ] Remove old headers from `src/build/include/noise/` (use deps/libnoise/include instead)
- [ ] Update `src/CMakeLists.txt` to link real libnoise
- [ ] Update include paths in CMake

#### 2.4 Verify

Files that use libnoise:
- `noiseutils.cpp`
- `TexturedSphere.cpp`  
- `PlanetaryBody.cpp`

Test by checking if planet textures generate correctly.

---

## 3. FMOD → Allegro 5 Audio Addon

### Current Situation
- `src/fmod.h` and `src/fmod.hpp` are stubs
- `src/AudioSystem_stub.cpp` has empty implementations
- No sound in the game

### Solution: Use Allegro 5 Audio Addon

Allegro 5 audio is already installed via Homebrew:
- `allegro_audio` - Core audio routines
- `allegro_acodec` - Audio codecs (OGG, WAV, FLAC, MP3)

### Implementation Steps

#### 3.1 Analyze Current FMOD Usage

First, understand what AudioSystem does:

```bash
# Check the original AudioSystem.cpp (not the stub)
cat src/AudioSystem.cpp
```

Key FMOD features used:
- Load and play sound effects (WAV)
- Load and play music (OGG/MP3)
- Adjust volume
- 3D positional audio (maybe)
- Looping

#### 3.2 Create New AudioSystem Implementation

Replace `AudioSystem_stub.cpp` with `AudioSystem_allegro.cpp`:

```cpp
// AudioSystem_allegro.cpp
#include "AudioSystem.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

// Map FMOD concepts to Allegro 5:
// FMOD::System  -> ALLEGRO_VOICE + ALLEGRO_MIXER
// FMOD::Sound   -> ALLEGRO_SAMPLE
// FMOD::Channel -> ALLEGRO_SAMPLE_INSTANCE

bool AudioSystem::Init() {
    if (!al_install_audio()) {
        return false;
    }
    if (!al_init_acodec_addon()) {
        return false;
    }
    
    // Create voice and mixer
    voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    al_attach_mixer_to_voice(mixer, voice);
    
    return true;
}

void AudioSystem::Shutdown() {
    al_destroy_mixer(mixer);
    al_destroy_voice(voice);
    al_uninstall_audio();
}

// ... implement other methods
```

#### 3.3 API Mapping

| FMOD API | Allegro 5 Equivalent |
|----------|---------------------|
| `FMOD::System::createSound()` | `al_load_sample()` |
| `FMOD::System::playSound()` | `al_play_sample()` or `al_play_sample_instance()` |
| `FMOD::Channel::setVolume()` | `al_set_sample_instance_gain()` |
| `FMOD::Channel::setPaused()` | `al_set_sample_instance_playing()` |
| `FMOD::Channel::stop()` | `al_stop_sample_instance()` |
| `FMOD::Sound::release()` | `al_destroy_sample()` |
| `FMOD::System::update()` | Not needed (Allegro handles internally) |

#### 3.4 CMake Changes

```cmake
# Find Allegro 5 audio addons
find_library(ALLEGRO_AUDIO_LIB allegro_audio PATHS /opt/homebrew/opt/allegro/lib)
find_library(ALLEGRO_ACODEC_LIB allegro_acodec PATHS /opt/homebrew/opt/allegro/lib)

target_link_libraries(starflighttlc PRIVATE
  ${ALLEGRO_AUDIO_LIB}
  ${ALLEGRO_ACODEC_LIB}
)
```

#### 3.5 Files to Modify
- [ ] Read original `src/AudioSystem.cpp` to understand full API
- [ ] Create `src/AudioSystem_allegro.cpp` with real implementation
- [ ] Delete `src/AudioSystem_stub.cpp`
- [ ] Delete stub headers `src/fmod.h` and `src/fmod.hpp`
- [ ] Update `src/AudioSystem.h` to remove FMOD types, use Allegro types
- [ ] Update `src/CMakeLists.txt` to use new implementation and link audio libraries
- [ ] Remove FMOD includes from other files (Game.cpp, Module*.h files)

#### 3.6 Audio File Compatibility

Check what audio formats the game uses:

```bash
find bin/data -name "*.wav" -o -name "*.ogg" -o -name "*.mp3" | head -20
```

Allegro 5 acodec supports: WAV, OGG, FLAC, MP3, IT, XM, S3M, MOD

---

## 4. Implementation Order

Recommended order (easiest to hardest):

### Phase 1: libnoise (Low risk, isolated)
1. Clone and build eXpl0it3r/libnoise
2. Update CMake to use real library
3. Test planet generation

### Phase 2: Allegro Font (Medium complexity)
1. Create alfont compatibility wrapper
2. Initialize Allegro 5 font addon in Game::Init()
3. Test text rendering

### Phase 3: Allegro Audio (Most complex)
1. Analyze original AudioSystem.cpp thoroughly
2. Create new implementation with Allegro 5 audio
3. Test sound effects and music
4. Handle edge cases (looping, volume, etc.)

---

## 5. Testing Checklist

### libnoise
- [ ] Planet surface textures generate (not flat/blank)
- [ ] Different planet types have different terrain
- [ ] No crashes in PlanetaryBody or TexturedSphere

### AlFont / Allegro Font
- [ ] Title screen text renders
- [ ] In-game UI text renders
- [ ] Different font sizes work
- [ ] Text is readable and properly positioned

### Audio
- [ ] Music plays on title screen
- [ ] Sound effects play (button clicks, etc.)
- [ ] Volume controls work
- [ ] No audio glitches or crashes
- [ ] Game doesn't hang if audio files missing

---

## 6. Fallback Options

If any implementation proves too difficult:

### AlFont Fallback
- Use Allegro 5's built-in bitmap font (`al_create_builtin_font()`) for basic text
- Less pretty but functional

### libnoise Fallback  
- Keep shim but improve it with basic Perlin noise implementation
- Won't match original quality but will show something

### Audio Fallback
- Use SDL2_mixer instead of Allegro audio (more documentation available)
- Or use miniaudio (single-header library, very portable)

---

## 7. Dependencies Summary

| Original | Replacement | Source | License |
|----------|-------------|--------|---------|
| AlFont 2.0.9 | Allegro 5 Font + TTF | Homebrew allegro | zlib |
| libnoise 1.0 | eXpl0it3r/libnoise | GitHub | LGPL-2.1 |
| FMOD Ex 4.x | Allegro 5 Audio | Homebrew allegro | zlib |

All replacements are open source and free for commercial use.
