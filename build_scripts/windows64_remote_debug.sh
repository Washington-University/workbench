#!/bin/sh

ssh -v caret@winbuild 'c:\dev7\windows64\caret7_source\build_scripts\windows64_debug.bat' > remote_launch_windows64_debug.txt 2>&1
cat remote_launch_windows64_debug.txt | mailx -s 'Caret7 Windows 64 DEBUG Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu
