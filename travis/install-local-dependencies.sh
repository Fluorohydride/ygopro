#!/usr/bin/env bash

set -euxo pipefail

# We are locked to FreeType 2.6.5 for now because of TTF interpreter behaviour in newer versions
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name http://downloads.sourceforge.net/freetype/freetype-2.6.5.tar.bz2
echo Extracting FreeType...
tar xf freetype-2.6.5.tar.bz2 > /dev/null
mv freetype-2.6.5/builds freetype
mv freetype-2.6.5/include freetype
mv freetype-2.6.5/src freetype
rm -rf freetype-2.6.5
rm freetype-2.6.5.tar.bz2

curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://downloads.sourceforge.net/irrlicht/irrlicht-1.8.4.zip
echo Extracting irrlicht... this may take some time.
unzip -uo irrlicht-1.8.4.zip > /dev/null
mv irrlicht-1.8.4/include irrlicht/include
# Technically, only code files need to be moved, and code files in lzma and aesGladman, but this is easier
mv irrlicht-1.8.4/source/Irrlicht irrlicht/src
rm -rf irrlicht-1.8.4
rm irrlicht-1.8.4.zip
# We will build against vcpkg-provided versions of these libs
rm -rf irrlicht/src/bzip2 irrlicht/src/jpeglib irrlicht/src/libpng irrlicht/src/zlib irrlicht/src/MacOSX
patch -p0 -i irrlicht/irrlicht-windows.patch
echo Patched irrlicht.
