#!/usr/bin/env bash

set -euo pipefail

PICS_URL=${PICS_URL:-""}
FIELDS_URL=${FIELDS_URL:-""}
DISCORD_APP_ID=${DISCORD_APP_ID:-""}

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    ./premake5 vs2017 --pics=\"$PICS_URL\" --fields=\"$FIELDS_URL\" --discord=\"$DISCORD_APP_ID\" --sound=sdl-mixer
    "$MSBUILD_PATH/msbuild.exe" -m -p:Configuration=$BUILD_CONFIG ./build/ygo.sln -t:ygoprodll
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    ./premake5 gmake2 --vcpkg-root=$VCPKG_ROOT --pics=\"$PICS_URL\" --fields=\"$FIELDS_URL\" --discord=\"$DISCORD_APP_ID\" --sound=sdl-mixer
    make -Cbuild -j2 config=$BUILD_CONFIG ygoprodll
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    ./premake5 gmake2 --vcpkg-root=$VCPKG_ROOT --pics=\"$PICS_URL\" --fields=\"$FIELDS_URL\" --discord=\"$DISCORD_APP_ID\" --sound=sdl-mixer
    make -Cbuild -j2 config=$BUILD_CONFIG ygoprodll
    make -Cbuild -j2 config=$BUILD_CONFIG discord-launcher
fi
