#!/bin/sh

#
# This script copies adds the frameworks to the Mac Bundles
#

exeName=$1

for buildType in Debug/ Release/ RelWithDebInfo/ MinRelSize/ ""
do
   echo "BUILD TYPE ${buildType}"
   appName=${buildType}${exeName}.app
   exeName=${appName}/Contents/MacOS/${exeName}
   echo "App ${appName}"
   echo "Exe ${exeName}"

   if [ -f ${exeName} ] ; then
      if [ ! -d ${appName}/Contents/Frameworks ] ; then
          macdeployqt ${appName}
      fi
   fi
done


