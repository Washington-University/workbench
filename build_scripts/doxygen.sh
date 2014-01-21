#!/bin/sh

#
# This file is executed by (via "launchd")
#    /System/Library/LaunchDaemons/edu.wustl.caret7.doxygen.plist
#

#
# File for capturing standard error
#
BUILD_ROOT_DIR=/mnt/myelin/caret7_documentation
ERROR_FILE=${BUILD_ROOT_DIR}/result_doxygen.txt
rm -f ${ERROR_FILE}

#
# If something fails, keep going
#
set noon

#
# Run the build script
#
echo "Starting doxygen generation" >> ${ERROR_FILE} 2>&1
${BUILD_ROOT_DIR}/caret7_source/build_scripts/doxygen_aux.sh >> ${ERROR_FILE} 2>&1
echo "Finished doxygen generation" >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
cat ${ERROR_FILE} | mail -v -s 'Caret7 Doxygen Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu

echo "Sent mail"

