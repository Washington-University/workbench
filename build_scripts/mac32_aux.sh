#!/bin/sh

#
# This file is executed by (via "launchd")
#    /System/Library/LaunchDaemons/edu.wustl.caret.build.mac32.plist
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
BUILD_ROOT_DIR=/Volumes/DS4600/caret7_development/mac32
GIT_ROOT_DIR=${BUILD_ROOT_DIR}/caret7_source
SRC_DIR=${GIT_ROOT_DIR}/src
BUILD_DIR=${BUILD_ROOT_DIR}/build
cd ${BUILD_ROOT_DIR}
echo "BUILD_DIR: ${BUILD_DIR}"

#
# Use Qt SDK
#
QTDIR=/Users/caret/QtSDK/Desktop/Qt/474/gcc
export QTDIR
echo "QTDIR: ${QTDIR}"

PATH=${QTDIR}/bin:${PATH}
export PATH
QMAKE_PROG=`which qmake`
echo "QMAKE: ${QMAKE_PROG}"

#
# Go into git checkout directory
#
cd ${GIT_ROOT_DIR}


#
# Update source from repository
#
echo "UPDATING SOURCE FROM GIT REPOSITORY"
git pull -u

#
# Configure and build as release.
# Build twice so that if the build fails, the second build will
# contain just the output that shows the errors.
# Catch output and echo to screen.
#
echo "BUILDING SOURCE"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake ${SRC_DIR}
make -j2 
make -j2

#
# Run 'macdeployqt' on the App so that frameworks are copied
#
echo "RUNNING MACDEPLOYQT"
macdeployqt Desktop/workbench.app

#
# Copy to distribution directory
#
echo "COPYING PROGRAMS"
DIST_DIR=/Volumes/DS4600/caret7_distribution/caret/macosx32_apps
/bin/cp -rv Desktop/workbench.app ${DIST_DIR}
/bin/cp -v CommandLine/caret_command ${DIST_DIR}


echo "SCRIPT COMPLETED SUCCESSFULLY"

