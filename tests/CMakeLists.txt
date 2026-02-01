cmake_minimum_required(VERSION 3.16)
project(tlc_tests CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(hello_allegro hello_allegro.cpp)

# Allegro-Legacy builds a liballeg.* in deps/allegro-legacy/build/lib
set(ALLEGRO_LEGACY_ROOT "${CMAKE_CURRENT_LIST_DIR}/../deps/allegro-legacy")
set(ALLEGRO_LEGACY_BUILD_LIBDIR "${ALLEGRO_LEGACY_ROOT}/build/lib")

target_include_directories(hello_allegro PRIVATE
    "${ALLEGRO_LEGACY_ROOT}/include"
    "${ALLEGRO_LEGACY_ROOT}/build/include"
    "/opt/homebrew/opt/allegro/include"
)

target_link_directories(hello_allegro PRIVATE
    "${ALLEGRO_LEGACY_BUILD_LIBDIR}"
    "/opt/homebrew/opt/allegro/lib"
)

# Library name is 'alleg' (OUTPUT_NAME), file is liballeg.dylib
target_link_libraries(hello_allegro PRIVATE alleg)

# Allegro 5 (Homebrew) provides an entrypoint shim for macOS.
# Allegro-Legacy's headers pull in Allegro 5 headers which rename main.
target_link_libraries(hello_allegro PRIVATE allegro_main)

# Ensure the executable can find liballeg.dylib at runtime without installing.
set_target_properties(hello_allegro PROPERTIES
    BUILD_RPATH "${ALLEGRO_LEGACY_BUILD_LIBDIR}"
)
