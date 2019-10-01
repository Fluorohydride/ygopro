#!/usr/bin/env bash

set -euxo pipefail

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    ./travis/get-windows-dependencies.sh
    ./travis/get-windows-vcpkg-cache.sh
    ./travis/get-windows-d3d9sdk.sh
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    ./travis/get-linux-vcpkg-cache.sh
#   ./ocgcore/travis/install-lua.sh
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    ./travis/get-osx-sdk.sh $MACOSX_DEPLOYMENT_TARGET
    ./travis/get-osx-cache.sh
#   ./ocgcore/travis/install-lua.sh
fi
