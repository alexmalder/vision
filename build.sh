#/bin/bash
set -eo pipefail

mkdir -p build
cd build
cmake ..
make -j4
make install
cd ../
