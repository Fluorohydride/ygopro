#!/usr/bin/env bash

set -euxo pipefail

mkdir -p "$VCPKG_ROOT"
cd "$VCPKG_ROOT"
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output installed.7z $VCPKG_CACHE_7Z_URL
7z x installed.7z
mv irrlicht/include /usr/local/include/irrlicht
mv irrlicht/lib/* /usr/local/lib
rm -rf installed/x64-osx/include/freetype
rm installed/x64-osx/include/ft2build.h
rm installed/x64-osx/lib/libfreetype.a
rm installed/x64-osx/debug/lib/libfreetype.a
