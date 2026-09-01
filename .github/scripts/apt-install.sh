#!/bin/bash
set -e

# Fourth one is for eliot's build (without dependencies)
sudo apt update
# For gperf
sudo apt install -y autoconf autoconf-archive automake libtool
# For libxcrypt
sudo apt install -y libltdl-dev
# For qtbase
sudo apt install -y pkgconf libx11-dev libx11-xcb-dev \
    libxkbcommon-dev libxkbcommon-x11-dev libxcb-xkb-dev \
    libgl1-mesa-dev libegl1-mesa-dev libdbus-1-dev libxi-dev \
    '^libxcb.*-dev' libxrender-dev libglu1-mesa-dev libsm-dev libice-dev
# For eliot's main build
sudo apt install -y gettext cmake ninja-build "$@"
