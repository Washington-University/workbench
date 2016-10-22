#!/bin/sh

ssh -v caret@winbuild 'c:\dev7\windows64\caret7_source\build_scripts\windows64.bat' > remote_launch_windows64.txt 2>&1
cat remote_launch_windows64.txt | mailx -s 'Caret7 Windows 64 Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu
