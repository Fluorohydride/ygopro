#!/usr/bin/env bash

set -euxo pipefail

mkdir -p "$VCPKG_ROOT"
cd "$VCPKG_ROOT"
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output installed.7z $VCPKG_CACHE_7Z_URL
7z x installed.7z
