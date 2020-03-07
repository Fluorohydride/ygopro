#!/usr/bin/env bash

set -euxo pipefail

# Install irrlicht from source with custom patches
# Not easily included in local project structure due to Objective-C code
cd /tmp
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/edo9300/irrlicht1-8-4/archive/master.zip
7z x irrlicht1-8-4-master.zip
cd irrlicht1-8-4-master
xcodebuild -project source/Irrlicht/MacOSX/MacOSX.xcodeproj -configuration Release -target libIrrlicht.a SYMROOT=build -sdk $SDKROOT -parallelizeTargets
sudo mkdir -p /usr/local/include/irrlicht
sudo cp -r include/*.h /usr/local/include/irrlicht
sudo cp source/Irrlicht/MacOSX/build/Release/libIrrlicht.a /usr/local/lib

# Install libevent from source due to binary compatibility issues with 10.13 and earlier
# Overwrite Homebrew's Cellar
cd /tmp
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/libevent/libevent/releases/download/release-2.1.11-stable/libevent-2.1.11-stable.tar.gz
tar xf libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
./configure --prefix /usr/local/opt/libevent
make -j2
make install

cd /tmp
git clone --depth=1 https://github.com/edo9300/discord-rpc.git
mkdir -p discord-rpc/build
cd discord-rpc/build
cmake ..
cmake --build .
cmake --install .
