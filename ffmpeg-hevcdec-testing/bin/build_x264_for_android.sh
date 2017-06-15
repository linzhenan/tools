#!/bin/bash

ANDROID_NDK=/Users/zhenan/dev/android-ndk-r14b
PLATFORM=${ANDROID_NDK}/platforms/android-14/arch-arm
TOOLCHAIN=${ANDROID_NDK}/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64

PREFIX=./android

./configure --prefix=$PREFIX \
            --enable-static \
            --enable-pic \
            --host=arm-linux \
            --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
            --sysroot=$PLATFORM \
            --extra-ldflags="-pie"

make clean
make -j 8
make install

