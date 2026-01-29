cmake_minimum_required(VERSION 3.21)

project(QtOpenCVCameraQml
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick QuickControls2)
find_package(Qt6 6.2 QUIET COMPONENTS Multimedia)
find_package(OpenCV QUIET)

qt_standard_project_setup()

  # Disable D3D12 for MinGW compatibility
set(QT_FEATURE_d3d12 OFF CACHE BOOL "" FORCE)

set(APP_SOURCES
  src/main.cpp
  src/CameraController.h
  src/CameraImageProvider.h
  src/CameraImageProvider.cpp
)

if(OpenCV_FOUND)
  list(APPEND APP_SOURCES src/CameraController.cpp)
elseif(Qt6Multimedia_FOUND)
  list(APPEND APP_SOURCES src/CameraController_qtmm.cpp)
  set(HAS_CAMERA_BACKEND TRUE)
else()
  list(APPEND APP_SOURCES src/CameraController_minimal.cpp)
endif()

qt_add_executable(appQtOpenCVCameraQml ${APP_SOURCES})

qt_add_qml_module(appQtOpenCVCameraQml
  URI CameraPlayer
  VERSION 1.0
  QML_FILES
    qml/Main.qml
)

target_link_libraries(appQtOpenCVCameraQml
  PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
)
if(OpenCV_FOUND)
  target_compile_definitions(appQtOpenCVCameraQml PRIVATE CAMERA_HAS_OPENCV=1)
  target_include_directories(appQtOpenCVCameraQml PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(appQtOpenCVCameraQml PRIVATE ${OpenCV_LIBS})
elseif(Qt6Multimedia_FOUND)
  target_compile_definitions(appQtOpenCVCameraQml PRIVATE CAMERA_HAS_QT_MULTIMEDIA=1)
  target_link_libraries(appQtOpenCVCameraQml PRIVATE Qt6::Multimedia)
endif()

