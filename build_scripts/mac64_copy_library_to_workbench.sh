#!/bin/sh

#
# Usage: <library-full-path> <executable-full-path>
#
# This script copies a library to a Workbench Mac App's 
# directory for extra libraries needed by the App's executable.
# The path to the library then needs to be changed
# in the executable from its original path to a path
# relative to the executable.

#
#
# Uncomment to print debug information
#
##debug_flag=1

#
# Input library that is copied for executable
#
#executable_input=/Users/caret/caret7_development/mac64/build/Desktop/wb_view.app/Contents/MacOS/wb_view
#library_input=/opt/local/lib/libomp/libomp.dylib
library_input=$1
executable_input=$2

#
# Extract need paths
#
exe_path_name=${executable_input}
exe_name=`basename ${exe_path_name}`
macos_dir_path=`dirname ${exe_path_name}`
contents_dir_path=`dirname ${macos_dir_path}`
app_name=${exe_name}.app

if [ $debug_flag ]; then
   echo "Executable name: ${exe_name}"
   echo "MacOS directory: ${macos_dir_path}"
   echo "Contents directory: ${contents_dir_path}"
fi

echo "INSTALLING LIBRARY: ${library_input}"
echo "   INTO: ${executable_input}"


#
# Name and path to directory in the App to
# which the library is inserted.
#
workbench_lib_dir_name=WorkbenchLibs
workbench_lib_dir_path_name=${contents_dir_path}/${workbench_lib_dir_name}

#
# Full path and just name of library
#
lib_path_name=${library_input}
lib_name=`basename ${library_input}`

if [ $debug_flag ]; then
   echo "library full path: ${lib_path_name}"
   echo "library name (no path): ${lib_name}"
fi

#exit

#
# Does app's "Content" directory exist?
#
if [ -d "${contents_dir_path}" ]; then
   if [ $debug_flag ]; then
      echo "App Contents found"
   fi

   #
   # Does the Workbench library directory exist in the app's content
   # If not create it
   #
   if [ ! -d "${workbench_lib_dir_path_name}" ]; then
      if [ $debug_flag ]; then
         echo "Making directory ${workbench_lib_dir_path_name}"
      fi
      mkdir ${workbench_lib_dir_path_name}
   fi

   #
   # Verify Workbench library directory exists
   #
   if [ -d "${workbench_lib_dir_path_name}" ]; then
      if [ $debug_flag ]; then
         echo "lib dir exists"
      fi

      #
      # Copy the library to the App's Workbench library directory
      #
      if [ $debug_flag ]; then
         cp -v ${lib_path_name} ${workbench_lib_dir_path_name}
      else
         cp ${lib_path_name} ${workbench_lib_dir_path_name}
      fi
      
      
      #
      # Change the path of the library inside of executable.
      #
      old_name=${lib_path_name}
      new_name="@executable_path/../${workbench_lib_dir_name}/${lib_name}"
      if [ $debug_flag ]; then
         echo "old name: ${old_name}"
         echo "new name: ${new_name}"
      fi
      install_name_tool -change ${old_name} ${new_name} ${exe_path_name}
   else
      echo "ERROR: App Lib Directory with name \""${wb_view_lib_dir}"\" not found"
   fi
else
   echo "ERROR: App Contents Directory with name \""${contents_dir_path}"\" not found"
fi
