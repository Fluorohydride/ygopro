#!/usr/bin/env bash

set -euxo pipefail

if [[ "$TRAVIS_OS_NAME" == "windows" ]]; then
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip
	unzip -uo premake-5.0.0-alpha14-windows.zip
    rm premake-5.0.0-alpha14-windows.zip
fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-linux.tar.gz
	tar xf premake-5.0.0-alpha14-linux.tar.gz
    rm premake-5.0.0-alpha14-linux.tar.gz
fi
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
	curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-macosx.tar.gz
	tar xf premake-5.0.0-alpha14-macosx.tar.gz
    rm premake-5.0.0-alpha14-macosx.tar.gz
fi
