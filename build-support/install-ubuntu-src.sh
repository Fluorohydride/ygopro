#!/usr/bin/env bash

set -euxo pipefail

# Installs needed dependencies to /usr/local from source
git clone --depth=1 --branch=master https://github.com/fmtlib/fmt.git /tmp/fmt
mkdir -p /tmp/fmt/build
cd /tmp/fmt/build
cmake .. -DFMT_TEST=OFF
make
sudo make install

sudo mkdir -p /usr/local/include/nlohmann
cd /usr/local/include/nlohmann
sudo curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
