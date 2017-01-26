IF EXIST "%programfiles%\NSIS\makensis.exe" (
	"%programfiles%\NSIS\makensis.exe" af.nsi
) ELSE (
	"%programfiles(x86)%\NSIS\makensis.exe" af.nsi
)
