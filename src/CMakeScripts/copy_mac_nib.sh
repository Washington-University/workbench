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

      if [ -f  ${QTDIR}/src/gui/mac/qt_menu.nib ] ; then
         cp -R ${QTDIR}/src/gui/mac/qt_menu.nib ${buildType}${exeName}.app/Contents/Resources 
      fi

      if [ -f  ${QTDIR}/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib ] ; then
         cp -R ${QTDIR}/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib ${buildType}${exeName}.app/Contents/Resources 
      fi

   fi
done


