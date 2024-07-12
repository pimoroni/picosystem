include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

# Enable support for string_view
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(picosystem/micropython)
include(cppmem/micropython)

# Do not include stack unwinding & exception handling for C++ user modules
target_compile_definitions(usermod INTERFACE PICO_CXX_ENABLE_EXCEPTIONS=0)
target_compile_options(usermod INTERFACE $<$<COMPILE_LANGUAGE:CXX>:
    -fno-exceptions
    -fno-unwind-tables
    -fno-rtti
    -fno-use-cxa-atexit
>)
target_link_options(usermod INTERFACE -specs=nano.specs)
