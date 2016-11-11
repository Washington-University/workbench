@echo off
if [%MACHINE%]==[] call setup_env.bat

set OLD_DIR=%CD%
REM rd /q/s %DIR%\caret7_source\build
md %DIR%\caret7_source\build
cd /d %DIR%\caret7_source\build
REMcmake -G "NMake Makefiles JOM" -DCMAKE_BUILD_TYPE=Release ../src 

REM   -DFREETYPE_INCLUDE_DIR_freetype2=C:\dev32\install\FreeType-2.4.8\include\freetype2 
REM   -DFREETYPE_INCLUDE_DIR_ft2build=C:\dev32\install\FreeType-2.4.8\include
REM   -DFREETYPE_LIBRARY=C:\dev32\install\FreeType-2.4.8\lib\freetype.lib


cmake ^
   -DWORKBENCH_FREETYPE_DIR=C:\dev32\install\FreeType-2.4.8\lib ^
   -DZLIB_ROOT=C:\dev32\install\zlib ^
    -G "NMake Makefiles JOM" ^
    -DCMAKE_BUILD_TYPE=Release ^
    ../src

jom -j8
cd /d %OLD_DIR%
set OLD_DIR=

