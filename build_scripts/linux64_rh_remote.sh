#!/bin/sh
BUILD=linux64_rh
BUILD_SERVER=login01.chpc.wustl.edu
COPY_SERVER=dtn01.chpc.wustl.edu

BUILD_DIR=/home/caret/caret7_dev

ssh -v caret@${BUILD_SERVER} ${BUILD_DIR}/caret7_source/build_scripts/${BUILD}.sh $1 > $PWD/remote_launch_${BUILD}.txt 2>&1
buildResult=$?

if [ ! $result ]; then
    echo "Build was successful, now copying to distribution directory"

    #
    # Copy to distribution directory
    # Copy from 'dtn01 (copying will hang on login01)
    #
    echo "COPYING PROGRAMS"
    COPY_FROM_DIR=caret@${COPY_SERVER}:${BUILD_DIR}/build
    DIST_DIR=/mainpool/storage/distribution/caret7_distribution/workbench/exe_rh_linux64
    scp ${COPY_FROM_DIR}/Desktop/wb_view ${DIST_DIR} > $PWD/remote_launch_${BUILD}.txt 2>&1
    scp ${COPY_FROM_DIR}/CommandLine/wb_command ${DIST_DIR} > $PWD/remote_launch_${BUILD}.txt 2>&1
 
    # 
    # Need to be changed
    #
    #scp ${GIT_ROOT_DIR}/src/CommandLine/wb_shortcuts caret@myelin1.wustl.edu:/mainpool/storage/distribution/caret7_distribution/workbench/bin_rh_linux64
    #put the completion into the exe directory, because it isn't useful to run it directly (and would make wb_comm<tab> behave worse)
    #scp ${GIT_ROOT_DIR}/src/CommandLine/bashcomplete_wb_command caret@myelin1.wustl.edu:${DIST_DIR}

echo "SCRIPT COMPLETED SUCCESSFULLY"
else
    echo "Build failed"
fi

#cat $PWD/remote_launch_${BUILD}.txt | mailx -s 'Caret7 Linux 64 RedHat Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu


