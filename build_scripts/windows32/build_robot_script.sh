#!/bin/sh

export CMD=/cygdrive/c/Windows/system32/cmd.exe
#
# Go to correct directory
#
BUILD_DIR=/cygdrive/c/dev/windows32
cd ${BUILD_DIR}

#
# Go into source directory
#
cd caret5_source

#
# File for capturing standard error
#
ERROR_FILE=${BUILD_DIR}/result_build_windows32.txt
rm -f ${ERROR_FILE}
echo "Windows32 Build Result" > ${ERROR_FILE}
#
# Grab the latest sources
#
git pull -u

#
# Force updating __DATE__ that shows up in title bar
#
if [ "$1" == "FORCE_UPDATE" ]
then
echo "Update Forced" >> ${ERROR_FILE} 2>&1
rm -f caret/release/GuiMainWindow.obj caret_command_operations/release/CommandVersion.obj caret_edit/release/main.obj >> ${ERROR_FILE} 2>&1
fi

#
# Build caret
#
cd ..
$CMD /c run.bat >> ${ERROR_FILE} 2>&1

cd caret5_source
DIST_DIR=caret@hippocampus:/Volumes/DS4600/caret_distribution/caret/bin_windows32

echo "Copying Files" >> ${ERROR_FILE}
scp -v caret/release/caret5.exe $DIST_DIR >> ${ERROR_FILE} 2>&1
scp -v caret_command/release/caret_command.exe $DIST_DIR >> ${ERROR_FILE} 2>&1
scp -v caret_edit/release/caret_edit.exe $DIST_DIR >> ${ERROR_FILE} 2>&1
echo "Finished Copying Files" >> ${ERROR_FILE}
scp -r ${ERROR_FILE} caret@hippocampus:/Volumes/DS4600/caret_development/windows32
#
# Send output as email
#
cat ${ERROR_FILE} | email -n caret -f 'caret@jonsdev-win.wustl.edu' -s 'Windows 32 Build Result' john@brainvis.wustl.edu,jschindl@brainvis.wustl.edu,tsc5yc@mst.edu

#
# Dump output to terminal
#
cat ${ERROR_FILE}


