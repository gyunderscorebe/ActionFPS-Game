rd /s /q ac
mkdir ac
cd ../../..
git archive -o source/vcpp/buildEnv/ac.zip HEAD
cd source/vcpp/buildEnv/ac/
unzip.exe ../ac.zip
cd source/vcpp
\Windows\Microsoft.NET\Framework\v4.0.30319\msbuild.exe cube.sln /p:Configuration=Release
cd ../launcher
\Windows\Microsoft.NET\Framework\v4.0.30319\msbuild.exe aclauncher.sln /p:Configuration=Release
pause
cd ../vcpp/buildEnv