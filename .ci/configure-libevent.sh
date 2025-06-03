#!/bin/sh
set -x
set -o errexit

cd event
./configure --disable-openssl --enable-static=yes --enable-shared=no
sed -f make-event-config.sed < config.h > ./include/event2/event-config.h
cd ..
