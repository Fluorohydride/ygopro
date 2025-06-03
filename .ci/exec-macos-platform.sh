#!/bin/bash
set -x
set -o errexit

TARGET_PLATFORM=$(arch)
TARGET_YGOPRO_BINARY_PATH=./ygopro-platforms/ygopro-platform-$TARGET_PLATFORM

./.ci/configure-audio.sh

rm -rf sqlite3/VERSION sqlite3/version

./premake5 gmake --cc=clang

cd build
make config=release -j$(sysctl -n hw.ncpu)
cd ..

mkdir ygopro-platforms
mv bin/release/YGOPro.app $TARGET_YGOPRO_BINARY_PATH

#if [[ $TARGET_PLATFORM == "x86" ]]; then
#  install_name_tool -change /usr/local/lib/libirrklang.dylib @executable_path/../Frameworks/libirrklang.dylib $TARGET_YGOPRO_BINARY_PATH
#fi

strip $TARGET_YGOPRO_BINARY_PATH
