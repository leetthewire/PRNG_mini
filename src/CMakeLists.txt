# Collect header and source files
file(GLOB_RECURSE MAIN_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE MAIN_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
file(GLOB_RECURSE INCLUDE_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h)

# Exclude main.c (e.g., test runners or CLI entry point)
list(FILTER MAIN_SOURCES EXCLUDE REGEX ".*main.c")

# Organize into Visual Studio groups (optional for IDEs)
foreach(source_file ${MAIN_SOURCES})
    get_filename_component(dir ${source_file} DIRECTORY)
    file(RELATIVE_PATH group ${CMAKE_CURRENT_SOURCE_DIR} ${dir})
    source_group("Source Files\\${group}" FILES ${source_file})
endforeach()

foreach(header_file ${MAIN_HEADERS})
    get_filename_component(dir ${header_file} DIRECTORY)
    file(RELATIVE_PATH group ${CMAKE_CURRENT_SOURCE_DIR} ${dir})
    source_group("Header Files\\${group}" FILES ${header_file})
endforeach()

foreach(include_file ${INCLUDE_HEADERS})
    get_filename_component(dir ${include_file} DIRECTORY)
    file(RELATIVE_PATH group ${CMAKE_CURRENT_SOURCE_DIR} ${dir})
    string(REPLACE ".." "" clean_group ${group})
    source_group("Header Files${clean_group}" FILES ${include_file})
endforeach()

# Determine library type (shared/static)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
if (SHARED_LIBRARY)
    add_library(PRNG_mini SHARED ${MAIN_HEADERS} ${MAIN_SOURCES} ${INCLUDE_HEADERS})
    target_precompile_headers(PRNG_mini PRIVATE ${INCLUDE_HEADERS})
else()
    add_library(PRNG_mini STATIC ${MAIN_HEADERS} ${MAIN_SOURCES} ${INCLUDE_HEADERS})
endif()

# Include paths
target_include_directories(PRNG_mini PRIVATE
    ../include/
)

# Link directories
target_link_directories(PRNG_mini PRIVATE
    ../build/outputs/
)

# Unix-specific linker fix
if (UNIX AND NOT APPLE)
    target_link_options(PRNG_mini PRIVATE "-Wl,--allow-multiple-definition")
endif()

# Set target properties
set_target_properties(PRNG_mini PROPERTIES
    PUBLIC_HEADER ../include/PRNG_mini.h
    LINKER_LANGUAGE C
)

# Compiler flags for Debug vs Release
if(WIN32)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    string(REPLACE "/O2" "/Od" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
    string(REPLACE "/Ob2" "/Ob0" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
else()
    string(REPLACE "-O2" "-Od" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
endif()

# Add install rules for iOS builds
if(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    install(TARGETS PRNG_mini
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

# Enable PIC (for Unix-like)
if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set_property(TARGET PRNG_mini PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
