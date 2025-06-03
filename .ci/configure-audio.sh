#!/bin/bash

libs_to_configure=(
  "ogg"
  # "opus"
)

cd miniaudio/external
for lib in "${libs_to_configure[@]}"; do
  cd $lib
  ./configure
  cd ..
done
