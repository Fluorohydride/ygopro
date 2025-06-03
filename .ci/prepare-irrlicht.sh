#!/bin/bash
set -x
set -o errexit
source .ci/asset-branch

IRRLICHT_REPO_URL="https://code.moenext.com/mycard/irrlicht-new.git"
IRRLICHT_BRANCH_NAME="$ASSET_BRANCH_NAME"

if [ ! -d "irrlicht" ]; then
  git clone --depth=1 --branch "$IRRLICHT_BRANCH_NAME" "$IRRLICHT_REPO_URL" irrlicht
else
  cd irrlicht
  git fetch origin "$IRRLICHT_BRANCH_NAME"
  git checkout "$IRRLICHT_BRANCH_NAME"
  git reset --hard origin/"$IRRLICHT_BRANCH_NAME"
  cd ..
fi
