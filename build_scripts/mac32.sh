#!/bin/sh

#
# This file is executed by (via "launchd")
#    /System/Library/LaunchDaemons/edu.wustl.caret.build.mac32.plist
#
# (1) Update source code from repository
# (2) Build Caret programs
# (3) Copy executables into the distribution
#

#
# File for capturing standard error
#
BUILD_ROOT_DIR=/Users/caret/caret7_development/mac32
ERROR_FILE=${BUILD_ROOT_DIR}/result_build_mac32.txt
rm -f ${ERROR_FILE}

#
# If something fails, keep going
#
set noon

#
# Run the build script
#
echo "Starting mac32 aux script" >> ${ERROR_FILE} 2>&1
${BUILD_ROOT_DIR}/caret7_source/build_scripts/mac32_aux.sh >> ${ERROR_FILE} 2>&1
echo "Finished mac32 aux script" >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
MAIL_ERROR_FILE=${BUILD_ROOT_DIR}/mail_result_build_mac32.txt
rm -f ${MAIL_ERROR_FILE}
cat ${ERROR_FILE} | mail -v -s 'Caret7 Mac 32 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu >> ${MAIL_ERROR_FILE} 2>&1

echo "Sent mail"

