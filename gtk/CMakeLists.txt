cmake_minimum_required(VERSION 3.30)
project(gtkTest)

set(CMAKE_CXX_STANDARD 20)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK4 REQUIRED gtk4)
PKG_CHECK_MODULES(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_BASE REQUIRED gstreamer-base-1.0)

INCLUDE_DIRECTORIES(${GTK4_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} include/ ../customWarning/ ../customWarning/colorPrint/)
LINK_DIRECTORIES(${GTK4_LIBRARY_DIRS} ${GSTREAMER_LIBRARY_DIRS})

add_executable(gtkTest main.cpp
        src/application.cpp
        ../customWarning/colorPrint/colorPrint.cpp
        src/patcher.cpp)
TARGET_LINK_LIBRARIES(gtkTest ${GTK4_LIBRARIES} ${GSTREAMER_LIBRARIES} ${GSTREAMER_BASE_LIBRARIES})