#!/usr/bin/env bash

if [[ ! $VCPKG_ROOT ]]; then
    export VCPKG_ROOT=/c/vcpkg
    git clone --depth=1 https://github.com/Microsoft/vcpkg.git $VCPKG_ROOT
    cd $VCPKG_ROOT
    powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '.\bootstrap-vcpkg.bat'"
    ./vcpkg.exe integrate install
    ./vcpkg.exe integrate powershell # optional
    cd -
fi
$VCPKG_ROOT/vcpkg.exe install --triplet ${1:-x86-windows-static} libevent lua[cpp] sqlite3 fmt curl libgit2 nlohmann-json bzip2 libjpeg-turbo libpng zlib