cmake_minimum_required(VERSION 3.16)

project(StarflightTLC VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Custom find modules live here.
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(AllegroLegacy REQUIRED)

# PkgConfig is optional on Windows (vcpkg provides Lua differently)
find_package(PkgConfig QUIET)

# ─────────────────────────────────────────────────────────────────────────────
# Platform-specific configuration
# ─────────────────────────────────────────────────────────────────────────────

if(WIN32)
    # Windows: Use vcpkg or manual configuration
    # Try to find Lua via CMake first (vcpkg provides this)
    find_package(Lua QUIET)
    if(Lua_FOUND)
        set(TLC_LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
        set(TLC_LUA_LIBRARIES ${LUA_LIBRARIES})
        set(TLC_LUA_LIBRARY_DIRS "")
    else()
        message(FATAL_ERROR "Lua not found. Please install via vcpkg: vcpkg install lua")
    endif()
elseif(PkgConfig_FOUND)

# Lua: Prefer pkg-config, since Homebrew no longer ships lua@5.1.
# If you have a local Lua 5.1 install that exposes a lua5.1.pc, this will find it.
# Otherwise, we fall back to plain 'lua' via pkg-config (likely 5.4).
#
# On macOS/Homebrew, pkg-config commonly returns "-llua" (no version suffix).
# The actual dylib exists (e.g. /opt/homebrew/lib/liblua.dylib), but the link
# can still fail if the link directory is not on the default search path.
# Capture -L flags so targets can add them explicitly.
    pkg_check_modules(LUA51 QUIET lua5.1)
    if(LUA51_FOUND)
        set(TLC_LUA_INCLUDE_DIRS ${LUA51_INCLUDE_DIRS})
        set(TLC_LUA_LIBRARIES ${LUA51_LIBRARIES})
        set(TLC_LUA_LIBRARY_DIRS ${LUA51_LIBRARY_DIRS})
    else()
        pkg_check_modules(LUA REQUIRED lua)
        set(TLC_LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIRS})
        set(TLC_LUA_LIBRARIES ${LUA_LIBRARIES})
        set(TLC_LUA_LIBRARY_DIRS ${LUA_LIBRARY_DIRS})
    endif()
else()
    # Fallback: Try CMake's FindLua
    find_package(Lua REQUIRED)
    set(TLC_LUA_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
    set(TLC_LUA_LIBRARIES ${LUA_LIBRARIES})
    set(TLC_LUA_LIBRARY_DIRS "")
endif()

add_subdirectory(src)
