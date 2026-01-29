@echo off
REM Build Qt + OpenCV Camera Player (QML) - Windows
REM Run from project root.

setlocal enabledelayedexpansion

set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "BUILD_DIR=%PROJECT_ROOT%\build"
set "BUILD_TYPE=%CMAKE_BUILD_TYPE%"
if "%BUILD_TYPE%"=="" set "BUILD_TYPE=Release"

cd /d "%PROJECT_ROOT%"
echo Building in %BUILD_DIR% (%BUILD_TYPE%) ...

cmake -S . -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo CMake configuration failed.
    exit /b 1
)

cmake --build "%BUILD_DIR%" -j
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

echo Done. Run: %BUILD_DIR%\appQtOpenCVCameraQml.exe
