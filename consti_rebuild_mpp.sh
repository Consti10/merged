#!/bin/bash

# to rebuild anything under the "external" directory, remove its folder 
# and then run ./build.sh
# buildroot will detect that, sync its sources from source and rebuild only the removed folder



rm -rf buildroot/output/rockchip_rv1126_rv1109/build/mpp-release

echo "removed directory"

./build.sh 

echo "rebuild"

adb push buildroot/output/rockchip_rv1126_rv1109/build/mpp-release/test/mpi_enc_test /oem/usr/bin/mpi_enc_test
adb push buildroot/output/rockchip_rv1126_rv1109/build/mpp-release/test/mpi_dec_test /oem/usr/bin/mpi_dec_test

echo "flashed"
