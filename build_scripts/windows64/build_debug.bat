@echo off
if [%MACHINE%]==[] call setup_env.bat

set OLD_DIR=%CD%
REM rd /q/s %DIR%\caret7_source\build_debug
md %DIR%\caret7_source\build_debug
cd /d %DIR%\caret7_source\build_debug
cmake -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Debug ../src
jom -j8
cd /d %OLD_DIR%
set OLD_DIR=
