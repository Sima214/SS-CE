@echo off
setlocal

rem Constants
set "ZIP7_PATH=C:\Program Files\7-Zip\7z.exe"
set "PYTHON_URL=https://www.python.org/ftp/python/3.7.3/python-3.7.3-embed-win32.zip"
set "MINGW_URL=https://d3f3nimbmr2mxp.cloudfront.net/ssce/prebuilt/win/mingw64.7z"
set "CMAKE_URL=https://github.com/Kitware/CMake/releases/download/v3.14.0/cmake-3.14.0-win32-x86.zip"
set "JEMALLOC_URL=https://d3f3nimbmr2mxp.cloudfront.net/ssce/prebuilt/win/jemalloc-x86_64-lto.7z"

rem Test if 7z is working properly.
"%ZIP7_PATH%"
if %ERRORLEVEL% EQU 0 (
    echo 7z is working properly.
) else (
    echo Could not start 7z. Please correct the path.
    pause
    exit /b 1
)

echo Preparing build environment...
rmdir /s /q tools
mkdir tools
mkdir cache
cd cache

if not exist python.zip (
    echo Getting python...
    powershell -Command "Invoke-WebRequest -OutFile python.zip %PYTHON_URL% -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::FireFox"
)

if not exist mingw.7z (
	echo Getting mingw-w64...
	powershell -Command "Invoke-WebRequest -OutFile mingw.7z %MINGW_URL% -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::FireFox"
)

if not exist cmake.zip (
	echo Getting cmake...
	powershell -Command "Invoke-WebRequest -OutFile cmake.zip %CMAKE_URL% -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::FireFox"
)

cd ..\tools

echo Preparing tools...

"%ZIP7_PATH%" x ..\cache\mingw.7z
"%ZIP7_PATH%" x -obin ..\cache\python.zip
"%ZIP7_PATH%" x -o. ..\cache\cmake.zip
xcopy cmake-3.14.0-win32-x86\bin bin\ /s /e
xcopy cmake-3.14.0-win32-x86\doc doc\ /s /e
xcopy cmake-3.14.0-win32-x86\man man\ /s /e
xcopy cmake-3.14.0-win32-x86\share share\ /s /e
rmdir /s /q cmake-3.14.0-win32-x86

cd ..

echo Compiling...

set "PATH=%cd%\tools\bin;%PATH%"
git submodule update --init --recursive
set "PATH=%PATH:C:\Program Files\Git\usr\bin;=%"
set MINGW_PATH=%cd%\tools\bin

powershell -Command "Invoke-WebRequest -OutFile jemalloc-x86_64-lto.7z %JEMALLOC_URL% -UserAgent [Microsoft.PowerShell.Commands.PSUserAgent]::FireFox"
"%ZIP7_PATH%" x jemalloc-x86_64-lto.7z
set "PATH=%CD%\jemalloc\lib;%PATH%"

python .\windows.py --generator "MinGW Makefiles"

pause
