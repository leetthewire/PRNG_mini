if (MSVC)
    # Debug: Static runtime with debug info
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug" CACHE STRING "" FORCE)

    # Detect if we're building Release instead
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "" FORCE)
    endif()
endif()

add_executable(license_key main.c)

set_property(TARGET license_key PROPERTY C_STANDARD 11)

target_include_directories(license_key PRIVATE ../../include/)

target_link_directories(license_key PRIVATE ../../build/_build/)

target_link_libraries(license_key PRIVATE PRNG_mini)
