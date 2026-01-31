# FindAllegroLegacy.cmake
#
# Custom finder for Allegro Legacy (Allegro 4 compatibility layer)
# built locally in this repo under deps/allegro-legacy/build.
#
# Provides:
#   AllegroLegacy::allegrolegacy  (INTERFACE target)
#
# Variables:
#   AllegroLegacy_FOUND
#   AllegroLegacy_INCLUDE_DIRS
#   AllegroLegacy_LIBRARY_DIRS
#   AllegroLegacy_LIBRARIES

include(FindPackageHandleStandardArgs)

# Paths - adjust if needed
set(_ALLEGROLEGACY_ROOT "${CMAKE_CURRENT_LIST_DIR}/../deps/allegro-legacy")
get_filename_component(_ALLEGROLEGACY_ROOT "${_ALLEGROLEGACY_ROOT}" ABSOLUTE)

set(_ALLEGROLEGACY_SRC_INCLUDE "${_ALLEGROLEGACY_ROOT}/include")
set(_ALLEGROLEGACY_BUILD_INCLUDE "${_ALLEGROLEGACY_ROOT}/build/include")
set(_ALLEGROLEGACY_LIB "${_ALLEGROLEGACY_ROOT}/build/lib")

# Homebrew Allegro 5 paths (required by Allegro Legacy at runtime)
set(_ALLEGRO5_BREW_PREFIX "/opt/homebrew/opt/allegro")
set(_ALLEGRO5_BREW_INCLUDE "${_ALLEGRO5_BREW_PREFIX}/include")
set(_ALLEGRO5_BREW_LIB "${_ALLEGRO5_BREW_PREFIX}/lib")

# On macOS, libc++'s <cmath> expects to include its own wrapper <math.h> from
# the C++ headers directory. If third-party -I/-isystem paths are searched
# before libc++'s headers, that lookup can break. Treat the Allegro includes as
# SYSTEM and add them *after* our own includes.

# Find allegro.h in source include
find_path(AllegroLegacy_INCLUDE_DIR
  NAMES allegro.h
  PATHS "${_ALLEGROLEGACY_SRC_INCLUDE}"
  NO_DEFAULT_PATH
)

# Find liballeg library
find_library(AllegroLegacy_ALLEG_LIB
  NAMES alleg
  PATHS "${_ALLEGROLEGACY_LIB}"
  NO_DEFAULT_PATH
)

# CRITICAL: Include order matches what works in tests/CMakeLists.txt:
# 1. Source include (has allegro.h and allegro/*.h)
# 2. Build include (has generated allegro/platform/alplatf.h, alunixac.h)
# 3. Homebrew Allegro 5 include
#
# The generated headers in build/include/allegro/platform/ will be found
# when allegro.h includes "allegro/platform/alplatf.h" because the source
# include's allegro/platform/ does NOT have alplatf.h.
set(AllegroLegacy_INCLUDE_DIRS
  "${_ALLEGROLEGACY_SRC_INCLUDE}"
  "${_ALLEGROLEGACY_BUILD_INCLUDE}"
  "${_ALLEGRO5_BREW_INCLUDE}"
)

set(AllegroLegacy_LIBRARY_DIRS
  "${_ALLEGROLEGACY_LIB}"
  "${_ALLEGRO5_BREW_LIB}"
)

set(AllegroLegacy_LIBRARIES
  "${AllegroLegacy_ALLEG_LIB}"
)

find_package_handle_standard_args(AllegroLegacy
  REQUIRED_VARS
    AllegroLegacy_INCLUDE_DIR
    AllegroLegacy_ALLEG_LIB
)

if(AllegroLegacy_FOUND)
  if(NOT TARGET AllegroLegacy::allegrolegacy)
    add_library(AllegroLegacy::allegrolegacy INTERFACE IMPORTED)

    target_include_directories(AllegroLegacy::allegrolegacy SYSTEM INTERFACE
      ${AllegroLegacy_INCLUDE_DIRS}
    )

    target_link_directories(AllegroLegacy::allegrolegacy INTERFACE
      ${AllegroLegacy_LIBRARY_DIRS}
    )

    target_link_libraries(AllegroLegacy::allegrolegacy INTERFACE
      ${AllegroLegacy_LIBRARIES}
    )
  endif()
endif()
