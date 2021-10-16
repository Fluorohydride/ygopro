#!/usr/bin/env bash

set -euxo pipefail

./travis/install-local-dependencies.sh
if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
    ./travis/get-windows-vcpkg-cache.sh
    ./travis/get-windows-d3d9sdk.sh
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    ./travis/get-linux-vcpkg-cache.sh
#   ./ocgcore/travis/install-lua.sh
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	if [[ -z "${ARM64:-""}" ]]; then
		./travis/get-osx-sdk.sh $MACOSX_DEPLOYMENT_TARGET
	fi
    ./travis/get-osx-vcpkg-cache.sh
#   ./ocgcore/travis/install-lua.sh
fi
