@echo off
if [%MACHINE%]==[] call setup_env.bat
call %DIR%\run_qmake.bat
call %DIR%\run_nmake.bat release
call %DIR%\run_nmaake.bat debug