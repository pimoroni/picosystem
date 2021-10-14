if(NOT PICOSYSTEM_DIR)
  list(APPEND PICOSYSTEM_SDK_SEARCH_PATHS
  ${CMAKE_CURRENT_LIST_DIR}/../picosystem
    ${CMAKE_CURRENT_LIST_DIR}/../
  )
  foreach(SEARCH_PATH ${PICOSYSTEM_SDK_SEARCH_PATHS})
    get_filename_component(SEARCH_PATH ${SEARCH_PATH} ABSOLUTE)
    if(EXISTS ${SEARCH_PATH}/picosystem-config.cmake)
      message("PICOSYSTEM_DIR is ${SEARCH_PATH}")
      set(PICOSYSTEM_DIR ${SEARCH_PATH})
      break()
    endif()
  endforeach()
endif()
find_package(PICOSYSTEM CONFIG REQUIRED)