export TERM=${TERM:="xterm-256color"}

# cache buster: 2026-08-25

MICROPYTHON_FLAVOUR="micropython"
MICROPYTHON_VERSION="v1.29.0"

DIR2UF2_VERSION="v0.1.0"

BOARD="PIMORONI_PICOSYSTEM"


function log_success {
	echo -e "$(tput setaf 2)$1$(tput sgr0)"
}

function log_inform {
	echo -e "$(tput setaf 6)$1$(tput sgr0)"
}

function log_warning {
	echo -e "$(tput setaf 1)$1$(tput sgr0)"
}

function ci_micropython_clone {
    log_inform "Using MicroPython $MICROPYTHON_FLAVOUR/$MICROPYTHON_VERSION"
    git clone https://github.com/$MICROPYTHON_FLAVOUR/micropython "$CI_BUILD_ROOT/micropython"
    git -C "$CI_BUILD_ROOT/micropython" checkout $MICROPYTHON_VERSION
    git -C "$CI_BUILD_ROOT/micropython" submodule update --init lib/pico-sdk
    git -C "$CI_BUILD_ROOT/micropython" submodule update --init lib/micropython-lib
    git -C "$CI_BUILD_ROOT/micropython" submodule update --init lib/mbedtls
    git -C "$CI_BUILD_ROOT/micropython" submodule update --init lib/tinyusb
}

function ci_tools_clone {
    mkdir -p "$CI_BUILD_ROOT/tools"
    git clone https://github.com/gadgetoid/dir2uf2 -b "$DIR2UF2_VERSION" "$CI_BUILD_ROOT/tools/dir2uf2"
}

function ci_micropython_build_mpy_cross {
    ccache --zero-stats || true
    CROSS_COMPILE="ccache " make -C "$CI_BUILD_ROOT/micropython/mpy-cross"
    ccache --show-stats || true
}

function ci_apt_install_build_deps {
    sudo apt update && sudo apt install ccache
}

function ci_install_build_deps {
    ci_apt_install_build_deps
}

function ci_python_prepare {
    python3 -m pip install -r "${CI_PROJECT_ROOT}/ci/requirements.txt"
}

function ci_prepare_all {
    ci_tools_clone
    ci_micropython_clone
    ci_micropython_build_mpy_cross
}

function ci_debug {
    log_inform "Project root: $CI_PROJECT_ROOT"
    log_inform "Build root: $CI_BUILD_ROOT"
}

function micropython_version {
    echo "MICROPY_GIT_TAG=$MICROPYTHON_VERSION, $BOARD $TAG_OR_SHA" >> $GITHUB_ENV
    echo "MICROPY_GIT_HASH=$MICROPYTHON_VERSION-$TAG_OR_SHA" >> $GITHUB_ENV
}

function ci_cmake_configure {
    MICROPY_BOARD_DIR="$CI_PROJECT_ROOT/micropython/$BOARD"
    if [ ! -f "$MICROPY_BOARD_DIR/mpconfigboard.h" ]; then
        log_warning "Invalid board: \"$BOARD\"."
        return 1
    fi
    BUILD_DIR="$CI_BUILD_ROOT/build-$BOARD"
    cmake -S "$CI_BUILD_ROOT/micropython/ports/rp2" -B "$BUILD_DIR" \
    -DPICO_BUILD_DOCS=0 \
    -DUSER_C_MODULES="$CI_PROJECT_ROOT/micropython/modules/micropython.cmake" \
    -DMICROPY_BOARD_DIR="$MICROPY_BOARD_DIR" \
    -DMICROPY_BOARD="$BOARD" \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
}

function ci_cmake_build {
    BUILD_DIR="$CI_BUILD_ROOT/build-$BOARD"
    ccache --zero-stats || true
    cmake --build "$BUILD_DIR" -j 2 || return $?
    ccache --show-stats || true

    if [ -z ${CI_RELEASE_FILENAME+x} ]; then
        CI_RELEASE_FILENAME=picosystem
    fi

    log_inform "Copying firmware.uf2 to $(pwd)/$CI_RELEASE_FILENAME.uf2"
    cp "$BUILD_DIR/firmware.uf2" "$CI_RELEASE_FILENAME.uf2"
}

function ci_uf2_append_examples {
    MICROPY_BOARD_DIR="$CI_PROJECT_ROOT/micropython/$BOARD"
    if [ -z ${CI_RELEASE_FILENAME+x} ]; then
        CI_RELEASE_FILENAME=picosystem
    fi

    log_inform "Appending examples to $CI_RELEASE_FILENAME-with-examples.uf2"
    "$CI_BUILD_ROOT/tools/dir2uf2/dir2uf2" \
    --fs-compact \
    --append-to "$CI_RELEASE_FILENAME.uf2" \
    --manifest "$MICROPY_BOARD_DIR/uf2-manifest.txt" \
    --filename with-examples.uf2 \
    "$CI_PROJECT_ROOT/micropython/examples/picosystem/"
}

if [ -z ${CI_USE_ENV+x} ] || [ -z ${CI_PROJECT_ROOT+x} ] || [ -z ${CI_BUILD_ROOT+x} ]; then
    SCRIPT_PATH=${BASH_SOURCE-$0}
    SCRIPT_PATH=$(dirname "$SCRIPT_PATH")
    CI_PROJECT_ROOT=$(realpath "$SCRIPT_PATH/..")
    CI_BUILD_ROOT=$(pwd)
fi

ci_debug
