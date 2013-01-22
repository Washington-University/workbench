#!/bin/sh

#
# This file is executed by (via "launchd")
#    /System/Library/LaunchDaemons/edu.wustl.caret7.doxygen.plist
#
# (1) Update source code from repository
# (2) Build doxygen documentation
#

#
# Go to correct directory
#
MAIN_DIR=/mnt/myelin/caret7_documentation
SOURCE_DIR=${MAIN_DIR}/caret7_source
DOCUMENTATION_DIR=${MAIN_DIR}/documentation
BUILD_DIR=${MAIN_DIR}/build

#
# Remove existing documentation directory
#
rm -rf ${DOCUMENTATION_DIR}

#
# Go to the source directory which contains Doxyfile
# Doxyfile is read by doxygen and contains configuration information
#
cd ${SOURCE_DIR}

#
# Update source from repository
#
echo "UPDATING SOURCE FROM GIT REPOSITORY"
git pull -u

#
# Generate doxygen documentation
#
echo "Generating Documentation"
/Applications/Doxygen.app/Contents/Resources/doxygen

echo "SCRIPT COMPLETED SUCCESSFULLY"

