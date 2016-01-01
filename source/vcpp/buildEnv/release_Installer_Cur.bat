set exportDir=ac

rem purge old directory
del /S /Q %exportDir%

rem call build processes
call build_1_getfiles.bat
call build_2_prepare.bat
call build_3_makeinstaller.bat

pause
