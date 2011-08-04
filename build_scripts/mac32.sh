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
BUILD_DIR=/Volumes/DS4600/caret7_development/mac32
cd ${BUILD_DIR}

#
# Setup Environment
#

#for now we use caret5's lib dir
LIB_DIR=/Volumes/DS4600/caret_development/mac32
QTDIR=${BUILD_DIR}/libraries/qt-software/qt-4.7.1-static-32bit
export QTDIR

PATH=${QTDIR}/bin:${PATH}
export PATH

#
# Go into source directory
#
cd caret7_source

#
# File for capturing standard error
#
ERROR_FILE=${BUILD_DIR}/result_build_mac32.txt
rm -f ${ERROR_FILE}
echo "Caret7 Mac 32 Build Result" > ${ERROR_FILE}

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
#rm -rfv caret/GuiMainWindow.o caret/caret5.app caret_command/caret_command.app caret_edit/caret_edit.app caret_command_operations/CommandVersion.o caret_edit/main.o >> ${ERROR_FILE} 2>&1
#fi

#
# Configure and build as release.
# Build twice so that if the build fails, the second build will
# contain just the output that shows the errors.
# Catch output and echo to screen.
#
mkdir -p build >> ${ERROR_FILE} 2>&1
cd build >> ${ERROR_FILE} 2>&1
cmake -DCMAKE_OSX_ARCHITECTURES=i386 ../src >> ${ERROR_FILE} 2>&1
make -j2 
make -j2 >> ${ERROR_FILE} 2>&1
cat ${ERROR_FILE}

#
# Copy to distribution directory
#
DIST_DIR=/Volumes/DS4600/caret7_distribution/caret/macosx32_apps
/bin/cp -v Desktop/desktop ${DIST_DIR} >> ${ERROR_FILE} 2>&1
/bin/cp -v Command/caret_command ${DIST_DIR} >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
cat ${ERROR_FILE} | mail -s 'Caret7 Mac 32 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu

