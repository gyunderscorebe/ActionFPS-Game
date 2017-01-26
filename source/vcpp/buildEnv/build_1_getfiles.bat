set tmp_ac=%TEMP%\af
rd /s /q %tmp_ac%
mkdir %tmp_ac%
rd /s /q ac
echo D | xcopy ..\..\.. %tmp_ac% /s /e /q
rd /s /q %tmp_ac%\.git
echo D | xcopy %tmp_ac% ac /s /e /y /q
cd ac
cd source/vcpp/buildEnv/ac/
cd source/vcpp
\Windows\Microsoft.NET\Framework\v4.0.30319\msbuild.exe cube.sln /p:Configuration=Release
cd ../launcher
\Windows\Microsoft.NET\Framework\v4.0.30319\msbuild.exe aclauncher.sln /p:Configuration=Release
cd ../vcpp/buildEnv
