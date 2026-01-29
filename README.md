# Qt + OpenCV Camera Player (QML + C++)

Qt Quick (QML) UI with a C++ backend that captures a **live camera feed** using **OpenCV** and provides:

- Play / Pause / Resume
- Forward / Rewind (works while paused using a frame ring-buffer)
- Resolution selection (e.g. 640x480, 1280x720, 1920x1080)
- Basic error dialogs if the camera can’t be opened or frames can’t be read
## Screenshots

### Main Application Window
![Application UI](outputs/imgs/app-ui.png)

### Camera Feed with Controls
![Camera Feed](outputs/imgs/camera-feed.png)

## Demo Videos

### Application Demo
![Demo Video](outputs/videos/demo.mp4)
## Requirements (Linux)

- CMake (>= 3.21)
- A C++17 compiler (GCC/Clang)
- Qt 6 (Quick + QuickControls2)
- OpenCV

### Ubuntu/Debian example

```bash
sudo apt update
sudo apt install -y \
  cmake g++ \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev \
  libopencv-dev
```

## Build

From the project root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run

```bash
./build/appQtOpenCVCameraQml
```

## Notes

- **Pause** freezes the current frame (capture timer stops).
- **Rewind/Forward** are enabled only while paused, and navigate inside the buffered frames.
- Changing **Resolution** is best-effort: webcams may clamp to the nearest supported mode.

## Project Layout

- `CMakeLists.txt`: CMake build configuration (Qt6 + OpenCV)
- `src/`: C++ backend (OpenCV capture + ring buffer + image provider)
- `qml/`: QML UI

