FROM gitpod/workspace-full

RUN sudo apt-get update && sudo apt-get install -y \
    cmake \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    && sudo rm -rf /var/lib/apt/lists/*
RUN git clone https://github.com/raspberrypi/pico-sdk.git