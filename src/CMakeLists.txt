# src/CMakeLists.txt


# Gather files

# fileIO
set(FILEIO_MODULES
  fileIO/text.cpp
  fileIO/text.h
  fileIO/json.cpp
  fileIO/json.h
)

# parser
set(PARSER_MODULES
  parser/JSON_parser.h
  parser/chars/general.cpp
  parser/chars/general.h
)

# renderer
set(RENDERER_MODULES
  renderer/gl_loader.h
  renderer/renderer.cpp
  renderer/renderer.h
  renderer/vertex_array.cpp
  renderer/vertex_array.h
  renderer/vertex_buffer.cpp
  renderer/vertex_buffer.h
  renderer/index_buffer.cpp
  renderer/index_buffer.h
  renderer/shader.cpp
  renderer/shader.h
  renderer/texture2D.cpp
  renderer/texture2D.h
  renderer/renderer_base_api.h
)

# util
set(UTIL_MODULES
  util/fileIO.cpp
  util/fileIO.h
  util/rng.cpp
  util/rng.h
)

# Atlas
set(ATLAS_FILES
  Atlas/image.cpp
  Atlas/image.h
  Atlas/main.cpp
)

# CharTool
set(CHARTOOL_PAGE_FILES
  CharTool/page/page_base.h
  CharTool/page/general.cpp
  CharTool/page/general.h
)

set(CHARTOOL_FILES
  CharTool/main.cpp
  CharTool/app.cpp
  CharTool/app.h
  CharTool/frame.cpp
  CharTool/frame.h
  ${CHARTOOL_PAGE_FILES}
)

# Compiler
set(COMPILER_FILES
  Compiler/main.cpp
)

# DvD
set(DVD_MENU_FILES
  DvD/menu/menu_interface.cpp
  DvD/menu/menu_interface.h
  DvD/menu/submenu.cpp
  DvD/menu/submenu.h
  DvD/menu/button.cpp
  DvD/menu/button.h
  DvD/menu/selector.h
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
  DvD/scene/controls.cpp
  DvD/scene/controls.h
)

set(DVD_SHADER_RENDERER_FILES
  DvD/shader_renderer/fighter_renderer.cpp
  DvD/shader_renderer/fighter_renderer.h
  DvD/shader_renderer/primitive_renderer.cpp
  DvD/shader_renderer/primitive_renderer.h
  DvD/shader_renderer/texture2D_renderer.cpp
  DvD/shader_renderer/texture2D_renderer.h
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
  DvD/resource_manager.cpp
  DvD/resource_manager.h
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
  ${DVD_SHADER_RENDERER_FILES}
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

# UBU2JSON
set(UBU2JSON_FILES
  UBU2JSON/main.cpp
  UBU2JSON/ubu.cpp
  UBU2JSON/ubu.h
  UBU2JSON/chars.cpp
  UBU2JSON/chars.h
  UBU2JSON/scenes.cpp
  UBU2JSON/scenes.h
  UBU2JSON/stages.cpp
  UBU2JSON/stages.h
)


# Add executables
# Platform specific flags
set(GUI_EXECUTABLE_FLAG)
if(WIN32)
  set(GUI_EXECUTABLE_FLAG WIN32)
endif()

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

add_executable(CharTool
  ${GUI_EXECUTABLE_FLAG}
  ${CHARTOOL_FILES}
  ${FILEIO_MODULES}
  ${PARSER_MODULES}
)
target_compile_definitions(CharTool
  PRIVATE WXUSINGDLL
  PRIVATE DVD_VERSION_STRING="${DreamVsDream_VERSION}"
)

add_executable(Compiler
  ${COMPILER_FILES}
  DvD/error.cpp
  DvD/fighter.cpp
  DvD/file.cpp
  DvD/parser.cpp
  DvD/sprite.cpp
  fileIO/text.cpp
  util/fileIO.cpp
)
target_compile_definitions(Compiler
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE COMPILER
  PRIVATE NO_ZLIB
)

add_executable(DvD
  ${GUI_EXECUTABLE_FLAG}
  ${DVD_FILES}
  ${FILEIO_MODULES}
  ${RENDERER_MODULES}
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
  ${GUI_EXECUTABLE_FLAG}
  ${SPRTOOL_FILES}
  ${RENDERER_MODULES}
  DvD/error.cpp
  DvD/fighter.cpp
  DvD/graphics.cpp
  DvD/image.cpp
  DvD/parser.cpp
  DvD/sprite.cpp
  DvD/sys.cpp
  ${DVD_SHADER_RENDERER_FILES}
  fileIO/text.cpp
  util/fileIO.cpp
)
target_compile_definitions(Sprtool
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE SPRTOOL
  PRIVATE NO_ZLIB
)

add_executable(UBU2JSON
  ${UBU2JSON_FILES}
  ${FILEIO_MODULES}
  DvD/error.cpp
  DvD/parser.cpp
  util/fileIO.cpp
)
target_compile_definitions(UBU2JSON
  PRIVATE VERSION="${DreamVsDream_VERSION}"
  PRIVATE COMPILER
  PRIVATE NO_ZLIB
)


# Specific linking
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
  target_link_libraries(UBU2JSON
    PRIVATE shlwapi
  )
endif()
if(MINGW)
  target_link_libraries(DvD
    PRIVATE mingw32
  )
  target_link_libraries(Sprtool
    PRIVATE mingw32
  )
endif()

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

target_include_directories(CharTool
  PRIVATE nlohmann_json
  PRIVATE wxbase
  PRIVATE wxcore
  PRIVATE wxpropgrid
)
target_link_libraries(CharTool
  PRIVATE nlohmann_json
  PRIVATE wxbase
  PRIVATE wxcore
  PRIVATE wxpropgrid
)

target_include_directories(Compiler
  PRIVATE glm
)
target_link_libraries(Compiler
  PRIVATE glm
)

target_include_directories(DvD
  PRIVATE SDL2main
  PRIVATE SDL2
  PRIVATE glad
  PRIVATE glm
  PRIVATE ${zlib_LIBRARY}
  PRIVATE ${libpng_INCLUDE_DIRS}
  PRIVATE stb_image
  PRIVATE sndfile
  PRIVATE nlohmann_json
)
target_link_libraries(DvD
  PRIVATE SDL2main
  PRIVATE SDL2
  PRIVATE glad
  PRIVATE glm
  PRIVATE ${zlib_LIBRARY}
  PRIVATE ${libpng_LIBRARY}
  PRIVATE stb_image
  PRIVATE sndfile
  PRIVATE nlohmann_json
)

# target_include_directories(Fontbuild
#   ...
# )
# target_link_libraries(Fontbuild
#   ...
# )

target_include_directories(Sprtool
  PRIVATE SDL2
  PRIVATE SDL2main
  PRIVATE glad
  PRIVATE glm
  PRIVATE ${libpng_INCLUDE_DIRS}
)
target_link_libraries(Sprtool
  PRIVATE SDL2
  PRIVATE SDL2main
  PRIVATE glad
  PRIVATE glm
  PRIVATE ${libpng_LIBRARY}
)

target_include_directories(UBU2JSON
  PRIVATE nlohmann_json
)
target_link_libraries(UBU2JSON
  PRIVATE nlohmann_json
)


# Organize files for IDEs
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/fileIO" PREFIX "fileIO" FILES ${FILEIO_MODULES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/parser" PREFIX "parser" FILES ${PARSER_MODULES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/renderer" PREFIX "renderer" FILES ${RENDERER_MODULES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/util" PREFIX "util" FILES ${UTIL_MODULES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Atlas" PREFIX "Atlas" FILES ${ATLAS_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/CharTool" PREFIX "CharTool" FILES ${CHARTOOL_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/CharTool/page" PREFIX "CharTool/page" FILES ${CHARTOOL_PAGE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Compiler" PREFIX "Compiler" FILES ${COMPILER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD" PREFIX "DvD" FILES ${DVD_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD/menu" PREFIX "DvD/menu" FILES ${DVD_MENU_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD/scene" PREFIX "DvD/scene" FILES ${DVD_SCENE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/DvD/shader_renderer" PREFIX "DvD/shaderer_renderer" FILES ${DVD_SHADER_RENDERER_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Fontbuild" PREFIX "Fontbuild" FILES ${FONTBUILD_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/Sprtool" PREFIX "Sprtool" FILES ${SPRTOOL_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/UBU2JSON" PREFIX "UBU2JSON" FILES ${UBU2JSON_FILES})
