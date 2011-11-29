@echo=off
set CMD=c:\Windows\system32\cmd.exe
REM
REM Go to correct directory
REM
set BUILD_DIR=c:\dev7\windows32
cd %BUILD_DIR%

REM
REM Go into source directory
REM
cd caret7_source

echo "Caret7 Windows32 Build Result"

REM
REM Grab the latest Sources
REM
c:\cygwin\bin\git.exe pull -u

REM
REM Build caret
REM
cd build_scripts/windows32
%CMD% /c build.bat
cd ..
cd ..
set DIST_DIR=caret@hippocampus:/Volumes/DS4600/caret7_distribution/caret/bin_windows32

echo "Copying Files"
c:\cygwin\bin\scp build/Desktop/workbench.exe %DIST_DIR%
c:\cygwin\bin\scp build/CommandLine/caret_command.exe %DIST_DIR%
echo "Finished Copying Files"
