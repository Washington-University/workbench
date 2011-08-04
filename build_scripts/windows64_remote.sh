#!/bin/sh

ssh -v caret@128.252.37.24 /cygdrive/c/dev7/windows64/caret7_source/build_scripts/windows64.sh $1 > $PWD/remote_launch_windows64.txt 2>&1
