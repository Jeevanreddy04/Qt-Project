#!/usr/bin/env bash
# Build Qt + OpenCV Camera Player (QML)
# Run from project root.

set -e
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"

cd "$PROJECT_ROOT"
echo "Building in $BUILD_DIR ($BUILD_TYPE) ..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
cmake --build "$BUILD_DIR" -j
echo "Done. Run: $BUILD_DIR/appQtOpenCVCameraQml"
