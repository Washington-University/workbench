#!/bin/sh

#
# This script copies the mac icon into
# into the Mac App's Resources directory.
#

exeName=$1
iconName=$2

for buildType in Debug/ Release/ RelWithDebInfo/ MinRelSize/ ""
do
   echo "BUILD TYPE ${buildType}"
   appName=${buildType}${exeName}.app/Contents/MacOS/${exeName}
   echo "App ${appName}"

   if [ -f ${appName} ] ; then
      cp $iconName ${buildType}${exeName}.app/Contents/Resources
         #cp -R ${QTDIR}/src/gui/mac/qt_menu.nib ${buildType}${exeName}.app/Contents/Resources 
   fi
done


