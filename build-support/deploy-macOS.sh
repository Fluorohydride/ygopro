#!/usr/bin/env bash

function bundle {
    mkdir -p deploy/$1.app/Contents/MacOS
    # Binary seems to be incorrectly named with the current premake
    cp bin/${BUILD_CONFIG:-release}/$1.app deploy/$1.app/Contents/MacOS/$1
    dylibbundler -x deploy/$1.app/Contents/MacOS/$1 -b -d deploy/$1.app/Contents/Frameworks/ -p @executable_path/../Frameworks/ -cd
    # OpenSSL isn't in /usr/local/lib because Apple has deprecated it.
    # libssl for some reason doesn't link to the libcrypto symlink in /usr/local/opt/openssl/lib,
    # but directly to the Cellar location, and this isn't caught by dylibbundler
    # This line likely needs to be updated if libcrypto's version ever changes, but not openssl's version
    install_name_tool -change /usr/local/Cellar/openssl/1.0.2s/lib/libcrypto.1.0.0.dylib @executable_path/../Frameworks/libcrypto.1.0.0.dylib deploy/$1.app/Contents/Frameworks/libssl.1.0.0.dylib
    if [[ "$BUILD_CONFIG" -ne "debug" ]]; then
        strip deploy/$1.app/Contents/MacOS/$1
    fi
    mkdir -p deploy/$1.app/Contents/Resources
    cp gframe/ygopro.icns deploy/$1.app/Contents/Resources/edopro.icns
    defaults write "$PWD/deploy/$1.app/Contents/Info.plist" "CFBundleIconFile" "edopro.icns"
    defaults write "$PWD/deploy/$1.app/Contents/Info.plist" "CFBundleIdentifier" "io.github.edo9300.$1"
}

mkdir -p deploy
mkdir -p deploy/fonts
cp bin/${BUILD_CONFIG:-release}/*.dylib deploy/
bundle ygopro
bundle ygoprodll
cp *.conf deploy/
cp *.json deploy/
if [[ ! -d deploy/textures ]]; then cp -r textures deploy/; fi
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output deploy/cards.cdb https://github.com/YgoproStaff/live2019/raw/master/cards.cdb
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output deploy/fonts/NotoSansJP-Regular.otf https://github.com/googlefonts/noto-cjk/raw/master/NotoSansJP-Regular.otf
