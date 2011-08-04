#!/bin/sh
BUILD=linux64
BUILD_SERVER=sumsdev.wustl.edu

BUILD_DIR=/miniserver/caret7_development/${BUILD}
ssh -v caret@${BUILD_SERVER} ${BUILD_DIR}/caret7_source/build_scripts/${BUILD}.sh $1 > $PWD/remote_launch_${BUILD}.txt 2>&1
