@echo off
if [%MACHINE%]==[] call setup_env.bat
call run_qmake.bat
call run_nmake.bat release