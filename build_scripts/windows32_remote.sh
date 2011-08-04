#!/bin/sh

ssh -v caret@128.252.37.24 /cygdrive/c/dev7/windows32/caret7_source/build_scripts/windows32.sh $1 > $PWD/remote_launch_windows32.txt 2>&1
