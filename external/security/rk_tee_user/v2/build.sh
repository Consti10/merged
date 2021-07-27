#!/bin/bash
WORK_DIR_TOP=$(cd `dirname $0` ; pwd)

#./build.sh or ./build.sh 3232 to compile CA TA with 32 bits
#./build.sh 3264 to compile CA with 32 bits and TA with 64 bits
#./build.sh 6464 to compile CA TA with 64 bits
#./build.sh 6432 to compile CA with 64 bits and TA with 32 bits
#./build.sh ta to compile TA with 32 bits

TOOLCHAIN_PREBUILTS=$WORK_DIR_TOP/../../../prebuilts
TOOLCHAIN_PATH_ARM32=gcc/linux-x86/arm/gcc-linaro-6.3.1-2017.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-
TOOLCHAIN_PATH_AARCH64=gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-

if [ ! -d "$TOOLCHAIN_PREBUILTS" ]; then
	TOOLCHAIN_PREBUILTS=$WORK_DIR_TOP/../../../../prebuilts
fi

AARCH64_TOOLCHAIN=$TOOLCHAIN_PREBUILTS/$TOOLCHAIN_PATH_AARCH64
ARM32_TOOLCHAIN=$TOOLCHAIN_PREBUILTS/$TOOLCHAIN_PATH_ARM32

make TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 clean
BUILD_CATA_BITS="$1"
if [ "$BUILD_CATA_BITS" == "3232" ]; then
	export BUILD_CA=y
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "3264" ]; then
	export BUILD_CA=y
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE=$AARCH64_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm64 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "6464" ]; then
	export BUILD_CA=y
	make CROSS_COMPILE_HOST=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_TA=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE=$AARCH64_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm64 \
	COMPILE_NS_USER=64 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "6432" ]; then
	export BUILD_CA=y
	make CROSS_COMPILE_HOST=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=64 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "" ]; then
	export BUILD_CA=y
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "ta" ]; then
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi
