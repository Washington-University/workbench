#!/bin/sh

#
# This file is executed by (via "launchd")
#    /System/Library/LaunchDaemons/edu.wustl.caret.build.mac64.plist
#
# (1) Update source code from repository
# (2) Build Caret programs
# (3) Copy executables into the distribution
#
# Based off Tim Coalson's "buildbot" script
#

#
# Go to correct directory
#
BUILD_ROOT_DIR=/Volumes/DS4600/caret7_development/mac64
GIT_ROOT_DIR=${BUILD_ROOT_DIR}/caret7_source
SRC_DIR=${GIT_ROOT_DIR}/src
BUILD_DIR=${BUILD_ROOT_DIR}/build
cd ${BUILD_ROOT_DIR}

#
# Setup Environment
#

#for now we use caret5's lib dir
LIB_DIR=/Volumes/DS4600/caret_development/mac64
QTDIR=${LIB_DIR}/libraries/qt-software/qt-4.7.1-static-64bit
export QTDIR

PATH=${QTDIR}/bin:${PATH}
export PATH

#
# Go into git checkout directory
#
cd ${GIT_ROOT_DIR}

#
# File for capturing standard error
#
ERROR_FILE=${BUILD_ROOT_DIR}/result_build_mac64.txt
rm -f ${ERROR_FILE}
echo "Caret7 Mac 64 Build Result" > ${ERROR_FILE}

#
# Update source from repository
#
git pull -u >> ${ERROR_FILE} 2>&1
#hg update >> ${ERROR_FILE} 2>&1

#
# Force updating __DATE__ that shows up in title bar
#
#if [ "$1" == "FORCE_UPDATE" ]
#then
#echo "Update Forced" >> ${ERROR_FILE} 2>&1
#rm -f caret/GuiMainWindow.o caret/caret5.app caret_command/caret_command.app caret_edit/caret_edit.app caret_command_operations/CommandVersion.o caret_edit/main.o >> ${ERROR_FILE} 2>&1
#fi

#
# Configure and build as release.
# Build twice so that if the build fails, the second build will
# contain just the output that shows the errors.
# Catch output and echo to screen.
#
mkdir -p ${BUILD_DIR} >> ${ERROR_FILE} 2>&1
cd ${BUILD_DIR} >> ${ERROR_FILE} 2>&1
cmake ${SRC_DIR} >> ${ERROR_FILE} 2>&1
make -j2 
make -j2 >> ${ERROR_FILE} 2>&1
cat ${ERROR_FILE}

#
# Copy to distribution directory
#
DIST_DIR=/Volumes/DS4600/caret7_distribution/caret/macosx64_apps
/bin/cp -rv Desktop/workbench.app ${DIST_DIR} >> ${ERROR_FILE} 2>&1
/bin/cp -v CommandLine/caret_command ${DIST_DIR} >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
cat ${ERROR_FILE} | mail -s 'Caret7 Mac 64 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu

