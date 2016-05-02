#!/bin/sh

mv HelpFiles OLD_HelpFiles

cp -r /mnt/myelin/shared/WB_Tutorial/WB_1.2_Help ./HelpFiles

rm -rf OLD_HelpFiles

rcc -project -o resources.qrc

echo ""
echo "You now need to:"
echo "   1) Run CMake in the build directory"
echo "   2) Compile"
echo "   3) Commit changes to GIT repository"
echo "   4) Push source code to master repository"
echo ""
