# Real libnoise (replaces tlc_noise_shim)
# Build as a subproject so the main build can link to the libnoise target.
# Disable examples and noiseutils - the project has its own noiseutils.cpp
set(BUILD_LIBNOISE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_LIBNOISE_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory("${CMAKE_SOURCE_DIR}/deps/libnoise" "${CMAKE_BINARY_DIR}/deps/libnoise")


add_executable(starflighttlc
  main.cpp

  # Core Engine
  Game.cpp
  GameState.cpp
  Module.cpp
  ModeMgr.cpp
  Timer.cpp
  LogFile.cpp

  # Graphics/UI
  Sprite.cpp
  Button.cpp
  Label.cpp
  ScrollBox.cpp
  MiniWindow.cpp
  MessageBoxWindow.cpp
  PauseMenu.cpp
  TileScroller.cpp

  # Game Modules
  ModuleAuxiliaryDisplay.cpp
  ModuleBank.cpp
  ModuleCantina.cpp
  ModuleCaptainCreation.cpp
  ModuleCaptainsLounge.cpp
  ModuleCargoWindow.cpp
  ModuleControlPanel.cpp
  ModuleCredits.cpp
  ModuleCrewHire.cpp
  ModuleEncounter.cpp
  ModuleEngineer.cpp
  ModuleGameOver.cpp
  ModuleInterstellar.cpp
  ModuleMedical.cpp
  ModuleMessageGUI.cpp
  ModuleMiniGame.cpp
  ModulePlanetOrbit.cpp
  ModulePlanetSurface.cpp
  ModuleQuestLog.cpp
  ModuleSettings.cpp
  ModuleShipConfig.cpp
  ModuleSideViewer.cpp
  ModuleSolarSystem.cpp
  ModuleStarmap.cpp
  ModuleStarport.cpp
  ModuleStartup.cpp
  ModuleTitleScreen.cpp
  ModuleTopGUI.cpp
  ModuleTradeDepot.cpp

  # Game Logic
  Player.cpp
  Archive.cpp
  DataMgr.cpp
  QuestMgr.cpp
  Quest.cpp
  QuestDependency.cpp
  Requirement.cpp
  Script.cpp
  AudioSystem_allegro.cpp
  Stardate.cpp
  GameTime.cpp
  Flux.cpp
  CombatObject.cpp
  FactionStanding.cpp
  CaptainType.cpp
  CargoSize.cpp
  CollectItem.cpp
  Interact.cpp
  KillAnimal.cpp
  TotalQuestsCompleted.cpp

  # Planet/Space
  PlanetaryBody.cpp
  PlanetTileScroller.cpp
  PlanetSurfaceObject.cpp
  PlanetSurfacePlayerVessel.cpp
  PlanetScan.cpp
  TexturedSphere.cpp
  OrbitPlanet.cpp
  PlayerShipSprite.cpp
  TerrainVehicleSprite.cpp
  CombatPlayerVessel.cpp

  # Utilities
  Util.cpp
  Math.cpp
  Vector3.cpp
  Model.cpp
  debug.cpp
  noiseutils.cpp

  # AlFont compatibility (Allegro 5 font/ttf -> Allegro 4 BITMAP*)
  alfont_compat.cpp

  # Bundled Libraries (TinyXML)
  tinyxml/tinystr.cpp
  tinyxml/tinyxml.cpp
  tinyxml/tinyxmlerror.cpp
  tinyxml/tinyxmlparser.cpp
)

# Disable Allegro Legacy's fixed-point math aliases (fadd, fsub, etc.)
# that conflict with glibc's mathcalls-narrow.h on Linux
target_compile_definitions(starflighttlc PRIVATE
  ALLEGRO_LEGACY_NO_FIX_ALIASES
)

# Include directories - order matters!
# Match the working order from tests/CMakeLists.txt:
# 1. Source include (allegro.h)
# 2. Build include (generated platform headers)
# 3. Homebrew Allegro 5
target_include_directories(starflighttlc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/tinyxml"
  ${TLC_LUA_INCLUDE_DIRS}
)

# Allegro Legacy headers come from the AllegroLegacy::allegrolegacy target.
# Do not add them manually here (it can break libc++ header lookup on macOS
# by pulling in /opt/homebrew/opt/allegro/include too early).

# libnoise headers
# Add the parent include dir so code can use <noise/noise.h>.
# Keep it after source includes to avoid shadowing project headers.
target_include_directories(starflighttlc PRIVATE
  "${CMAKE_SOURCE_DIR}/deps/libnoise/include"
)

# Link directories
# (pkg-config may provide -L flags needed for Lua on macOS/Homebrew)
if(TLC_LUA_LIBRARY_DIRS)
  target_link_directories(starflighttlc PRIVATE ${TLC_LUA_LIBRARY_DIRS})
endif()

# Platform-specific Allegro linking
if(WIN32)
  # Windows: Link Allegro 5 libraries (from vcpkg or manual install)
  find_package(allegro CONFIG QUIET)
  if(allegro_FOUND)
    target_link_libraries(starflighttlc PRIVATE
      allegro::allegro
      allegro::allegro_font
      allegro::allegro_ttf
      allegro::allegro_primitives
      allegro::allegro_audio
      allegro::allegro_acodec
      allegro::allegro_main
    )
  else()
    # Fallback to library names
    target_link_libraries(starflighttlc PRIVATE
      allegro
      allegro_font
      allegro_ttf
      allegro_primitives
      allegro_audio
      allegro_acodec
      allegro_main
    )
  endif()
  # Windows OpenGL
  target_link_libraries(starflighttlc PRIVATE opengl32)
elseif(APPLE)
  # macOS: Homebrew Allegro
  # Add Allegro 5 include directory (needed by Allegro Legacy headers)
  target_include_directories(starflighttlc PRIVATE
    "/opt/homebrew/opt/allegro/include"
  )
  target_link_directories(starflighttlc PRIVATE
    "/opt/homebrew/opt/allegro/lib"
  )
  target_link_libraries(starflighttlc PRIVATE
    allegro
    allegro_font
    allegro_ttf
    allegro_primitives
    allegro_audio
    allegro_acodec
    allegro_main
    "-framework OpenGL"
  )
  # RPATH so executable finds dylibs at runtime
  set_target_properties(starflighttlc PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/deps/allegro-legacy/build/lib;/opt/homebrew/opt/allegro/lib"
    INSTALL_RPATH "@executable_path/../Frameworks"
  )
else()
  # Linux
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(ALLEGRO5 REQUIRED 
    allegro-5
    allegro_font-5
    allegro_ttf-5
    allegro_primitives-5
    allegro_audio-5
    allegro_acodec-5
    allegro_main-5
  )
  target_include_directories(starflighttlc PRIVATE ${ALLEGRO5_INCLUDE_DIRS})
  target_link_directories(starflighttlc PRIVATE ${ALLEGRO5_LIBRARY_DIRS})
  target_link_libraries(starflighttlc PRIVATE ${ALLEGRO5_LIBRARIES})
  # Linux OpenGL (need GLU for Model.cpp)
  find_package(OpenGL REQUIRED)
  target_link_libraries(starflighttlc PRIVATE OpenGL::GL OpenGL::GLU)
  # RPATH for Linux
  set_target_properties(starflighttlc PROPERTIES
    BUILD_RPATH "${CMAKE_SOURCE_DIR}/deps/allegro-legacy/build/lib"
    INSTALL_RPATH "$ORIGIN/lib"
  )
endif()

# Link common libraries (all platforms)
target_link_libraries(starflighttlc PRIVATE
  AllegroLegacy::allegrolegacy
  ${TLC_LUA_LIBRARIES}
  libnoise
)
