#!/bin/sh

rcc -project -o resources.qrc

echo "You will need to delete the Desktop and CommandLine projects in the build directories, run cmake, and then compile."
