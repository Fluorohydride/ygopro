#!/bin/bash
set -x
set -o errexit
source .ci/asset-branch
# ygopro-database
apt update && apt -y install wget git libarchive-tools sqlite3
git clone --depth=1 -b "$ASSET_BRANCH_NAME" https://code.moenext.com/mycard/ygopro-database
cp -rf ./ygopro-database/locales/$TARGET_LOCALE/strings.conf .
rm -f cards.cdb
sqlite3 ./ygopro-database/locales/$TARGET_LOCALE/cards.cdb .dump | sqlite3 cards.cdb
# ygopro-images
mkdir pics
if [[ "$ASSET_BRANCH_NAME" == "develop" ]]; then
  echo "This is a pre-release, skipping download."
else
  wget -O - https://cdn02.moecube.com:444/images/ygopro-images-${TARGET_LOCALE}.zip | bsdtar -C pics -xf -
fi
