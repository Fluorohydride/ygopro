#!/usr/bin/env bash

cp irrlicht/irrlicht-macOS.patch /tmp
cd /tmp
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://downloads.sourceforge.net/irrlicht/irrlicht-1.8.4.zip
7z x irrlicht-1.8.4.zip
cd irrlicht-1.8.4
/usr/local/opt/gpatch/bin/patch -p0 --binary -i ../irrlicht-macOS.patch
xcodebuild -project source/Irrlicht/MacOSX/MacOSX.xcodeproj -configuration Release -target libIrrlicht.a SYMROOT=build -sdk $SDKROOT -parallelizeTargets
sudo mkdir -p /usr/local/include/irrlicht
sudo cp -r include/*.h /usr/local/include/irrlicht
sudo cp source/Irrlicht/MacOSX/build/Release/libIrrlicht.a /usr/local/lib

# Installs lua for C++ to /usr/local from source because brew's version is wonky
cd /tmp
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://www.lua.org/ftp/lua-5.3.5.tar.gz
tar xf lua-5.3.5.tar.gz
cd lua-5.3.5
make -j2 macosx CC=$CXX
sudo make install

# Install libevent from source due to binary compatibility issues with 10.13 and earlier
cd /tmp
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/libevent/libevent/releases/download/release-2.1.11-stable/libevent-2.1.11-stable.tar.gz
tar xf libevent-2.1.11-stable.tar.gz
cd libevent-2.1.11-stable
./configure --prefix /usr/local/Cellar/libevent/2.1.11
make -j2
make install
