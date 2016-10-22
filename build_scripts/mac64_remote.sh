#!/bin/sh
BUILD=mac64
BUILD_SERVER=hippocampus.wustl.edu

BUILD_DIR=/Users/caret/caret7_development/${BUILD}
ssh -v caret@${BUILD_SERVER} ${BUILD_DIR}/caret7_source/build_scripts/${BUILD}.sh $1 > $PWD/remote_launch_${BUILD}.txt 2>&1
cat $PWD/remote_launch_${BUILD}.txt | mailx -s 'Caret7 Linux 64 Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu
