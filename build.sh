#!/bin/bash

set -e

this_dir=$(realpath $(dirname $0))
asio_installed=$HOME/asio-installed

rm -rf $asio_installed $this_dir/asio-module/__build $this_dir/__build

# Install the Asio module
mkdir -p $this_dir/asio-module/__build
cd $this_dir/asio-module/__build
cmake -DCMAKE_INSTALL_PREFIX=$asio_installed ..
make install

# Build the code
mkdir -p $this_dir/__build
cd $this_dir/__build
cmake -DCMAKE_PREFIX_PATH=$asio_installed -DCMAKE_CXX_COMPILER=clang++-19 -G Ninja ..
ninja