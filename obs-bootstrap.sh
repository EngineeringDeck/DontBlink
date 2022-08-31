#!/bin/bash

pushd `dirname $0`
if [ -d "obs-studio" ]; then
    pushd obs-studio
    git pull
else
    git clone --branch 27.2.4 --recursive https://github.com/obsproject/obs-studio.git
    pushd obs-studio
fi
./CI/full-build-macos.sh
popd
if [ ! -d "obs-qt" ]; then
	mkdir obs-qt
fi
pushd obs-qt
tar -xvf ../obs-build-dependencies/macos-deps-qt-2022-02-13-x86_64.tar.xz
popd
popd
