# FindAllegroLegacy.cmake
#
# Finds the Allegro Legacy library (Allegro 4 API on Allegro 5 backend)
#
# This will define the following variables:
#   AllegroLegacy_FOUND        - True if the library was found
#   AllegroLegacy_INCLUDE_DIRS - Include directories
#   AllegroLegacy_LIBRARIES    - Libraries to link
#
# and the following imported target:
#   AllegroLegacy::allegrolegacy
#

# Look for the library in deps/allegro-legacy/build
set(ALLEGRO_LEGACY_ROOT "${CMAKE_SOURCE_DIR}/deps/allegro-legacy")

# Find include directory
find_path(AllegroLegacy_INCLUDE_DIR
    NAMES allegro.h
    PATHS
        "${ALLEGRO_LEGACY_ROOT}/include"
        "${ALLEGRO_LEGACY_ROOT}/build/include"
    NO_DEFAULT_PATH
)

# Platform-specific library name
# Note: Allegro Legacy builds as "liballeg" not "liballegro_legacy"
if(WIN32)
    set(ALLEGRO_LEGACY_LIB_NAME "alleg" "allegro_legacy" "allegro4")
elseif(APPLE)
    set(ALLEGRO_LEGACY_LIB_NAME "alleg" "allegro_legacy")
else()
    set(ALLEGRO_LEGACY_LIB_NAME "alleg" "allegro_legacy")
endif()

# Find library
find_library(AllegroLegacy_LIBRARY
    NAMES ${ALLEGRO_LEGACY_LIB_NAME}
    PATHS
        "${ALLEGRO_LEGACY_ROOT}/build/lib"
        "${ALLEGRO_LEGACY_ROOT}/build"
        "${ALLEGRO_LEGACY_ROOT}/build/Release"
    NO_DEFAULT_PATH
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AllegroLegacy
    REQUIRED_VARS AllegroLegacy_LIBRARY AllegroLegacy_INCLUDE_DIR
)

if(AllegroLegacy_FOUND)
    set(AllegroLegacy_LIBRARIES ${AllegroLegacy_LIBRARY})
    set(AllegroLegacy_INCLUDE_DIRS ${AllegroLegacy_INCLUDE_DIR})
    
    # Also include the build directory for generated headers
    list(APPEND AllegroLegacy_INCLUDE_DIRS "${ALLEGRO_LEGACY_ROOT}/build/include")
    
    if(NOT TARGET AllegroLegacy::allegrolegacy)
        add_library(AllegroLegacy::allegrolegacy UNKNOWN IMPORTED)
        set_target_properties(AllegroLegacy::allegrolegacy PROPERTIES
            IMPORTED_LOCATION "${AllegroLegacy_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${AllegroLegacy_INCLUDE_DIRS}"
        )
    endif()
endif()

mark_as_advanced(AllegroLegacy_INCLUDE_DIR AllegroLegacy_LIBRARY)
