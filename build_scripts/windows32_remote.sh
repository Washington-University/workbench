#!/bin/sh

ssh -v caret@128.252.37.24 'c:\dev7\windows32\caret7_source\build_scripts\windows32.bat' > $PWD/remote_launch_windows32.txt 2>&1
cat $PWD/remote_launch_windows32.txt | mail -s 'Caret7 Windows 32 Build Result' john@brainvis.wustl.edu jschindl@brainvis.wustl.edu tsc5yc@mst.edu