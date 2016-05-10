#!/bin/sh
BUILD=mac64_noodle
BUILD_SERVER=noodle.wustl.edu

BUILD_DIR=/Users/caret/caret7_autobuild
ssh -v caret@${BUILD_SERVER} ${BUILD_DIR}/caret7_source/build_scripts/${BUILD}.sh $1 > $PWD/remote_launch_${BUILD}.txt 2>&1
