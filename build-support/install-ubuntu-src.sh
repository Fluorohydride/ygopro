#!/usr/bin/env bash

# Installs needed dependencies to /usr/local from source
git clone --depth=1 --branch=master https://github.com/fmtlib/fmt.git /tmp/fmt
cd /tmp/fmt
cmake .
make
sudo make install

curl --retry 5 --connect-timeout 30 --location --remote-header-name --remote-name https://raw.githubusercontent.com/nlohmann/json/develop/single_include/nlohmann/json.hpp
sudo mkdir -p /usr/local/include/nlohmann
sudo mv json.hpp /usr/local/include/nlohmann