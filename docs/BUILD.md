# CONNECT — Build & Run Documentation

> **Project:** ConnectifyUI — A Qt-based social media desktop application  
> **Language:** C++17 | **UI Framework:** Qt 5 or Qt 6 (Widgets)  
> **Build System:** CMake ≥ 3.16 + Ninja or MinGW Makefiles

---

## Table of Contents

1. [Prerequisites](#1-prerequisites)
2. [Environment Setup](#2-environment-setup)
3. [Clone / Verify the Project](#3-clone--verify-the-project)
4. [Build Methods](#4-build-methods)
   - [Option A — Qt Creator (Recommended for beginners)](#option-a--qt-creator-recommended)
   - [Option B — Command Line with CMake Presets (Ninja)](#option-b--command-line-cmake-presets-ninja)
   - [Option C — MinGW Makefiles](#option-c--mingw-makefiles)
5. [Running the Application](#5-running-the-application)
6. [Common Errors & Fixes](#6-common-errors--fixes)
7. [Project Structure Reference](#7-project-structure-reference)

---

## 1. Prerequisites

Install **all** of the following before building.

### 1.1 Qt Framework

Download from [https://www.qt.io/download-open-source](https://www.qt.io/download-open-source)

During the Qt installer, select:

- **Qt 6.x** → `Qt 6.x.x > MinGW 64-bit` (or MSVC if you prefer Visual Studio)
- **OR Qt 5.15** if Qt 6 is unavailable
- Under _Developer Tools_: check **CMake** and **Ninja**

After installation, note your Qt path, e.g.:

```
C:\Qt\6.5.3\mingw_64
```

### 1.2 MinGW (via MSYS2) — if not using MSVC

The CMakeLists.txt has a fallback path `C:/msys64/ucrt64`. Install MSYS2 from [https://www.msys2.org](https://www.msys2.org), then inside the MSYS2 terminal run:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
```

### 1.3 CMake ≥ 3.16

Verify your CMake version:

```cmd
cmake --version
```

If it says `3.15` or lower, download a newer release from [https://cmake.org/download](https://cmake.org/download).

---

## 2. Environment Setup

CMake discovers Qt through environment variables. Set **one** of the following:

### Option A — Set `QTDIR` (simplest)

```cmd
:: Replace with your actual Qt path
set QTDIR=C:\Qt\6.5.3\mingw_64
```

To make it permanent (Windows):

1. Open **System Properties → Advanced → Environment Variables**
2. Under _User variables_, click **New**
   - Name: `QTDIR`
   - Value: `C:\Qt\6.5.3\mingw_64`

### Option B — Set `Qt6_DIR`

```cmd
set Qt6_DIR=C:\Qt\6.5.3\mingw_64\lib\cmake\Qt6
```

### Option C — Use MSYS2 default path

If Qt was installed via MSYS2 to `C:\msys64\ucrt64`, no environment variable is needed — the CMakeLists.txt already includes that path as a fallback.

---

## 3. Clone / Verify the Project

```cmd
cd C:\
git clone <your-repo-url> CONNECT
cd CONNECT
```

Confirm the structure looks like this:

```
CONNECT/
├── backend/
│   ├── include/user.h
│   └── src/
├── frontend/
│   ├── pages/
│   ├── styles/style.qss
│   └── main.cpp
├── CMakeLists.txt
└── CMakePresets.json
```

---

## 4. Build Methods

### Option A — Qt Creator (Recommended)

1. Open **Qt Creator**
2. Go to **File → Open File or Project**
3. Select `C:\CONNECT\CMakeLists.txt`
4. When prompted to configure, choose your Qt kit (e.g., _Qt 6.5 MinGW 64-bit_)
5. Click the **Build** button (hammer icon) or press `Ctrl+B`
6. Qt Creator places the binary in a `build-*` folder automatically

---

### Option B — Command Line: CMake Presets (Ninja)

This uses the `qt-debug` preset defined in `CMakePresets.json`.

```cmd
:: Open a terminal in the project root
cd C:\CONNECT

:: Make sure QTDIR is set (see Section 2)
set QTDIR=C:\Qt\6.5.3\mingw_64

:: Add Qt's bin and MinGW's bin to PATH
set PATH=%QTDIR%\bin;C:\msys64\ucrt64\bin;%PATH%

:: Configure
cmake --preset qt-debug

:: Build
cmake --build --preset qt-debug
```

The output binary will be at:

```
C:\CONNECT\build-qt-debug\ConnectifyUI.exe
```

---

### Option C — MinGW Makefiles

Use this if Ninja is not installed.

```cmd
cd C:\CONNECT

set QTDIR=C:\Qt\6.5.3\mingw_64
set PATH=%QTDIR%\bin;C:\msys64\ucrt64\bin;%PATH%

cmake --preset qt-mingw-debug
cmake --build --preset qt-mingw-debug
```

Output binary:

```
C:\CONNECT\build-mingw-debug\ConnectifyUI.exe
```

---

## 5. Running the Application

### From the terminal

```cmd
cd C:\CONNECT\build-qt-debug

:: Copy Qt DLLs next to the executable (first run only)
windeployqt ConnectifyUI.exe

:: Run
ConnectifyUI.exe
```

> **Why `windeployqt`?** Qt applications need several `.dll` files in the same folder. `windeployqt` copies them automatically. It is located in `%QTDIR%\bin\windeployqt.exe`.

### From Qt Creator

Press the green **Run** button (▶) or `Ctrl+R`.

---

## 6. Common Errors & Fixes

### ❌ `Could not find Qt6` or `Qt5 not found`

**Cause:** CMake cannot locate your Qt installation.

**Fix:**

```cmd
set QTDIR=C:\Qt\6.5.3\mingw_64
:: Then re-run cmake --preset qt-debug
```

Or pass it directly:

```cmd
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.5.3/mingw_64"
```

---

### ❌ `ninja: command not found` or `'ninja' is not recognized`

**Fix:** Add Ninja to your PATH:

```cmd
set PATH=C:\Qt\Tools\Ninja;%PATH%
:: OR
set PATH=C:\msys64\ucrt64\bin;%PATH%
```

---

### ❌ `mingw32-make: command not found` (Option C)

**Fix:**

```cmd
set PATH=C:\msys64\ucrt64\bin;%PATH%
:: Confirm it works:
mingw32-make --version
```

---

### ❌ `fatal error: user.h: No such file or directory`

**Cause:** The MOC (Qt's meta-object compiler) can't resolve `#include "user.h"`.

**Fix:** This is already handled in `CMakeLists.txt` via:

```cmake
target_include_directories(ConnectifyUI PRIVATE backend/include backend ...)
```

Make sure you are building from the project root (`C:\CONNECT`) and not from inside a subdirectory.

---

### ❌ `Cannot open include file: 'QApplication'` (MSVC only)

**Cause:** Qt headers not found by the MSVC compiler.

**Fix:** Use the `msvc-debug` preset AND open the terminal from the **Developer Command Prompt for VS**, not a regular CMD:

```cmd
cmake --preset msvc-debug
cmake --build --preset msvc-debug
```

---

### ❌ Application launches but shows a blank/unstyled window

**Cause:** The QSS stylesheet (`style.qss`) is embedded as a Qt resource (`:/style.qss`). If the resource wasn't compiled in, the style silently fails to load.

**Fix:** Ensure `CMakeLists.txt` contains this block (it does by default for Qt 6):

```cmake
qt_add_resources(ConnectifyUI "resources"
    PREFIX "/"
    FILES frontend/styles/style.qss
)
```

If you're on Qt 5, you need to add a `.qrc` file manually — ask for help if needed.

---

### ❌ `CMake 3.23 or higher is required` (from CMakePresets.json)

**Cause:** Your installed CMake is older than 3.23.

**Fix:** Download and install CMake ≥ 3.23 from [https://cmake.org/download](https://cmake.org/download). You can also temporarily lower the requirement in `CMakePresets.json`:

```json
"cmakeMinimumRequired": { "major": 3, "minor": 16, "patch": 0 }
```

---

### ❌ DLL errors when running the `.exe` directly (outside Qt Creator)

**Cause:** Qt DLLs are missing next to the executable.

**Fix:**

```cmd
cd C:\CONNECT\build-qt-debug
windeployqt ConnectifyUI.exe
```

---

## 7. Project Structure Reference

```
CONNECT/
├── backend/
│   ├── include/
│   │   └── user.h          ← All data model classes (User, Post, etc.)
│   └── src/
│       ├── user.cpp
│       ├── post.cpp
│       ├── message.cpp
│       ├── notification.cpp
│       └── search.cpp
│
├── frontend/
│   ├── integration/
│   │   └── mainwindow_integration.h   ← Bridges backend ↔ Qt UI
│   ├── pages/
│   │   ├── mainwindow.*    ← Main window shell
│   │   ├── LoginPage.*     ← Login screen
│   │   ├── signuppage.*    ← Registration screen
│   │   ├── newsfeedpage.*  ← News feed
│   │   ├── profilepage.*   ← User profile
│   │   ├── searchpage.*    ← Search
│   │   ├── messagepage.*   ← Direct messages
│   │   ├── notificationpage.* ← Notifications
│   │   └── admindashboard.*   ← Admin panel
│   ├── styles/
│   │   └── style.qss       ← Dark theme stylesheet (compiled as Qt resource)
│   └── main.cpp            ← Qt entry point, loads QSS + launches MainWindow
│
├── CMakeLists.txt           ← Build configuration (supports Qt5 & Qt6)
└── CMakePresets.json        ← Preset configurations (qt-debug, qt-mingw-debug, msvc-debug)
```

---

## Quick-Start Cheat Sheet

```cmd
:: 1. Set Qt path
set QTDIR=C:\Qt\6.5.3\mingw_64
set PATH=%QTDIR%\bin;C:\msys64\ucrt64\bin;%PATH%

:: 2. Go to project root
cd C:\CONNECT

:: 3. Configure + Build
cmake --preset qt-debug
cmake --build --preset qt-debug

:: 4. Deploy Qt DLLs (first time only)
cd build-qt-debug
windeployqt ConnectifyUI.exe

:: 5. Run
ConnectifyUI.exe
```

---

_If you're still hitting an error after trying these steps, share the exact error message and which step it occurs at — that will make it easy to diagnose._
