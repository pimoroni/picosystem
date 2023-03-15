include_directories(${CMAKE_CURRENT_LIST_DIR}/../../)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../")

# Enable support for string_view
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(picosystem/micropython)
