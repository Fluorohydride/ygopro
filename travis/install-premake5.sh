#!/usr/bin/env bash

set -euxo pipefail

TRAVIS_OS_NAME=${TRAVIS_OS_NAME:-$1}
PREMAKE_VERSION=${PREMAKE_VERSION:-5.0.0-alpha15}

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v$PREMAKE_VERSION/premake-$PREMAKE_VERSION-windows.zip
	unzip -uo premake-$PREMAKE_VERSION-windows.zip
    rm premake-$PREMAKE_VERSION-windows.zip
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v$PREMAKE_VERSION/premake-$PREMAKE_VERSION-linux.tar.gz
	tar xf premake-$PREMAKE_VERSION-linux.tar.gz
    rm premake-$PREMAKE_VERSION-linux.tar.gz
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v$PREMAKE_VERSION/premake-$PREMAKE_VERSION-macosx.tar.gz
	tar xf premake-$PREMAKE_VERSION-macosx.tar.gz
    rm premake-$PREMAKE_VERSION-macosx.tar.gz
fi
