#!/bin/sh

#
# This script copies the qt_menu.nib directory from QT
# into the Mac App's Resources directory.
#

for buildType in Debug/ Release/ RelWithDebInfo/ MinRelSize/ ""
do
   echo "BUILD TYPE ${buildType}"
   if [ -f ${buildType}desktop.app/Contents/MacOS/desktop ] ; then
      if [ ! -d ${buildType}desktop.app/Contents/Resources ] ; then
         mkdir ${buildType}desktop.app/Contents/Resources
      fi
      cp -R ${QTDIR}/src/gui/mac/qt_menu.nib ${buildType}desktop.app/Contents/Resources 
   fi
done


