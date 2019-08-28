#!/usr/bin/env bash

set -euxo pipefail

cd /tmp
curl --retry 5 --connect-timeout 30 --location --remote-header-name --output cache.7z $CACHE_7Z_URL
7z x cache.7z

cp -r irrlicht/include /usr/local/include/irrlicht
cp irrlicht/lib/* /usr/local/lib
cp -r libevent /usr/local/Cellar/libevent/2.1.11
