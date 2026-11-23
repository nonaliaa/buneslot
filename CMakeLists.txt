cmake_minimum_required(VERSION 4.4)

project(buneslots LANGUAGES C)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "Building within the source tree is unsupported")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(common_options -Wall -Wextra)
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  list(APPEND common_options -ggdb -fsanitize=address)
endif()

add_compile_options(${common_options})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

add_subdirectory(src)