#!/bin/sh
BUILD=linux64
BUILD_SERVER=128.252.37.32

BUILD_DIR=/home/caret/${BUILD}/caret7_dev
ssh -v caret@${BUILD_SERVER} "/bin/bash -c \"cd ${BUILD_DIR};./build64.sh $1\"" > $PWD/remote_launch_${BUILD}.txt 2>&1
cat $PWD/remote_launch_${BUILD}.txt | mail -s 'Caret7 Linux64 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu

