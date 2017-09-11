#!/bin/bash

# build the project

# clean the environment

mkdir -p build

cd build

cmake ..

make -j8

cd ..