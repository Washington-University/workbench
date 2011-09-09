#!/bin/sh

#
# This script copies the qt_menu.nib directory from QT
# into the Mac App's Resources directory.
#

exeName=$1

for buildType in Debug/ Release/ RelWithDebInfo/ MinRelSize/ ""
do
   echo "BUILD TYPE ${buildType}"
   if [ -f ${buildType}${exeName}.app/Contents/MacOS/${exeName} ] ; then
      if [ ! -d ${buildType}${exeName}.app/Contents/Resources ] ; then
         mkdir ${buildType}${exeName}.app/Contents/Resources
      fi
      cp -R ${QTDIR}/src/gui/mac/qt_menu.nib ${buildType}${exeName}.app/Contents/Resources 
   fi
done


