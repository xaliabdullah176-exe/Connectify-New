# BUILD_AND_RUN.md — Setup, Build & Launch Guide

This document explains how to build and run the ConnectifyUI application from scratch on Windows.

---

## Prerequisites

| Tool | Version | Where to Get |
|---|---|---|
| Qt | 6.5.3+ (MinGW 64-bit) | https://www.qt.io/download |
| CMake | 3.23+ | https://cmake.org/download |
| Ninja | Any recent | Bundled with Qt installer or https://ninja-build.org |
| MinGW (g++) | 13.x (mingw1310_64) | Bundled with Qt installer |

> **Note:** The project also supports MSVC (Visual Studio 18 2026) if you prefer the MSVC toolchain. See the MSVC preset below.

---

## Option A — One-Click Launch (Recommended)

The project ships with `run.ps1` — a PowerShell script that configures, builds, deploys Qt DLLs, and launches the app in a single command.

```powershell
# Open PowerShell in C:\CONNECT and run:
.\run.ps1
```

The script:
1. Auto-detects your Qt installation path under `C:\Qt`
2. Sets `PATH` to include Qt binaries, MinGW, and Ninja
3. Runs `cmake -S . -B build-ps-debug -G "Ninja" -DCMAKE_BUILD_TYPE=Debug`
4. Runs `cmake --build build-ps-debug --parallel`
5. Runs `windeployqt.exe` on the first run to copy Qt DLLs next to the .exe
6. Launches `build-ps-debug\ConnectifyUI.exe`

> Qt DLL deployment only runs once (tracked by a `.dlls_deployed` marker file). Subsequent runs skip this step for speed.

---

## Option B — Manual CMake Build (Ninja preset)

```powershell
# 1. Set Qt path
$env:QTDIR = "C:\Qt\6.5.3\mingw_64"
$env:PATH   = "$env:QTDIR\bin;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\Tools\Ninja;" + $env:PATH

# 2. Configure
cmake --preset qt-debug

# 3. Build
cmake --build --preset qt-debug

# 4. Deploy Qt DLLs (first time only)
windeployqt.exe build-qt-debug\ConnectifyUI.exe

# 5. Run
.\build-qt-debug\ConnectifyUI.exe
```

---

## Option C — MSVC / Visual Studio

```powershell
# 1. Open "x64 Native Tools Command Prompt for VS 2022"
# 2. Set Qt path
set QTDIR=C:\Qt\6.5.3\msvc2019_64

# 3. Configure with MSVC preset
cmake --preset msvc-debug

# 4. Build
cmake --build --preset msvc-debug

# 5. Deploy and run
windeployqt.exe build-msvc-debug\Debug\ConnectifyUI.exe
.\build-msvc-debug\Debug\ConnectifyUI.exe
```

---

## Build Presets Reference

Defined in `CMakePresets.json`:

| Preset Name | Display Name | Generator | Output Directory |
|---|---|---|---|
| `qt-debug` | Qt Debug with Ninja (portable) | Ninja | `build-qt-debug/` |
| `qt-mingw-debug` | Qt MinGW Debug | MinGW Makefiles | `build-mingw-debug/` |
| `msvc-debug` | MSVC Debug (VS 18 2026) | Visual Studio 18 2026 | `build-msvc-debug/` |

All presets set `CMAKE_BUILD_TYPE=Debug` and look for Qt in `$env:QTDIR`, `$env:Qt6_DIR`, or `C:/msys64/ucrt64`.

---

## Project Structure Reference

```
C:\CONNECT\
├── backend/
│   ├── include/
│   │   └── user.h              ← all class declarations
│   ├── src/
│   │   ├── message.cpp         ← MessageSystem implementation
│   │   ├── notification.cpp    ← NotificationSystem implementation
│   │   ├── post.cpp            ← Post operations
│   │   ├── search.cpp          ← searchUsers / searchPosts
│   │   └── user.cpp            ← User, globals, persistence (saveData/loadData)
│   └── main.cpp                ← Console-mode entry point
├── frontend/
│   ├── integration/
│   │   └── mainwindow_integration.h   ← wiring guide for all pages
│   ├── pages/
│   │   ├── LoginPage.*         ← Login screen
│   │   ├── signuppage.*        ← Registration screen
│   │   ├── mainwindow.*        ← Root window + QStackedWidget
│   │   ├── newsfeedpage.*      ← Post feed
│   │   ├── profilepage.*       ← User profile
│   │   ├── messagepage.*       ← Inbox + chat
│   │   ├── searchpage.*        ← Search
│   │   ├── notificationpage.*  ← Notifications
│   │   └── admindashboard.*    ← Admin panel
│   ├── styles/
│   │   └── style.qss           ← Qt stylesheet (dark theme)
│   └── main.cpp                ← Qt GUI entry point (creates QApplication + MainWindow)
├── data.json                   ← persistent storage
├── notifications.txt           ← notification records
├── CMakeLists.txt              ← build definition
├── CMakePresets.json           ← preset definitions
└── run.ps1                     ← one-click build + launch script
```

---

## CMakeLists.txt — Key Sections

```cmake
cmake_minimum_required(VERSION 3.23)
project(ConnectifyUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

# Auto-discover all source files
file(GLOB_RECURSE PROJECT_SOURCES
    "backend/src/*.cpp"
    "frontend/pages/*.cpp"
    "frontend/pages/*.ui"
    "frontend/main.cpp"
)

# Build the executable
qt_add_executable(ConnectifyUI MANUAL_FINALIZATION ${PROJECT_SOURCES})

# Include directories (also passed to MOC)
target_include_directories(ConnectifyUI PRIVATE
    backend/include
    backend
    frontend/pages
    frontend/integration
)

# Link Qt Widgets
target_link_libraries(ConnectifyUI PRIVATE Qt6::Widgets)
```

---

## Troubleshooting

| Problem | Likely Cause | Fix |
|---|---|---|
| `Qt not found` | `QTDIR` env var not set | Set `$env:QTDIR = "C:\Qt\6.5.3\mingw_64"` before running cmake |
| `uic: command not found` | Qt bin not in PATH | Add `C:\Qt\6.5.3\mingw_64\bin` to PATH |
| `moc: fatal error ... user.h` | MOC can't find include | Ensure `AUTOMOC_MOC_OPTIONS` in CMakeLists.txt includes backend/include |
| App launches but crashes | Missing Qt DLLs | Run `windeployqt.exe ConnectifyUI.exe` in the build directory |
| `data.json` parse error | Corrupted save file | Delete `data.json` — a fresh one is created on next launch |
| `notifications.txt` error | Missing file | The file is created automatically on first notification |

---

## Console-Mode Backend (Optional)

The backend can also be run in pure console mode using `backend/main.cpp`. This mode does not require Qt and exercises all the OOP logic independently.

To build console-only (manual):

```bash
g++ -std=c++17 backend/main.cpp backend/src/*.cpp -Ibackend/include -o connect_console
./connect_console
```

This is useful for testing backend logic without the Qt dependency.
