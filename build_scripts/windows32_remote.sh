#!/bin/sh

ssh -v caret@winbuild 'c:\dev7\windows32\caret7_source\build_scripts\windows32.bat' > $PWD/remote_launch_windows32.txt 2>&1
cat $PWD/remote_launch_windows32.txt | mailx -s 'Caret7 Windows 32 Build Result' john@brainvis.wustl.edu tsc5yc@mst.edu
