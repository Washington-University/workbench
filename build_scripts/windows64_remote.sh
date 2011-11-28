#!/bin/sh

ssh -v caret@128.252.37.24 'c:\dev7\windows64\caret7_source\build_scripts\windows64.bat' > remote_launch_windows64.txt
cat remote_launch_windows64.txt | mail -s 'Caret7 Windows 64 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu
