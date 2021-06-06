# src/CMakeLists.txt


# Gather files

# util
set(UTIL_MODULES
  util/fileIO.cpp
  util/fileIO.h
  util/rng.cpp
  util/rng.h
  util/vec2.cpp
  util/vec2.h
)

# Atlas
set(ATLAS_FILES
  Atlas/image.cpp
  Atlas/image.h
  Atlas/main.cpp
)

# Compiler
set(COMPILER_FILES
  Compiler/main.cpp
)

# DvD
set(DVD_MENU_FILES
  DvD/menu/menu_interface.h
  DvD/menu/submenu.cpp
  DvD/menu/submenu.h
  DvD/menu/button.cpp
  DvD/menu/button.h
)
set(DVD_SCENE_FILES
  DvD/scene/scene.cpp
  DvD/scene/scene.h
  DvD/scene/scene_base.cpp
  DvD/scene/scene_base.h
  DvD/scene/intro.cpp
  DvD/scene/intro.h
  DvD/scene/title.cpp
  DvD/scene/title.h
  DvD/scene/select.cpp
  DvD/scene/select.h
  DvD/scene/versus.cpp
  DvD/scene/versus.h
  DvD/scene/options.cpp
  DvD/scene/options.h
  DvD/scene/fight.cpp
  DvD/scene/fight.h
  DvD/scene/netplay.cpp
  DvD/scene/netplay.h
  DvD/scene/credits.cpp
  DvD/scene/credits.h
)

set(DVD_FILES
  DvD/animation.cpp
  DvD/animation.h
  DvD/app.cpp
  DvD/app.h
  DvD/atlas.cpp
  DvD/atlas.h
  DvD/effect.cpp
  DvD/effect.h
  DvD/error.cpp
  DvD/error.h
  DvD/fighter.cpp
  DvD/fighter.h
  DvD/file.cpp
  DvD/file.h
  DvD/font.cpp
  DvD/font.h
  DvD/graphics.cpp
  DvD/graphics.h
  DvD/image.cpp
  DvD/image.h
  DvD/input.cpp
  DvD/input.h
  DvD/main.cpp
  DvD/network.cpp
  DvD/network.h
  DvD/parser.cpp
  DvD/parser.h
  DvD/player.cpp
  DvD/player.h
  DvD/shader.cpp
  DvD/shader.h
  DvD/sound.cpp
  DvD/sound.h
  DvD/sprite.cpp
  DvD/sprite.h
  DvD/stage.cpp
  DvD/stage.h
  DvD/sys.cpp
  DvD/sys.h
  DvD/thread.cpp
  DvD/thread.h
  ${DVD_MENU_FILES}
  ${DVD_SCENE_FILES}
)

# Fontbuild
set(FONTBUILD_FILES
  Fontbuild/main.c
)

# Sprtool
set(SPRTOOL_FILES
  Sprtool/init.cpp
  Sprtool/input.cpp
  Sprtool/main.cpp
)


# Add executables

add_executable(Atlas
  ${ATLAS_FILES}
  DvD/file.cpp
  util/fileIO.cpp
)
target_compile_definitions(Atlas
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE COMPILER
  PRIVATE NO_ZLIB
)

add_executable(Compiler
  ${COMPILER_FILES}
  DvD/error.cpp
  DvD/fighter.cpp
  DvD/file.cpp
  DvD/parser.cpp
  DvD/sprite.cpp
  util/fileIO.cpp
  util/vec2.cpp
)
target_compile_definitions(Compiler
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE COMPILER
  PRIVATE NO_ZLIB
)

add_executable(DvD
  ${DVD_FILES}
  ${UTIL_MODULES}
)
target_compile_definitions(DvD
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE GAME
)

add_executable(Fontbuild
  ${FONTBUILD_FILES}
)
target_compile_definitions(Fontbuild
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE COMPILER
  PRIVATE NO_ZLIB
)

add_executable(Sprtool
  ${SPRTOOL_FILES}
  DvD/error.cpp
  DvD/fighter.cpp
  DvD/graphics.cpp
  DvD/image.cpp
  DvD/parser.cpp
  DvD/sprite.cpp
  DvD/sys.cpp
  util/fileIO.cpp
  util/vec2.cpp
)
target_compile_definitions(Sprtool
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE SPRTOOL
  PRIVATE NO_ZLIB
)


# Target includes and links with dependencies

if(zlib IN_LIST SHARED_LIBRARIES)
  set(zlib_LIBRARY zlib)
else()
  set(zlib_LIBRARY zlibstatic)
endif()

# png does not work normally for some reason
if(png IN_LIST SHARED_LIBRARIES)
  set(libpng_LIBRARY png)
else()
  set(libpng_LIBRARY png_static)
endif()
get_target_property(libpng_INCLUDE_DIRS ${libpng_LIBRARY} INCLUDE_DIRECTORIES)


target_include_directories(Atlas
  PRIVATE ${libpng_INCLUDE_DIRS}
)
target_link_libraries(Atlas
  PRIVATE ${libpng_LIBRARY}
)

# target_include_directories(Compiler
  # ...
# )
# target_link_libraries(Compiler
  # ...
# )

target_include_directories(DvD
  PRIVATE SDL2
  PRIVATE glad
  PRIVATE ${zlib_LIBRARY}
  PRIVATE ${libpng_INCLUDE_DIRS}
  PRIVATE stb_image
  PRIVATE sndfile
)
target_link_libraries(DvD
  PRIVATE SDL2
  PRIVATE glad
  PRIVATE ${zlib_LIBRARY}
  PRIVATE ${libpng_LIBRARY}
  PRIVATE stb_image
  PRIVATE sndfile
)

# target_include_directories(Fontbuild
  # ...
# )
# target_link_libraries(Fontbuild
  # ...
# )

target_include_directories(Sprtool
  PRIVATE SDL2
  PRIVATE glad
  PRIVATE ${libpng_INCLUDE_DIRS}
)
target_link_libraries(Sprtool
  PRIVATE SDL2
  PRIVATE glad
  PRIVATE ${libpng_LIBRARY}
)

# Platform specific linking
if(WIN32)
  target_link_libraries(Atlas
    PRIVATE shlwapi
  )
  target_link_libraries(Compiler
    PRIVATE shlwapi
  )
  target_link_libraries(DvD
    PRIVATE ws2_32
    PRIVATE shlwapi
  )
  target_link_libraries(Sprtool
    PRIVATE shlwapi
  )
endif()


# Organize files for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/util" PREFIX "util" FILES ${UTIL_MODULE})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Atlas" PREFIX "Atlas" FILES ${ATLAS_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Compiler" PREFIX "Compiler" FILES ${COMPILER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD" PREFIX "DvD" FILES ${DVD_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD/menu" PREFIX "DvD/menu" FILES ${DVD_MENU_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD/scene" PREFIX "DvD/scene" FILES ${DVD_SCENE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Fontbuild" PREFIX "Fontbuild" FILES ${FONTBUILD_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Sprtool" PREFIX "Sprtool" FILES ${SPRTOOL_FILES})
