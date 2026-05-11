cmake_minimum_required(VERSION 3.16)
project(ConnectifyUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Qt discovery — checks env vars first, then msys2/ucrt64 fallback
# ---------------------------------------------------------------------------
if(DEFINED ENV{QTDIR} AND NOT "$ENV{QTDIR}" STREQUAL "")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()
if(DEFINED ENV{Qt6_DIR} AND NOT "$ENV{Qt6_DIR}" STREQUAL "")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
endif()
list(APPEND CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(QT_VERSION_MAJOR 5)
endif()

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
set(PROJECT_SOURCES
    # Qt entry point
    frontend/main.cpp

    # Pages
    frontend/pages/mainwindow.cpp
    frontend/pages/mainwindow.h
    frontend/pages/mainwindow.ui
    frontend/pages/LoginPage.h
    frontend/pages/LoginPage.cpp
    frontend/pages/signuppage.h
    frontend/pages/signuppage.cpp
    frontend/pages/newsfeedpage.h
    frontend/pages/newsfeedpage.cpp
    frontend/pages/profilepage.h
    frontend/pages/profilepage.cpp
    frontend/pages/searchpage.h
    frontend/pages/searchpage.cpp
    frontend/pages/messagepage.h
    frontend/pages/messagepage.cpp
    frontend/pages/admindashboard.h
    frontend/pages/admindashboard.cpp
    frontend/pages/notificationpage.h
    frontend/pages/notificationpage.cpp

    # NOTE: mainwindow_integration.h is intentionally NOT listed here.
    # It is a pass-through header that just includes mainwindow.h.
    # Listing it separately caused MOC to generate a second moc_mainwindow
    # that redefined the same MainWindow class → duplicate symbol errors.

    # Backend
    backend/src/user.cpp
    backend/src/post.cpp
    backend/src/message.cpp
    backend/src/notification.cpp
    backend/src/search.cpp
)

# ---------------------------------------------------------------------------
# Executable
# ---------------------------------------------------------------------------
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ConnectifyUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_add_resources(ConnectifyUI "resources"
        PREFIX "/"
        FILES frontend/styles/style.qss
    )
else()
    if(ANDROID)
        add_library(ConnectifyUI SHARED ${PROJECT_SOURCES})
    else()
        add_executable(ConnectifyUI ${PROJECT_SOURCES})
    endif()
endif()

# ---------------------------------------------------------------------------
# Include directories
# "backend/include" is listed so MOC can resolve #include "user.h" without
# any relative "../" paths, which break during the autogen step.
# ---------------------------------------------------------------------------
target_include_directories(ConnectifyUI PRIVATE
    backend/include
    backend
    frontend/pages
    frontend/integration
)

# Pass include dirs explicitly to MOC so it never needs relative paths
set_target_properties(ConnectifyUI PROPERTIES
    AUTOMOC_MOC_OPTIONS
        "-I${CMAKE_SOURCE_DIR}/backend/include;-I${CMAKE_SOURCE_DIR}/backend"
)

# ---------------------------------------------------------------------------
# Link
# ---------------------------------------------------------------------------
target_link_libraries(ConnectifyUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# ---------------------------------------------------------------------------
# Target properties
# ---------------------------------------------------------------------------
set_target_properties(ConnectifyUI PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
include(GNUInstallDirs)
install(TARGETS ConnectifyUI
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ConnectifyUI)
endif()