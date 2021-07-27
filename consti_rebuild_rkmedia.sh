#!/bin/bash

# to rebuild anything under the "external" directory, remove its folder 
# and then run ./build.sh
# buildroot will detect that, sync its sources from source and rebuild only the removed folder



rm -rf buildroot/output/rockchip_rv1126_rv1109/build/rkmedia

echo "removed directory"

./build.sh 
