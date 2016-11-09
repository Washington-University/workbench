@echo off
if [%MACHINE%]==[] call setup_env.bat

set OLD_DIR=%CD%
md %DIR%\caret7_source\build
cd /d %DIR%\caret7_source\build
cmake ^
   -DWORKBENCH_FREETYPE_DIR=C:\dev64\install\FreeType-2.4.8 ^
   -DZLIB_ROOT=C:\dev64\install\zlib ^
   -G "NMake Makefiles JOM" ^
   -DCMAKE_BUILD_TYPE=Release ^
   ../src

jom -j8
cd /d %OLD_DIR%
set OLD_DIR=


REM JWH   -DFREETYPE_INCLUDE_DIR_freetype2=C:\dev64\install\FreeType-2.4.8\include\freetype2
REM JWH   -DFREETYPE_INCLUDE_DIR_ft2build=C:\dev64\install\FreeType-2.4.8\include
REM JWH   -DFREETYPE_LIBRARY=C:\dev64\install\FreeType-2.4.8\lib\freetype.lib
