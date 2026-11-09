@echo off
rem Builds C++31GBM_FullScreen.exe and C++31GBM_Windowed.exe (Release, Win32).
setlocal
cd /d "%~dp0"

set "PATH=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer;%PATH%"
for /f "delims=" %%i in ('vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe') do set "MSBUILD=%%i"
if not defined MSBUILD (
    echo MSBuild was not found.
    exit /b 1
)

call :build FullScreen BUILD_FULLSCREEN || exit /b 1
call :build Windowed BUILD_WINDOWED || exit /b 1

echo.
echo Done: C++31GBM_FullScreen.exe / C++31GBM_Windowed.exe
exit /b 0

:build
echo.
echo === %1 ===
"%MSBUILD%" C++31GBM.vcxproj -t:Build -p:Configuration=Release -p:Platform=Win32 -p:TargetName=C++31GBM_%1 -p:OutDir=.\ -p:IntDir=Release\%1\ -p:WindowModeDefine=%2 -m -v:m -nologo
exit /b %errorlevel%
