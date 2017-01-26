@echo on
set acdir=ac
set workingacdir=..\..\..\

rem delete config
del %acdir%\config\saved.cfg
echo "" > %acdir%\config\servers.cfg

rem purge screenshots
del /Q %acdir%\screenshots\*

rem purge demo directory
del /Q %acdir%\demos\*

rem remove linux stuff
del %acdir%\*.sh
del %acdir%\config\*.sh
rmdir /S /Q %acdir%\bin_unix

rem remove source files (those are available in the source pkg)
rmdir /S /Q %acdir%\source

rem use actionfps_release as launcher
IF EXIST %acdir%\actionfps_release.bat (
del %acdir%\actionfps.bat
ren %acdir%\actionfps_release.bat actionfps.bat
)

copy /Y  %workingacdir%\source\vcpp\buildEnv\icon.ico %acdir%\icon.ico
