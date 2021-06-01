# CMakeLists.txt

cmake_minimum_required(VERSION 3.14.0)


project("DreamVsDream"
  VERSION 0.2.1
)


# Set CMAKE_BUILD_TYPE to DEBUG if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Set build type to DEBUG as none was specified.")
  set(CMAKE_BUILD_TYPE DEBUG CACHE STRING "Build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "DEBUG"
    "RELEASE"
    "RELWITHDEBINFO"
    "MINSIZEREL"
  )
endif()
if(CMAKE_CONFIGURATION_TYPES)
  message(STATUS "CMAKE_CONFIGURATION_TYPES=${CMAKE_CONFIGURATION_TYPES}")
else()
  message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()


# First, mark all external libraries as shared
set(SHARED_LIBRARIES
  SDL2
  zlib
  png
  ogg
  vorbis
  vorbisenc
  FLAC
  opus
  sndfile
)
# Then, unmark external libraries that cause problems if shared
if(MINGW)
  list(REMOVE_ITEM SHARED_LIBRARIES
    zlib
    png
    vorbis
    vorbisenc
  )
endif()

message(STATUS "Shared libraries: ${SHARED_LIBRARIES}")

# Add external libraries from extern
add_subdirectory("extern" EXCLUDE_FROM_ALL)


# Add executables from src
add_subdirectory("src")

# Set DREAMVSDREAM_BINARIES
set(DREAMVSDREAM_BINARIES
  Atlas
  Compiler
  DvD
  Fontbuild
  Sprtool
)
message(STATUS "Dream Vs. Dream binaries: ${DREAMVSDREAM_BINARIES}")


# Set binary build directory inside bin
set(BINARY_BUILD_TARGETS
  ${DREAMVSDREAM_BINARIES}
  ${SHARED_LIBRARIES}
)

set_target_properties(${BINARY_BUILD_TARGETS}
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
