#!/bin/sh
BUILD=mac32
BUILD_SERVER=hippocampus.wustl.edu

BUILD_DIR= /Volumes/DS4600/caret7_development/${BUILD}
ssh -v caret@${BUILD_SERVER} ${BUILD_DIR}/caret7_source/build_scripts/${BUILD}.sh $1 > ${$BUILD_DIR}/build_scripts/remote_launch_${BUILD}.txt 2>&1
