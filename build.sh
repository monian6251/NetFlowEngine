#!/bin/bash

mkdir build
cd build && cmake ..
make

cd .. && mkdir -p output/bin output/lib output/include
cp log.ini output/bin/
cp devices.conf output/bin/
cp build/NetFlowEngine output/bin/

rm -rf build
