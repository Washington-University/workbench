#!/bin/sh

#
# This file is executed on the redhat build machine
#    
#
# (1) Update source code from repository
# (2) Build Caret programs
# (3) Copy executables into the distribution
#
# Based off Tim Coalson's "buildbot" script
#

BUILD_ROOT_DIR=/home/caret/caret7_dev
GIT_ROOT_DIR=${BUILD_ROOT_DIR}/caret7_source
BUILD_DIR=${BUILD_ROOT_DIR}/build
cd ${BUILD_ROOT_DIR}
echo "BUILD_DIR: ${BUILD_DIR}"

#
# Go into git checkout directory
#
cd ${GIT_ROOT_DIR}


#
# Update source from repository
#
echo "UPDATING SOURCE FROM GIT REPOSITORY"
git fetch
git reset --hard origin/master

#
# Build, assume the build directory is already set up as desired
#
echo "BUILDING SOURCE"
cd ${BUILD_DIR}
make -j2

#
# Copy to distribution directory
#
echo "COPYING PROGRAMS"
DIST_DIR=/mainpool/storage/distribution/caret7_distribution/workbench/exe_rh_linux64
scp -rv Desktop/wb_view caret@myelin1.wustl.edu:${DIST_DIR}
scp -rv CommandLine/wb_command caret@myelin1.wustl.edu:${DIST_DIR}

echo "SCRIPT COMPLETED SUCCESSFULLY"

