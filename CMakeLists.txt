cmake_minimum_required(VERSION 4.4)

if ("${BUNESLOTS_VERSION}" STREQUAL "")
  message(WARNING "Version information not found, defaulting to \"2026.11.0\"")
  set(BUNESLOTS_VERSION "2026.11.0")
endif()

project(buneslots
  LANGUAGES C
  VERSION "${BUNESLOTS_VERSION}"
)

option(BUNESLOTS_WITH_ASAN "Compile with AddressSanitizer" OFF)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "Building within the source tree is unsupported")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(common_options -Wall -Wextra)
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  list(APPEND common_options -ggdb)
endif()

if (BUNESLOTS_ENABLE_ASAN)
  list(APPEND common_options -fsanitize=address)
endif()

add_compile_options(${common_options})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_subdirectory(src)
