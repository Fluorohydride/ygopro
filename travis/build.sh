#!/usr/bin/env bash

set -euo pipefail

PICS_URL=${PICS_URL:-""}
FIELDS_URL=${FIELDS_URL:-""}
COVERS_URL=${COVERS_URL:-""}
DISCORD_APP_ID=${DISCORD_APP_ID:-""}
UPDATE_URL=${UPDATE_URL:-""}

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    ./premake5 vs2017 --oldwindows=true --sound=sfml --no-joystick=true --pics=\"$PICS_URL\" --fields=\"$FIELDS_URL\" --covers=\"$COVERS_URL\" --discord=\"$DISCORD_APP_ID\" --update-url=\"$UPDATE_URL\"
    msbuild.exe -m -p:Configuration=$BUILD_CONFIG ./build/ygo.sln -t:ygoprodll -verbosity:minimal -p:EchoOff=true
    python.exe ./travis/patcher.py bin/$BUILD_CONFIG/ygoprodll.exe
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    ./premake5 gmake2 --vcpkg-root=$VCPKG_ROOT --sound=sfml --no-joystick=true --pics=\"$PICS_URL\" --fields=\"$FIELDS_URL\" --covers=\"$COVERS_URL\" --discord=\"$DISCORD_APP_ID\" --update-url=\"$UPDATE_URL\"
    make -Cbuild -j2 config=$BUILD_CONFIG ygoprodll
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    ./premake5 gmake2 --vcpkg-root=$VCPKG_ROOT --sound=sfml --no-joystick=true --pics=\"$PICS_URL\" --fields=\"$FIELDS_URL\" --covers=\"$COVERS_URL\" --discord=\"$DISCORD_APP_ID\" --update-url=\"$UPDATE_URL\"
    make -Cbuild -j2 config=$BUILD_CONFIG ygoprodll
fi
