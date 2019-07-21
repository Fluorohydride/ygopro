#!/usr/bin/env bash

if [[ $(command -v brew) == "" ]]; then
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
fi
brew install dylibbundler gpatch p7zip libevent fmt freetype sqlite curl libgit2 nlohmann-json
