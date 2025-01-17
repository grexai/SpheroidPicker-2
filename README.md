# Automatic SpheroidPicker

This project is a Qt-based application configured to use multiple libraries and dependencies. Below are the requirements and setup instructions.

## Requirements

### General
- **Qt Framework**: Includes modules like `core`, `gui`, `widgets`, `serialport`, and `xml`.
- **C++ Compiler**: Must support C++11 or later.
- **OpenCV**: Version 4.5.5 is used for image processing.
- **Leica SDK**: Includes specific libraries for Leica hardware integration.
- **TensorFlow**: TensorFlow GPU version 2.7.0 for deep learning features.
- **QuaZIP**: For handling zip files, configured as a static library.

### Operating Systems
- **Windows**: Tested with Windows configurations.

### Libraries and Dependencies
#### Qt Modules
- Core functionality: `core`, `gui`, `widgets`.
- Additional modules: `serialport`, `xml`.

#### OpenCV
- Libraries:
  - `opencv_core455`
  - `opencv_imgproc455`
  - `opencv_highgui455`
  - `opencv_imgcodecs455`
  - `opencv_videoio455`
  - `opencv_ml455`
  - `opencv_dnn455`
- Include Path: `../../libraries/opencv-4.5.5/opencv-4.5.5/build/install/include`
- Library Path: `../../libraries/opencv-4.5.5/opencv-4.5.5/build/lib/Release`

#### Leica SDK
- Include Path: `../../libraries/Leica sdk/C++/include`
- Library Path: `../../libraries/Leica sdk/C++/lib64`
- Library: `ahmcore`

#### TensorFlow
- Version: TensorFlow GPU 2.7.0
- Download at;
- Include Path: `../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0/include`
- Library Path: `../../libraries/libtensorflow-gpu-windows-x86_64-2.7.0/lib`
- Library: `tensorflow.lib` (Windows) or `tensorflow.a` (Unix)

#### QuaZIP
- Configured as a static library (`QUAZIP_STATIC`).

## Build Configuration

### Compiler Flags
- Enable C++11: `CONFIG += c++11`
- Enable Qt deprecated warnings: `DEFINES += QT_DEPRECATED_WARNINGS`
- Optional: Disable deprecated APIs with a specific version: `DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000`

### File Structure
- **Source Files**: `main.cpp`, `mainwindow.cpp`, `mosaicworker.cpp`, etc.
- **Header Files**: `mainwindow.h`, `serialcom.h`, `stagecontroller.h`, etc.
- **UI Forms**: `mainwindow.ui`, `calibratedialog.ui`, `hardwareselector.ui`, etc.

### Deployment
- **Windows**: Libraries and dependencies are configured for Windows with `win32` conditions.


## Installation and Setup

1. Install the required Qt framework and ensure `qmake` is available.
2. Install OpenCV 4.5.5 and ensure the include and library paths are configured correctly.
3. Install the Leica SDK and verify the paths in the project file.
4. For TensorFlow:
   - Ensure CUDA and cuDNN are installed for GPU support.
   - Add TensorFlow include and library paths as specified.
5. Install QuaZIP and configure it as a static library.

## Build Instructions

1. Open the project in Qt Creator or configure with `qmake`.
2. Run the following commands:
   ```bash
   qmake
   make