# Project verification

Checklist to verify **Qt + OpenCV Camera Player (QML)** builds and runs correctly.

## Build

- [ ] **Linux:** `./scripts/build.sh` or `cmake -S . -B build && cmake --build build -j` completes without errors
- [ ] **Windows (Qt Creator):** Configure with MSVC kit + `OpenCV_DIR` → Build Project succeeds

## Run

- [ ] Application starts (window opens)
- [ ] **Play** starts camera feed; video area shows live frames
- [ ] **Pause** freezes the current frame
- [ ] **Resume** continues from paused state
- [ ] **Rewind / Forward** (when paused) step through buffered frames
- [ ] **Resolution** combo changes resolution (best-effort; camera may clamp)
- [ ] **Step frames** and **Buffer (sec)** affect rewind/forward behavior
- [ ] Error dialog appears if camera is unavailable or frame read fails

## Optional

- [ ] Build with OpenCV: camera works via OpenCV backend
- [ ] Build without OpenCV but with Qt Multimedia: camera works via Qt Multimedia backend (if available)
- [ ] Build without OpenCV and without Qt Multimedia: app runs; Play shows “Camera support requires OpenCV library” (or similar) error

## Environment

- [ ] **Linux:** Qt 6, OpenCV (or Qt Multimedia), C++17 compiler installed
- [ ] **Windows:** Qt 6 MSVC kit, OpenCV vc17, and OpenCV DLLs on PATH or next to `.exe`

If any step fails, see [SETUP.md](SETUP.md) and [README.md](README.md).
