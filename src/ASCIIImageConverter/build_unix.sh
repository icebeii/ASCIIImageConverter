#!/bin/bash

set -e
cd src
mkdir build
cd build
cmake ..
make