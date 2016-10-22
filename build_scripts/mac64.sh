#!/bin/sh

#
# This file is executed by (via "launchd")
#    /System/Library/LaunchDaemons/edu.wustl.caret.build.mac64.plist
#
# (1) Update source code from repository
# (2) Build Caret programs
# (3) Copy executables into the distribution
#

#
# File for capturing standard error
#
BUILD_ROOT_DIR=/Users/caret/caret7_development/mac64

#
# If something fails, keep going
#
set noon

#
# Run the build script
#
echo "Starting mac64 aux script"
${BUILD_ROOT_DIR}/caret7_source/build_scripts/mac64_aux.sh
echo "Finished mac64 aux script"

