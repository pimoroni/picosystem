function (pico_add_extra_outputs)
endfunction()

add_library(picosystem
    ${CMAKE_CURRENT_LIST_DIR}/hardware.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/picosystem.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/blend.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/state.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/primitives.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/utility.cpp
    ${PICOSYSTEM_LIBRARY_PATH}/assets.cpp
)

if(EMSCRIPTEN)
	target_compile_options(picosystem
		PRIVATE
			"-s;USE_SDL=2;"
	)

	set(SDL2_LIBRARIES "-s USE_SDL=2")

	set(EMSCRIPTEN_SHELL ${CMAKE_CURRENT_SOURCE_DIR}/emscripten-shell.html PARENT_SCOPE)
else()
    # attempt to find the framework (find_package won't work as there's no config file)
    if(APPLE)
        find_library(SDL2_LIBRARIES NAMES SDL2)

        find_path(SDL2_INCLUDE_DIRS NAMES SDL.h
            PATHS ~/Library/Frameworks /Library/Frameworks
        )
    endif()

    # fallback guess for SDL location on Windows
    if(WIN32 AND NOT SDL2_DIR AND 32BLIT_DIR)
        set(SDL2_DIR "${32BLIT_DIR}/vs/sdl")
    endif()

    if(ANDROID AND NOT SDL2_DIR)
        set(SDL2_DIR "${ANDROID_LIB_DIR}/${ANDROID_ABI}/lib/cmake/SDL2")
        set(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH}" "${ANDROID_LIB_DIR}/${ANDROID_ABI}") # won't search outside this
    endif()

    if(NOT SDL2_LIBRARIES OR NOT SDL2_INCLUDE_DIRS)
        find_package(SDL2 REQUIRED)
    endif()

    # If SDL2 was built using CMake, the generated configuration files define SDL2::* targets instead of the SDL2_* variables
    if(TARGET SDL2::SDL2)
        set(SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2)
    # handle SDL2 built with only a static library
    elseif(TARGET SDL2::SDL2-static)
        set(SDL2_LIBRARIES SDL2::SDL2main SDL2::SDL2-static)
    else()
        target_include_directories(picosystem
            PRIVATE	${SDL2_INCLUDE_DIRS}
        )
    endif()
endif()

target_include_directories(picosystem PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../libraries/)

target_link_libraries(picosystem PUBLIC ${SDL2_LIBRARIES})

function(picosystem_executable NAME SOURCES)
  add_executable(
    ${NAME}
    ${SOURCES}
    ${ARGN}
  )

  # Pull in pico libraries that we need
  target_link_libraries(${NAME} picosystem)

  # create map/bin/hex file etc.

  pico_add_extra_outputs(${NAME})
endfunction()

function(pixel_double NAME)
  target_compile_options(${NAME} PRIVATE -DPIXEL_DOUBLE)
  target_compile_options(picosystem PRIVATE -DPIXEL_DOUBLE)
endfunction()

function(disable_startup_logo NAME)
  target_compile_options(${NAME} PRIVATE -DNO_STARTUP_LOGO)
  target_compile_options(picosystem PRIVATE -DNO_STARTUP_LOGO)
endfunction()

function(no_font NAME)
  target_compile_options(${NAME} PRIVATE -DNO_FONT)
  target_compile_options(picosystem PRIVATE -DNO_FONT)
endfunction()

function(no_spritesheet NAME)
  target_compile_options(${NAME} PRIVATE -DNO_SPRITESHEET)
  target_compile_options(picosystem PRIVATE -DNO_SPRITESHEET)
endfunction()

function(no_overclock NAME)
  # noop on SDL
endfunction()