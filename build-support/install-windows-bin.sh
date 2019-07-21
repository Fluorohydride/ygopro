#!/usr/bin/env bash

# Installs needed binaries to the working directory
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip
rm -f premake5.exe
unzip -uo premake-5.0.0-alpha14-windows.zip
rm premake-5.0.0-alpha14-windows.zip

# We are locked to FreeType 2.6.5 for now because of TTF interpreter behaviour in newer versions
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name http://downloads.sourceforge.net/freetype/freetype-2.6.5.tar.bz2
echo Extracting FreeType...
tar xf freetype-2.6.5.tar.bz2 > /dev/null
mv freetype-2.6.5/builds freetype
mv freetype-2.6.5/include freetype
mv freetype-2.6.5/src freetype
rm -rf freetype-2.6.5
rm freetype-2.6.5.tar.bz2

curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name http://www.ambiera.at/downloads/irrKlang-64bit-1.6.0.zip
echo Extracting irrKlang64...
unzip -uo irrKlang-64bit-1.6.0.zip > /dev/null
rm -rf irrKlang
mv irrKlang-64bit-1.6.0 irrKlang
rm irrKlang-64bit-1.6.0.zip

# We wrap irrKlang32's extract because its zip has a hidden macOS directory that we don't want
curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://www.ambiera.at/downloads/irrKlang-32bit-1.6.0.zip
echo Extracting irrKlang32...
unzip -uo irrKlang-32bit-1.6.0.zip -d irrKlang-tmp > /dev/null
# Merge 32-bit binaries into folder
mv irrKlang-tmp/irrKlang-1.6.0/bin/win32-gcc irrKlang/bin/win32-gcc
mv irrKlang-tmp/irrKlang-1.6.0/bin/win32-visualStudio irrKlang/bin/win32-visualStudio
mv irrKlang-tmp/irrKlang-1.6.0/lib/Win32-gcc irrKlang/lib/Win32-gcc
mv irrKlang-tmp/irrKlang-1.6.0/lib/Win32-visualStudio irrKlang/lib/Win32-visualStudio
rm -rf irrKlang-tmp
rm irrKlang-32bit-1.6.0.zip

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
