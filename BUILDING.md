# Building Starflight: The Lost Colony

This guide covers building the game from source on Linux, macOS, and Windows.

## Prerequisites

### All Platforms
- CMake 3.16 or newer
- Git (with submodules support)
- C++17 compatible compiler

### Linux (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    liballegro5-dev \
    liballegro-acodec5-dev \
    liballegro-audio5-dev \
    liballegro-font5-dev \
    liballegro-image5-dev \
    liballegro-ttf5-dev \
    liballegro-primitives5-dev \
    liblua5.4-dev \
    libgl1-mesa-dev
```

### macOS

Using [Homebrew](https://brew.sh/):

```bash
brew install allegro lua pkg-config ninja cmake
```

### Windows

Using [vcpkg](https://github.com/microsoft/vcpkg):

```powershell
vcpkg install allegro5 lua --triplet x64-windows
```

You'll also need Visual Studio 2019 or newer with C++ support.

## Clone the Repository

```bash
git clone --recursive https://github.com/acoliver/tlc-remaster.git
cd tlc-remaster
```

If you already cloned without `--recursive`:

```bash
git submodule update --init --recursive
```

## Build Instructions

### Step 1: Build Dependencies

#### Allegro Legacy (all platforms)

```bash
cd deps/allegro-legacy
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cd ../../..
```

#### libnoise (built automatically by main CMake)

The main build will handle libnoise automatically.

### Step 2: Build the Game

#### Linux / macOS

```bash
mkdir -p build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

#### Windows (Visual Studio)

```powershell
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_INSTALLATION_ROOT/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Release
```

## Running the Game

The game must be run from the `bin/` directory (where the `data/` folder is located):

### Linux / macOS

```bash
cd bin
../build/src/starflighttlc
```

### Windows

```powershell
cd bin
..\build\src\Release\starflighttlc.exe
```

## Packaging for Distribution

### Linux

```bash
# After building, create a distributable package
mkdir -p package/starflight-tlc
cp build/src/starflighttlc package/starflight-tlc/
cp -r bin/data package/starflight-tlc/
mkdir -p package/starflight-tlc/saves
cp deps/allegro-legacy/build/lib/*.so* package/starflight-tlc/lib/

# Create launcher script
cat > package/starflight-tlc/starflight-tlc.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"
cd "$SCRIPT_DIR"
exec ./starflighttlc "$@"
EOF
chmod +x package/starflight-tlc/starflight-tlc.sh

# Create tarball
cd package && tar -czvf ../starflight-tlc-linux.tar.gz starflight-tlc
```

### macOS

A `.app` bundle and `.dmg` are created by the CI release workflow. For local testing, you can run the executable directly from the build directory.

### Windows

```powershell
# After building, create a distributable package
mkdir package\starflight-tlc
copy build\src\Release\starflighttlc.exe package\starflight-tlc\
xcopy /E /I bin\data package\starflight-tlc\data
mkdir package\starflight-tlc\saves

# Copy required DLLs (adjust paths as needed)
copy $env:VCPKG_INSTALLATION_ROOT\installed\x64-windows\bin\*.dll package\starflight-tlc\

# Create ZIP
Compress-Archive -Path package\starflight-tlc -DestinationPath starflight-tlc-windows.zip
```

## Troubleshooting

### "Allegro Legacy not found"

Make sure you built Allegro Legacy first (Step 1) and that the build succeeded.

### "Lua not found"

- **Linux**: Install `liblua5.4-dev`
- **macOS**: Install `lua` via Homebrew
- **Windows**: Install `lua` via vcpkg

### "OpenGL not found" (Linux)

```bash
sudo apt-get install libgl1-mesa-dev
```

### Game crashes on startup

Make sure you're running from the `bin/` directory where `data/` is located.

### No sound

Check that `AUDIO_MUSIC = true` in `bin/data/config.lua`.

## Development

### Regenerate compile_commands.json

```bash
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
```

### Debug build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
```
