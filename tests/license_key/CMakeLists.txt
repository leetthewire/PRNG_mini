add_executable(license_key main.c)

set_property(TARGET license_key PROPERTY C_STANDARD 11)

target_include_directories(license_key PRIVATE ../../include/)

target_link_directories(license_key PRIVATE ../../build/_build/)

target_link_libraries(license_key PRIVATE PRNG_mini)
