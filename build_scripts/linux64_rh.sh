#!/bin/sh

#
# This file is executed by myelin1.
#    
#
# (1) Update source code from repository
# (2) Build Caret programs
# (3) Copy executables into the distribution
#

#
# File for capturing standard error
#
BUILD_ROOT_DIR=/home/caret/caret7_dev
ERROR_FILE=${BUILD_ROOT_DIR}/result_build_linux64_rh.txt
rm -f ${ERROR_FILE}

#
# If something fails, keep going
#
set noon

#
# Run the build script
#
echo "Starting linux64 Redhat aux script" >> ${ERROR_FILE} 2>&1
${BUILD_ROOT_DIR}/caret7_source/build_scripts/linux64_rh_aux.sh >> ${ERROR_FILE} 2>&1
echo "Finished linux64 Redhat script" >> ${ERROR_FILE} 2>&1

#
# Send output as email
#
MAIL_ERROR_FILE=${BUILD_ROOT_DIR}/mail_result_build_linux64_rh.txt
rm -f ${MAIL_ERROR_FILE}
cat ${ERROR_FILE} | mail -v -s 'Caret7 Linux 64 Redhat Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu >> ${MAIL_ERROR_FILE} 2>&1

echo "Sent mail"

