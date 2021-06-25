# extern/CMakeLists.txt


# Intended properties
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries." FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build tests." FORCE)

# Function to set BUILD_SHARED_LIBS for libraries
function(conditional_set_BUILD_SHARED_LIBS lib)
  if(lib IN_LIST SHARED_LIBRARIES)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries." FORCE)
  else()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries." FORCE)
  endif()
endfunction()



# External libraries

# SDL
message(STATUS "Library: SDL")
conditional_set_BUILD_SHARED_LIBS(SDL2)
add_subdirectory("SDL" EXCLUDE_FROM_ALL)

if(MINGW)
  target_link_libraries(SDL2
    PRIVATE iconv
  )
endif()


# glad
message(STATUS "Library: glad")
conditional_set_BUILD_SHARED_LIBS(glad)
set(GLAD_REPRODUCIBLE ON CACHE BOOL "glad: Reproducible build" FORCE)
add_subdirectory("glad" EXCLUDE_FROM_ALL)


# glm
message(STATUS "Library: glm")
set(GLM_TEST_ENABLE OFF CACHE BOOL "glm: Build unit tests" FORCE)
add_subdirectory("glm" EXCLUDE_FROM_ALL)


# zlib
message(STATUS "Library: zlib")
conditional_set_BUILD_SHARED_LIBS(zlib)
add_subdirectory("zlib" EXCLUDE_FROM_ALL)

get_target_property(ZLIB_INCLUDE_DIR zlib INCLUDE_DIRECTORIES)

# Install export zlib targets because libpng is exported as well
install(TARGETS zlib zlibstatic
  EXPORT zlib_targets
)
install(EXPORT zlib_targets
  DESTINATION "lib/zlib"
)


# libpng
message(STATUS "Library: libpng")
conditional_set_BUILD_SHARED_LIBS(png)
if(png IN_LIST SHARED_LIBRARIES)
  set(ZLIB_LIBRARY zlib)
  set(PNG_SHARED ON CACHE BOOL "libpng: Build shared lib" FORCE)
  set(PNG_STATIC OFF CACHE BOOL "libpng: Build static lib" FORCE)
else()
  set(ZLIB_LIBRARY zlibstatic)
  set(PNG_SHARED OFF CACHE BOOL "libpng: Build shared lib" FORCE)
  set(PNG_STATIC ON CACHE BOOL "libpng: Build static lib" FORCE)
endif()
set(PNG_BUILD_ZLIB ON CACHE BOOL "libpng: Custom zlib Location, else find_package is used" FORCE)
set(PNG_TESTS OFF CACHE BOOL "libpng: Build libpng tests" FORCE)
add_subdirectory("libpng" EXCLUDE_FROM_ALL)


# stb_image
message(STATUS "Library: stb_image")
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb_image")


# ogg
message(STATUS "Library: ogg")
conditional_set_BUILD_SHARED_LIBS(ogg)
add_subdirectory("ogg" EXCLUDE_FROM_ALL)

set(OGG_LIBRARY ogg)
get_target_property(OGG_INCLUDE_DIR ogg INCLUDE_DIRECTORIES)
add_library(Ogg::ogg ALIAS ogg)


# vorbis
message(STATUS "Library: vorbis")
conditional_set_BUILD_SHARED_LIBS(vorbis)
add_subdirectory("vorbis" EXCLUDE_FROM_ALL)

set(Vorbis_Vorbis_LIBRARY vorbis)
get_target_property(Vorbis_Vorbis_INCLUDE_DIR vorbis INCLUDE_DIRECTORIES)
add_library(Vorbis::vorbisenc ALIAS vorbisenc)


# flac
message(STATUS "Library: flac")
conditional_set_BUILD_SHARED_LIBS(FLAC)
set(BUILD_EXAMPLES OFF CACHE BOOL "flac: Build and install examples" FORCE)
add_subdirectory("flac" EXCLUDE_FROM_ALL)

set(FLAC_LIBRARY FLAC)
get_target_property(FLAC_INCLUDE_DIR FLAC INCLUDE_DIRECTORIES)
add_library(FLAC::FLAC ALIAS FLAC)


# opus
message(STATUS "Library: opus")
conditional_set_BUILD_SHARED_LIBS(opus)
if(opus IN_LIST SHARED_LIBRARIES)
  set(OPUS_BUILD_SHARED_LIBRARY ON CACHE BOOL "opus: build shared library." FORCE)
else()
  set(OPUS_BUILD_SHARED_LIBRARY OFF CACHE BOOL "opus: build shared library." FORCE)
endif()
set(OPUS_BUILD_PROGRAMS OFF CACHE BOOL "opus: Build programs" FORCE)
if(MINGW)
  set(OPUS_STACK_PROTECTOR OFF CACHE BOOL "opus: Use stack protection" FORCE)
endif()
add_subdirectory("opus" EXCLUDE_FROM_ALL)

set(OPUS_LIBRARY opus)
get_target_property(OPUS_INCLUDE_DIR opus INCLUDE_DIRECTORIES)
add_library(Opus::opus ALIAS opus)


# libsndfile
# M: libsndfile/cmake/SndFileChecks.cmake
# M: libsndfile/src/ogg_opus.c
message(STATUS "Library: libsndfile")
conditional_set_BUILD_SHARED_LIBS(sndfile)
set(BUILD_PROGRAMS OFF CACHE BOOL "libsndfile: Build programs" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "libsndfile: Build examples" FORCE)
add_subdirectory("libsndfile" EXCLUDE_FROM_ALL)


# json
message(STATUS "Library: json")
set(JSON_BuildTests OFF CACHE BOOL "json: Build the unit tests when BUILD_TESTING is enabled." FORCE)
set(JSON_Install OFF CACHE BOOL "json: Install CMake targets during install step." FORCE)
add_subdirectory("json" EXCLUDE_FROM_ALL)


# wxWidgets
message(STATUS "Library: wxWidgets")
conditional_set_BUILD_SHARED_LIBS(wxbase)
add_subdirectory("wxWidgets" EXCLUDE_FROM_ALL)
