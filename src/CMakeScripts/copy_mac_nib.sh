#!/bin/sh

#
# This script copies the qt_menu.nib directory from QT
# into the Mac App's Resources directory.
#

exeName=$1

for buildType in Debug/ Release/ RelWithDebInfo/ MinRelSize/ ""
do
   echo "BUILD TYPE ${buildType}"
   appName=${buildType}${exeName}.app/Contents/MacOS/${exeName}
   echo "App ${appName}"

   if [ -f ${appName} ] ; then
      if [ ! -d ${buildType}${exeName}.app/Contents/Resources ] ; then
         echo "Creating resources directory"
         mkdir ${buildType}${exeName}.app/Contents/Resources
      fi

      nib1=${QTDIR}/src/gui/mac/qt_menu.nib
      echo "nib1: ${nib1}"
      if [ -f ${nib1}  ] ; then
         echo "Copying NIB file 1"
         cp -R ${QTDIR}/src/gui/mac/qt_menu.nib ${buildType}${exeName}.app/Contents/Resources 
      fi

      nib2=${QTDIR}/lib/QtGui.framework/Versions/4/Resources/qt_menu.nib
      echo "nib2: ${nib2}"
      if [ -d  ${nib2} ] ; then
         echo "Copying NIB file 2"
         cp -R ${nib2} ${buildType}${exeName}.app/Contents/Resources 
      fi

   fi
done


