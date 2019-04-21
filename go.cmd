@echo off & cls & setlocal

set vsVersion=(16^^^^,17]
set buildFile=%~dp0Build\build.build

set requires=Microsoft.Component.MSBuild Microsoft.VisualStudio.Component.VC.Tools.x86.x64
set vsWhereBaseCmd="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -version %vsVersion% -requires %requires% 

for /F "tokens=* usebackq delims=" %%i in (`%vsWhereBaseCmd% -property installationPath`) do set vsInstallationPath=%%i
if "%vsInstallationPath%" equ "" echo Visual studio not found & exit /b 1
echo vsInstallationPath=%vsInstallationPath%

for /F "tokens=* usebackq delims=" %%i in (`%vsWhereBaseCmd% -find MSBuild\**\Bin\amd64\MSBuild.exe`) do set msbuild=%%i
if "%msbuild%" equ "" echo msbuild not found & exit /b 1
echo msbuild=%msbuild%

set args=%*
if "%1" NEQ "" set args=/t:%*

call "%vsInstallationPath%\VC\Auxiliary\Build\vcvars64.bat"
"%msbuild%" /p:vsInstallationPath="%vsInstallationPath%" %buildFile% %args%