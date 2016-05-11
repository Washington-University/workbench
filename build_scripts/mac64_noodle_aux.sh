#!/bin/sh

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
BUILD_ROOT_DIR=/Users/caret/caret7_autobuild
GIT_ROOT_DIR=${BUILD_ROOT_DIR}/caret7_source
SRC_DIR=${GIT_ROOT_DIR}/src
BUILD_SCRIPT_DIR=${GIT_ROOT_DIR}/build_scripts
#BUILD_SCRIPT_DIR=/Users/caret/develop_build_script
BUILD_DIR=${BUILD_ROOT_DIR}/build
cd ${BUILD_ROOT_DIR}
echo "BUILD_DIR: ${BUILD_DIR}"

#
# Setup Environment
#

#
# Use Qt SDK
#
QTDIR=/opt/caret64_sdk/install/qt-4.8.7/bin/qmake
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
# Clang compiler
#
# OpenMP is added to compiler and environment
# variables must be set so compiler finds OpenMP.
#
CC_COMPILER=/opt/local/bin/clang-mp-3.7
CXX_COMPILER=/opt/local/bin/clang++-mp-3.7
OPENMP_COMPILE_OPTION="-fopenmp -D_OPENMP"
export OPENMP_COMPILE_OPTION
OPENMP_HEADER_DIR=/opt/local/include/libomp
export OPENMP_HEADER_DIR
OPENMP_LIB_DIR=/opt/local/lib/libomp
export OPENMP_LIB_DIR

#
# Mesa Library for Offscreen OpenGL
#
OSMESA_DIR=/opt/caret64_sdk/install/Mesa-7.8.2

#
# CMake executable path
#
CMAKE_EXE=/opt/caret64_sdk/install/cmake-3.5.2/bin/cmake

#
# Configure with CMake
#
echo "BUILDING SOURCE"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
${CMAKE_EXE} \
   -DCMAKE_BUILD_TYPE=Release \
   -DCMAKE_C_COMPILER=${CC_COMPILER} \
   -DCMAKE_CXX_COMPILER=${CXX_COMPILER} \
   -DCMAKE_VERBOSE_MAKEFILE=FALSE \
   -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 \
   -DCMAKE_OSX_SYSROOT="/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk" \
   ${SRC_DIR}

#
# Run Make to build.
# It is run twice so that errors will be at end of log file
#
make -j2 
make -j

#
# Run 'macdeployqt' on the Apps so that Qt frameworks are copied
#
echo "RUNNING MACDEPLOYQT"
macdeployqt Desktop/wb_view.app -verbose=0
macdeployqt CommandLine/wb_command.app -verbose=0

#
# Need to copy OpenMP library to wb_view and wb_command
#
${BUILD_SCRIPT_DIR}/mac64_copy_library_to_workbench.sh \
   ${OPENMP_LIB_DIR}/libomp.dylib \
   ${BUILD_DIR}/Desktop/wb_view.app/Contents/MacOS/wb_view
${BUILD_SCRIPT_DIR}/mac64_copy_library_to_workbench.sh \
   ${OPENMP_LIB_DIR}/libomp.dylib \
   ${BUILD_DIR}/CommandLine/wb_command.app/Contents/MacOS/wb_command

#
# Copy to distribution directory
#
echo "COPYING PROGRAMS"
DIST_DIR=/mainpool/storage/distribution/caret7_distribution/workbench/macosx64_noodle_apps
scp -r Desktop/wb_view.app caret@myelin1:${DIST_DIR}
scp -r CommandLine/wb_command.app caret@myelin1:${DIST_DIR}

echo "SCRIPT COMPLETED SUCCESSFULLY"

