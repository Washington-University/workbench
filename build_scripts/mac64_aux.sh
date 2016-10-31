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
BUILD_ROOT_DIR=/Users/caret/caret7_development/mac64
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
#QTDIR=/Users/caret/QtSDK/Desktop/Qt/474/gcc
# has SSL linked
QTDIR=/opt/caret64_sdk/install/qt-4.8.3
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
#CC_COMPILER=/usr/local/clang-llvm/clang+llvm-3.2-x86_64-apple-darwin11/bin/clang
#CXX_COMPILER=/usr/local/clang-llvm/clang+llvm-3.2-x86_64-apple-darwin11/bin/clang++
#CC_COMPILER=/usr/bin/gcc
#CXX_COMPILER=/usr/bin/g++
#CC_COMPILER=/usr/local/gcc-4.9.3/bin/gcc
#CXX_COMPILER=/usr/local/gcc-4.9.3/bin/g++

#
# Clang compiler with OpenMP
#
CC_COMPILER=/usr/local/clang-openmp-opt/llvm/build/Release/bin/clang2
CXX_COMPILER=/usr/local/clang-openmp-opt/llvm/build/Release/bin/clang2++
OPENMP_COMPILE_OPTION=-fopenmp
export OPENMP_COMPILE_OPTION
OPENMP_HEADER_DIR=/usr/local/clang-openmp-opt/llvm/build/Release/include
export OPENMP_HEADER_DIR
OPENMP_LIB_DIR=/usr/local/clang-openmp-opt/llvm/build/Release/lib
export OPENMP_LIB_DIR


echo "BUILDING SOURCE"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
/usr/local/cmake-3.6.2/bin/cmake \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_C_COMPILER=${CC_COMPILER} \
   -DCMAKE_CXX_COMPILER=${CXX_COMPILER} \
   -DCMAKE_VERBOSE_MAKEFILE=FALSE \
   ${SRC_DIR}
make -j2 
make -j2

#
# Run 'macdeployqt' on the Apps so that frameworks are copied
#
echo "RUNNING MACDEPLOYQT"
macdeployqt Desktop/wb_view.app -verbose=0
macdeployqt CommandLine/wb_command.app -verbose=0

#
# Copy to distribution directory
#
echo "COPYING PROGRAMS"
DIST_DIR=/mainpool/storage/distribution/caret7_distribution/workbench/macosx64_apps
scp -r Desktop/wb_view.app caret@myelin1:${DIST_DIR}
scp -r CommandLine/wb_command.app caret@myelin1:${DIST_DIR}
scp ${SRC_DIR}/CommandLine/wb_shortcuts caret@myelin1:/mainpool/storage/distribution/caret7_distribution/workbench/bin_macosx64
scp ${SRC_DIR}/CommandLine/bashcomplete_wb_command caret@myelin1:${DIST_DIR}

echo "SCRIPT COMPLETED SUCCESSFULLY"

