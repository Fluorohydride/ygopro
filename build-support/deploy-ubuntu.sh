#!/usr/bin/env bash

mkdir -p deploy
mkdir -p deploy/fonts
cp bin/${BUILD_CONFIG:-release}/*.so deploy/
cp bin/${BUILD_CONFIG:-release}/ygopro deploy/
cp bin/${BUILD_CONFIG:-release}/ygoprodll deploy/
if [[ "$BUILD_CONFIG" -ne "debug" ]]; then
    strip deploy/ygopro
    strip deploy/ygoprodll
fi
cp irrKlang/bin/linux-gcc-64/libIrrKlang.so deploy/
cp *.conf deploy/
cp *.json deploy/
if [[ ! -d deploy/textures ]]; then cp -r textures deploy/; fi
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output deploy/cards.cdb https://github.com/YgoproStaff/live2019/raw/master/cards.cdb
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output deploy/fonts/NotoSansJP-Regular.otf https://github.com/googlefonts/noto-cjk/raw/master/NotoSansJP-Regular.otf
