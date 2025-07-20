if (MSVC)
    # Debug: Static runtime with debug info
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug" CACHE STRING "" FORCE)

    # Detect if we're building Release instead
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "" FORCE)
    endif()
endif()

add_executable(general_test main.c)

set_property(TARGET general_test PROPERTY C_STANDARD 11)

target_include_directories(general_test PRIVATE ../../include/)

target_link_directories(general_test PRIVATE ../../build/_build/)

target_link_libraries(general_test PRIVATE PRNG_mini)
