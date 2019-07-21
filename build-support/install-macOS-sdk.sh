#!/usr/bin/env bash

if [[ "$#" -lt 1 ]]; then
    echo "Usage: ./install-macOS-sdk.sh [version] <...>"
    echo "  e.g. ./install-macOS-sdk.sh 10.9 10.12"
    exit
fi

SDK_PATH=$(dirname `xcrun --sdk macosx --show-sdk-path`)
while [[ $# -gt 0 ]]; do
    curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://github.com/phracker/MacOSX-SDKs/releases/download/10.13/MacOSX$1.sdk.tar.xz
    sudo tar xf MacOSX$1.sdk.tar.xz -C $SDK_PATH
    rm MacOSX$1.sdk.tar.xz
    shift
done
