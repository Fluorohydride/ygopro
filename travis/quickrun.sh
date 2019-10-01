#!/usr/bin/env bash

# Let's you run the binary out of the built folder on Windows and Linux
# Downloads a sample card database and font
# Copies local configs, textures, and irrKlang shared libraries
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

if [[ -f irrKlang/bin/win32-visualStudio/irrKlang.dll ]]; then
    if [[ "${1:-32}" -eq "32" ]]; then
        cp irrKlang/bin/win32-visualStudio/*.dll $DESTINATION
    elif [[ "$1" -eq "64" ]]; then
        cp irrKlang/bin/winx64-visualStudio/*.dll $DESTINATION
    fi
fi

if [[ ! -f $DESTINATION/libIrrKlang.so ]]; then
    cp irrKlang/bin/linux-gcc-64/*.so $DESTINATION
fi
