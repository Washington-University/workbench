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
BUILD_DIR=/miniserver/caret7_development/linux32
cd ${BUILD_DIR}

#
# Setup Environment
#

#for now we use caret5's lib dir
LIB_DIR=/miniserver/caret_development/linux32
QTDIR=${LIB_DIR}/libraries/qt-software/qt-everywhere-opensource-src-4.7.1
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
ERROR_FILE=${BUILD_DIR}/result_build_linux32.txt
rm -f ${ERROR_FILE}
echo "Linux 32 Build Result" > ${ERROR_FILE}

#
# Update source from repository
#
git pull -u >> ${ERROR_FILE} 2>&1
#hg update >> ${ERROR_FILE} 2>&1

#
# Force updating __DATE__ that shows up in title bar
#
if [ "$1" == "FORCE_UPDATE" ]
then
echo "Update Forced" >> ${ERROR_FILE} 2>&1
rm -fv caret/GuiMainWindow.o caret/caret5 caret_command/caret_command caret_edit/caret_edit caret_command_operations/CommandVersion.o caret_edit/main.o >> ${ERROR_FILE} 2>&1
fi

#
# Configure and build as release.
# Build twice so that if the build fails, the second build will
# contain just the output that shows the errors.
# Catch output and echo to screen.
#
mkdir -p build >> ${ERROR_FILE} 2>&1
cd build >> ${ERROR_FILE} 2>&1
cmake ../src >> ${ERROR_FILE} 2>&1
make -j2 
make -j2 >> ${ERROR_FILE} 2>&1
cat ${ERROR_FILE}

#
# Copy to distribution directory
#
DIST_DIR=/miniserver/caret7_distribution/caret/bin_linux32
/bin/cp -v Desktop/workbench ${DIST_DIR} >> ${ERROR_FILE} 2>&1
/bin/cp -v Command/caret_command ${DIST_DIR} >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
cat ${ERROR_FILE} | mail -s 'Caret7 Linux 32 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu

