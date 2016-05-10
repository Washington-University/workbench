#!/bin/sh

#
#
#

#
# File for capturing standard error
#
BUILD_ROOT_DIR=/Users/caret/caret7_autobuild
ERROR_FILE=${BUILD_ROOT_DIR}/result_build_mac64_noodle.txt
rm -f ${ERROR_FILE}

#
# If something fails, keep going
#
set noon

#
# Run the build script
#
echo "Starting mac64 noodle aux script" >> ${ERROR_FILE} 2>&1
${BUILD_ROOT_DIR}/caret7_source/build_scripts/mac64_noodle_aux.sh >> ${ERROR_FILE} 2>&1
echo "Finished mac64 noodle aux script" >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
MAIL_ERROR_FILE=${BUILD_ROOT_DIR}/mail_result_build_mac64_noodle.txt
rm -f ${MAIL_ERROR_FILE}
#cat ${ERROR_FILE} | mail -v -s 'Caret7 Mac 64 Noodle Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu >> ${MAIL_ERROR_FILE} 2>&1
cat ${ERROR_FILE} | mail -v -s 'Caret7 Mac 64 Noodle Build Result' john@brainvis.wustl.edu >> ${MAIL_ERROR_FILE} 2>&1

echo "Sent mail"

