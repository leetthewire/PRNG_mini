if (MSVC)
    # Debug: Static runtime with debug info
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug" CACHE STRING "" FORCE)

    # Detect if we're building Release instead
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "" FORCE)
    endif()
endif()

add_executable(randomness main.c)

set_property(TARGET randomness PROPERTY C_STANDARD 11)

target_include_directories(randomness PRIVATE ../../include/)

target_link_directories(randomness PRIVATE ../../build/_build/)

target_link_libraries(randomness PRIVATE PRNG_mini)
