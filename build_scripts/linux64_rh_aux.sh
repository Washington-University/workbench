#!/bin/sh

#
# This file is executed by myelin1
#    
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
BUILD_ROOT_DIR=/home/caret/caret7_dev
GIT_ROOT_DIR=${BUILD_ROOT_DIR}/caret7_source
SRC_DIR=${GIT_ROOT_DIR}/src
BUILD_DIR=${BUILD_ROOT_DIR}/build
cd ${BUILD_ROOT_DIR}
echo "BUILD_DIR: ${BUILD_DIR}"

#
# Setup Environment
#

#
# Use Qt SDK
#

QTDIR=/home/caret/caret5_and_7_libraries/install/qt-4.8.4-shared
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
git reset --hard HEAD
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
cmake -DCMAKE_BUILD_TYPE=Release ${SRC_DIR}
make -j2 
make -j2

#
# Copy to distribution directory
#
echo "COPYING PROGRAMS"
DIST_DIR=/mainpool/storage/distribution/caret7_distribution/workbench/exe_rh_linux64
scp -rv Desktop/workbench caret@myelin1.wustl.edu:${DIST_DIR}
scp -rv CommandLine/wb_command caret@myelin1.wustl.edu:${DIST_DIR}

ssh caret@myelin1.wustl.edu touch ${DIST_DIR}/workbench

echo "SCRIPT COMPLETED SUCCESSFULLY"

