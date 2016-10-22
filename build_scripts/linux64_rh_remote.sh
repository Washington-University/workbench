#!/bin/sh
BUILD=linux64_rh
BUILD_SERVER=login1.chpc.wustl.edu

#BUILD_DIR=/home/caret/caret7_dev/${BUILD}
BUILD_DIR=/home/caret/caret7_dev
ssh -v caret@${BUILD_SERVER} ${BUILD_DIR}/caret7_source/build_scripts/${BUILD}.sh $1 > $PWD/remote_launch_${BUILD}.txt 2>&1
cat $PWD/remote_launch_${BUILD}.txt | mailx -s 'Caret7 Linux 64 Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu
