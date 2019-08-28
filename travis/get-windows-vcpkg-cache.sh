#!/usr/bin/env bash

set -euxo pipefail

git clone --depth=1 https://github.com/Microsoft/vcpkg.git "$VCPKG_ROOT"
cd "$VCPKG_ROOT"
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output installed.zip $VCPKG_CACHE_ZIP_URL
unzip -uo installed.zip > /dev/null
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '.\bootstrap-vcpkg.bat'"
./vcpkg.exe integrate install
./vcpkg.exe install $VCPKG_LIBS
