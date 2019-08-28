#!/usr/bin/env bash

# Packages just the binaries into deploy

set -euxo pipefail

BUILD_CONFIG=${BUILD_CONFIG:-release}

function copy_if_exists {
    if [[ -f bin/$BUILD_CONFIG/$1 ]]; then
        cp bin/$BUILD_CONFIG/$1 deploy
    fi
}

function strip_if_exists {
    # Unfortunately Windows seems to be allowing us to refer to exe files without the extensions
    # so check for the existence of strip
    if [[ "$(which strip)" != "" ]] && [[ "$BUILD_CONFIG" == "release" ]] && [[ -f bin/$BUILD_CONFIG/$1 ]]; then
        strip bin/$BUILD_CONFIG/$1
    fi
}

function bundle_if_exists {
    if [[ -f bin/$BUILD_CONFIG/$1.app ]]; then
        mkdir -p deploy/$1.app/Contents/MacOS
        # Binary seems to be incorrectly named with the current premake
        cp bin/$BUILD_CONFIG/$1.app deploy/$1.app/Contents/MacOS/$1
        dylibbundler -x deploy/$1.app/Contents/MacOS/$1 -b -d deploy/$1.app/Contents/Frameworks/ -p @executable_path/../Frameworks/ -cd
        # OpenSSL isn't in /usr/local/lib because Apple has deprecated it.
        # libssl for some reason doesn't link to the libcrypto symlink in /usr/local/opt/openssl/lib,
        # but directly to the Cellar location, and this isn't caught by dylibbundler
        # This line likely needs to be updated if libcrypto's version ever changes, but not openssl's version
        install_name_tool -change /usr/local/Cellar/openssl/1.0.2s/lib/libcrypto.1.0.0.dylib @executable_path/../Frameworks/libcrypto.1.0.0.dylib deploy/$1.app/Contents/Frameworks/libssl.1.0.0.dylib

        mkdir -p deploy/$1.app/Contents/Resources
        cp gframe/ygopro.icns deploy/$1.app/Contents/Resources/edopro.icns
        defaults write "$PWD/deploy/$1.app/Contents/Info.plist" "CFBundleIconFile" "edopro.icns"
        defaults write "$PWD/deploy/$1.app/Contents/Info.plist" "CFBundleIdentifier" "io.github.edo9300.$1"
    fi
}

mkdir -p deploy

copy_if_exists ocgcore.dll
copy_if_exists ygopro.exe
copy_if_exists ygoprodll.exe

copy_if_exists libocgcore.so
strip_if_exists ygopro
copy_if_exists ygopro
strip_if_exists ygoprodll
copy_if_exists ygoprodll

copy_if_exists libocgcore.dylib
strip_if_exists ygopro.app
bundle_if_exists ygopro
strip_if_exists ygoprodll.app
bundle_if_exists ygoprodll
