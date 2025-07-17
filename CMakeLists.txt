# Main entry point for the CMake configuration
cmake_minimum_required(VERSION 3.18)

project(
  PRNG_mini
  VERSION 0.1
  DESCRIPTION "C-based pseudo-random generator"
  LANGUAGES C)

if (MSVC)
    # Debug: Static runtime with debug info
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug" CACHE STRING "" FORCE)

    # Detect if we're building Release instead
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "" FORCE)
    endif()
endif()

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

# Add test directories if TESTS is enabled
if (TESTS)
    file(GLOB_RECURSE TESTS_FOLDERS
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/**/CMakeLists.txt)

    foreach(TESTS_FOLDER ${TESTS_FOLDERS})
        get_filename_component(relative_file_path ${TESTS_FOLDER} DIRECTORY)

        # Add each test subdirectory
        add_subdirectory(${relative_file_path})
    endforeach()
endif()