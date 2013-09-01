Set objShell = WScript.CreateObject("WScript.Shell")
Set colUsrEnvVars = objShell.Environment("USER")

QtPath = "C:\dev6411\install\Qt\bin"

Path = colUsrEnvVars("OLDPATH")

If Len(Path) = 0 Then
	colUsrEnvVars("OLDPATH") = colUsrEnvVars("PATH")
    'objShell.ExpandEnvironmentStrings("%PATH%")
	WScript.Echo colUsrEnvVars("OLDPATH")
    Path = colUsrEnvVars("OLDPATH")
End If

NewPath =  (QtPath & ";" & Path)
colUsrEnvVars("PATH") = (NewPath)

colUsrEnvVars("QTDIR") = "C:\dev6411\install\Qt"

colUsrEnvVars("ZLIB_LIB_DIR") = "C:\dev6411\install\zlib\lib"
colUsrEnvVars("ZLIB_INC_DIR") = "C:\dev6411\install\zlib\include"