@echo off
setlocal

set buildFile=%~dp0Build\build.build
set msbuildVersion=15.0

set vswherecmd="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath
for /F "tokens=* usebackq" %%i in (`%vswherecmd%`) do set vsInstallationPath=%%i
if "%vsInstallationPath%" equ "" echo Visual studio not found & goto :eof

set msb="%vsInstallationPath%\MSBuild\%msbuildVersion%\Bin\MSBuild.exe"
if not exist %msb% echo MSBuild not found at %msb% & goto :eof

set args=%*
if "%1" NEQ "" set args=/t:%*

%msb% /p:vsInstallationPath="%vsInstallationPath%" %buildFile% %args%