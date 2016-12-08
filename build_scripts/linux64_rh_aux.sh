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
# Exit with result of running 'make'
# Cannot scp does not work when run on cluster system to outside cluster
#
exit $?





#
# Copy to distribution directory
#
echo "COPYING PROGRAMS"
DIST_DIR=/mainpool/storage/distribution/caret7_distribution/workbench/exe_rh_linux64
scp Desktop/wb_view caret@myelin1.wustl.edu:${DIST_DIR}
scp CommandLine/wb_command caret@myelin1.wustl.edu:${DIST_DIR}
scp ${GIT_ROOT_DIR}/src/CommandLine/wb_shortcuts caret@myelin1.wustl.edu:/mainpool/storage/distribution/caret7_distribution/workbench/bin_rh_linux64
#put the completion into the exe directory, because it isn't useful to run it directly (and would make wb_comm<tab> behave worse)
scp ${GIT_ROOT_DIR}/src/CommandLine/bashcomplete_wb_command caret@myelin1.wustl.edu:${DIST_DIR}

echo "SCRIPT COMPLETED SUCCESSFULLY"

