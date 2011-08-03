@echo off
if [%MACHINE%]==[] call setup_env.bat
set OLD_DIR=%CD%
cd /d %DIR%\caret5_source\windows
call run_qmake_nmake.bat
cd / %OLD_DIR%
unset OLD_DIR