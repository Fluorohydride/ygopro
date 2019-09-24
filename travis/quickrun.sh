#!/usr/bin/env bash

# Let's you run the binary out of the built folder on Windows and Linux
# Downloads a sample card database and font
# Copies local configs and textures
set -euxo pipefail

DESTINATION=bin/${BUILD_CONFIG:-debug}

cp *.conf $DESTINATION
cp *.json $DESTINATION
if [[ ! -d $DESTINATION/textures ]]; then cp -r textures $DESTINATION; fi

mkdir -p $DESTINATION/fonts
if [[ ! -f $DESTINATION/fonts/NotoSansJP-Regular.otf ]]; then
    curl --retry 5 --connect-timeout 30 --location --remote-header-name --output $DESTINATION/fonts/NotoSansJP-Regular.otf https://github.com/googlefonts/noto-cjk/raw/master/NotoSansJP-Regular.otf
fi

if [[ ! -f $DESTINATION/cards.cdb ]]; then
    curl --retry 5 --connect-timeout 30 --location --remote-header-name --output $DESTINATION/cards.cdb https://github.com/YgoproStaff/live2019/raw/master/cards.cdb
fi
