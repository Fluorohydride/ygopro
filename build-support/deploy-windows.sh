#!/usr/bin/env bash

mkdir -p deploy
mkdir -p deploy/fonts
cp bin/${BUILD_CONFIG:-release}/*.dll deploy/
cp bin/${BUILD_CONFIG:-release}/*.exe deploy/
if [[ "${1:-32}" -eq "32" ]]; then
    cp irrKlang/bin/win32-visualStudio/irrKlang.dll deploy/irrKlang.dll
elif [[ "$1" -eq "64" ]]; then
    cp irrKlang/bin/winx64-visualStudio/irrKlang.dll deploy/irrKlang.dll
fi
cp *.conf deploy/
cp *.json deploy/
if [[ ! -d deploy/textures ]]; then cp -r textures deploy/; fi
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output deploy/cards.cdb https://github.com/YgoproStaff/live2019/raw/master/cards.cdb
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output deploy/fonts/NotoSansJP-Regular.otf https://github.com/googlefonts/noto-cjk/raw/master/NotoSansJP-Regular.otf
