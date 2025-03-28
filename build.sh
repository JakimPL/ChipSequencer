#!/bin/bash

mkdir -p build
mkdir -p bin
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
