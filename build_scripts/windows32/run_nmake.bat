@echo off


if [%MACHINE%]==[] (
call setup_env.bat
)

if [%1]==[] (
set BUILD=clean
)

if NOT [%1]==[] (
set BUILD=%1
)


set OLD_DIR=%CD%
cd /d %DIR%\caret5_source\windows
echo %BUILD%
call run_nmake.bat %BUILD%
cd /d %OLD_DIR%
unset OLD_DIR