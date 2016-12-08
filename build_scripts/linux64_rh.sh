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

#
# If something fails, keep going
#
set noon

#
# Run the build script
#
echo "Starting linux64 Redhat aux script"
${BUILD_ROOT_DIR}/caret7_source/build_scripts/linux64_rh_aux.sh
resultFlag=$?
echo "Finished linux64 Redhat script"

exit $resultFlag
