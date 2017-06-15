#!/bin/bash

ANDROID_NDK=/Users/zhenan/dev/android-ndk-r14b
PLATFORM=${ANDROID_NDK}/platforms/android-14/arch-arm
TOOLCHAIN=${ANDROID_NDK}/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64

PREFIX=./android
X264PREFIX=/Users/zhenan/dev/x264/android

ADDI_CFLAGS="-marm -I${X264PREFIX}/include"
ADDI_LDFLAGS="-L${X264PREFIX}/lib -lx264 -pie -fPIE"

./configure \
    --prefix=$PREFIX \
    --disable-shared \
    --enable-static \
    --enable-pic \
    --disable-doc \
    --enable-ffmpeg \
    --disable-ffplay \
    --disable-ffprobe \
    --disable-ffserver \
    --disable-avdevice \
    --disable-symver \
    --enable-gpl \
    --enable-libx264 \
    --enable-encoder=libx264 \
    --enable-decoder=h264 \
    --cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
    --target-os=linux \
    --arch=arm \
    --enable-cross-compile \
    --sysroot=$PLATFORM \
    --extra-cflags="-Os -fpic $ADDI_CFLAGS" \
    --extra-ldflags="$ADDI_LDFLAGS" \

make clean
make -j 4
make install
