#!/bin/bash
set -x
set -o errexit
# TARGET_LOCALE

apt update && apt -y install tar git zstd
mkdir dist replay
tar --zstd -cf dist/ygopro-$CI_COMMIT_REF_NAME-darwin-$TARGET_LOCALE.tar.zst --exclude='.git*' ygopro.app LICENSE README.md lflist.conf strings.conf system.conf cards.cdb script textures deck single pics replay windbot bot bot.conf pack
