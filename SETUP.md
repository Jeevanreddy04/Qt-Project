# Setup

Install dependencies, configure, build, and run **Qt + OpenCV Camera Player (QML)**.

## Linux (Ubuntu/Debian)

### 1. Install dependencies

```bash
sudo apt update
sudo apt install -y \
  cmake g++ \
  qt6-base-dev qt6-declarative-dev qt6-tools-dev \
  libopencv-dev
```

### 2. Build

From the project root:

```bash
./scripts/build.sh
```

Or manually:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### 3. Run

```bash
./build/appQtOpenCVCameraQml
```

---

## Windows (MSVC)

### 1. Prerequisites

- **Visual Studio 2022** (or Build Tools) with **Desktop development with C++**
- **Qt 6** (MSVC 2022 64-bit kit) installed via Qt Online Installer
- **OpenCV** for Windows (vc17) extracted, e.g. to `C:\opencv_build\opencv`

### 2. Configure (Qt Creator)

- Open the project (`CMakeLists.txt`) in Qt Creator.
- Select kit **Desktop Qt 6.x MSVC 2022 64-bit**.
- In **Projects → CMake → Additional CMake options**, add:
  ```text
  -DOpenCV_DIR=C:/opencv_build/opencv/build/x64/vc17/lib
  ```
  (Adjust path if your OpenCV location differs.)
- Run CMake (Configure).

### 3. Build and run

**Option A: Using Qt Creator**
- **Build → Build Project**
- **Run**

**Option B: Using command line**
From the project root:

```cmd
scripts\build.bat
```

Or with PowerShell:

```powershell
.\scripts\build.ps1
```

Then run:
```cmd
build\appQtOpenCVCameraQml.exe
```

### 4. OpenCV DLLs at runtime

Add OpenCV `bin` to **PATH**, or copy required DLLs from  
`C:\opencv_build\opencv\build\x64\vc17\bin` next to the built `.exe`.

---

## Project layout

- `CMakeLists.txt` – CMake configuration (Qt6 + OpenCV)
- `src/` – C++ backend (camera, image provider)
- `qml/` – QML UI
- `web/` – Web assets (if used)
- `scripts/` – Build and utility scripts

See [README.md](README.md) for features and notes.
