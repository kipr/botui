#!/bin/bash
set -e
cd "$(dirname "$0")"
mkdir -p build && cd build

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build . -- -j "$(nproc)"
