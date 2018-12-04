cmake_minimum_required(VERSION 3.1)

project(dobble LANGUAGES C)

# Display Makefile commands
set(CMAKE_VERBOSE_MAKEFILE ON)

# Debug by default
set(CMAKE_BUILD_TYPE Debug)

# Require -std=gnu11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find SDL2 packages
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Enable -Wall -Werror
set(CMAKE_C_FLAGS "-Wall -Werror ${CMAKE_C_FLAGS}")

# Full path to data directory
get_filename_component(DATA_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data" REALPATH)

# Convert to C string
function(convert_to_cstring_literal var value)
  string(REGEX REPLACE "([\\\$\"])" "\\\\\\1" escaped "${value}")
  set("${var}" "\"${escaped}\"" PARENT_SCOPE)
endfunction()

# Create header file
configure_file(header/dobble-config.h.in ${CMAKE_BINARY_DIR}/dobble-config.h
  ESCAPE_QUOTES)

# Relative path to build directory
file(RELATIVE_PATH BUILD_RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

if (DOBBLE_CORRECTION)
  add_definitions(-DCORRECTION=1)
endif()

# List of header files
set(header
  ${CMAKE_BINARY_DIR}/dobble-config.h
  header/dobble.h
  header/graphics.h)

# List of source files
set(sources
  src/graphics.c
  src/dobble.c)

# List of include directorie
include_directories(
  ${CMAKE_SOURCE_DIR}/header
  ${CMAKE_BINARY_DIR}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${SDL2_TTF_INCLUDE_DIR})

# Create executable
add_executable(${PROJECT_NAME} ${header} ${sources})

# Libraries
target_link_libraries(
  ${PROJECT_NAME}
  m
  ${SDL2_LIBRARY}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_TTF_LIBRARIES})

# Archive maker
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_VERSION_MAJOR 2017)
set(CPACK_PACKAGE_VERSION_MINOR 12)
set(CPACK_PACKAGE_VERSION_PATCH 0)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CMAKE_PROJECT_NAME}-rendu")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "\\\\.o$"
  "\\\\.odt$"
  "\\\\.swp$"
  "${CMAKE_BINARY_DIR}/"
  "/data/OPEN_CLIP_ART/"
  "${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
