# Main entry point for the CMake configuration
cmake_minimum_required(VERSION 3.18)

project(
  PRNG_mini
  VERSION 0.1
  DESCRIPTION "C-based pseudo-random generator"
  LANGUAGES C)

set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_output/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_output/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_output/)

# Configure settings for iOS builds
if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(CMAKE_SYSTEM_PROCESSOR "x86_64" CACHE STRING "" FORCE)
    set(CMAKE_SYSTEM_NAME "iOS" CACHE STRING "" FORCE)
endif()

# Set compiler flags
if(MSVC)
    # Compiler flags for MSVC
    add_compile_options(/nologo /W4 /wd4146 /wd4244)
else()
    # Compiler flags for GCC/Clang
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-Wmissing-prototypes -Wredundant-decls -Wshadow -Wpointer-arith)
    add_compile_options(-O3 -fomit-frame-pointer)
    add_definitions(-w)
endif()

add_subdirectory(src)