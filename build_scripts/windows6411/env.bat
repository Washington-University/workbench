@echo off
rem isn't batch awesome, no else statments...
IF [%1]==[] (
	set DIR "."
)
IF NOT [%1]==[] (
set DIR=%1
)
IF [%2]==[] (
	set MACHINE X86
)
IF NOT [%2]==[] (
set MACHINE=%2
)

IF NOT [%INITVS%]==[%MACHINE%] (
	echo "Setting %MACHINE% bit compiler variables"
	set INITVS=%MACHINE%
	call "c:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\vcvarsall.bat" %MACHINE%
)