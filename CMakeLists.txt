cmake_minimum_required(VERSION 3.16)
project(ConnectifyUI VERSION 0.1 LANGUAGES CXX)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to discover Qt via common environment variables first (portable across laptops).
if(DEFINED ENV{QTDIR} AND NOT "$ENV{QTDIR}" STREQUAL "")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()
if(DEFINED ENV{Qt6_DIR} AND NOT "$ENV{Qt6_DIR}" STREQUAL "")
    list(APPEND CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
endif()

find_package(Qt6 QUIET COMPONENTS Widgets)
if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets)
    set(QT_VERSION_MAJOR 5)
endif()
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    LoginPage.h
    LoginPage.cpp
    signuppage.h
    signuppage.cpp
    newsfeedpage.h
    newsfeedpage.cpp
    profilepage.h
    profilepage.cpp
    searchpage.h
    searchpage.cpp
    messagepage.h
    messagepage.cpp
    admindashboard.h
    admindashboard.cpp
    notificationpage.h
    notificationpage.cpp
    backend/user.cpp
    backend/post.cpp
    backend/message.cpp
    backend/notification.cpp
    backend/search.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ConnectifyUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    qt_add_resources(ConnectifyUI "resources"
        PREFIX "/"
        FILES style.qss
    )
else()
    if(ANDROID)
        add_library(ConnectifyUI SHARED ${PROJECT_SOURCES})
    else()
        add_executable(ConnectifyUI ${PROJECT_SOURCES})
    endif()
endif()
target_link_libraries(ConnectifyUI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if(DEFINED QT_VERSION AND QT_VERSION VERSION_LESS "6.1.0")
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ConnectifyUI)
endif()
set_target_properties(ConnectifyUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
include(GNUInstallDirs)
install(TARGETS ConnectifyUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ConnectifyUI)
endif()
