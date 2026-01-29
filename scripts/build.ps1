# Build Qt + OpenCV Camera Player (QML) - Windows PowerShell
# Run from project root.

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Resolve-Path (Join-Path $ScriptDir "..")
$BuildDir = Join-Path $ProjectRoot "build"
$BuildType = if ($env:CMAKE_BUILD_TYPE) { $env:CMAKE_BUILD_TYPE } else { "Release" }

Set-Location $ProjectRoot
Write-Host "Building in $BuildDir ($BuildType) ..."

cmake -S . -B "$BuildDir" -DCMAKE_BUILD_TYPE="$BuildType"
if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed." -ForegroundColor Red
    exit 1
}

cmake --build "$BuildDir" -j
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed." -ForegroundColor Red
    exit 1
}

Write-Host "Done. Run: $BuildDir\appQtOpenCVCameraQml.exe" -ForegroundColor Green
