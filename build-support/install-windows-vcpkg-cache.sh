#!/usr/bin/env bash

git clone --depth=1 https://github.com/Microsoft/vcpkg.git $VCPKG_ROOT
cd $VCPKG_ROOT
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/kevinlul/edopro-vcpkg-cache/raw/master/installed.zip
unzip -uo installed.zip
powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '.\bootstrap-vcpkg.bat'"
./vcpkg.exe integrate install
cd -
./build-support/install-windows-vcpkg.sh; 