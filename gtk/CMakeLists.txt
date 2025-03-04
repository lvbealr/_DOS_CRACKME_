cmake_minimum_required(VERSION 3.30)
# Consider renaming the project (gtkTest is not the best option I guess)
project(gtkTest)

# It's better to use a target_compile_features() function. CMAKE_CXX_STANDARD sets standard version for ALL projects at once. Not great. 
set(CMAKE_CXX_STANDARD 20)

# Why did you write the following lines in upper case? It seems you're using a lower case everywhere else
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK4 REQUIRED gtk4)
PKG_CHECK_MODULES(GSTREAMER REQUIRED gstreamer-1.0)
PKG_CHECK_MODULES(GSTREAMER_BASE REQUIRED gstreamer-base-1.0)

INCLUDE_DIRECTORIES(${GTK4_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS} include/ ../customWarning/ ../customWarning/colorPrint/)
LINK_DIRECTORIES(${GTK4_LIBRARY_DIRS} ${GSTREAMER_LIBRARY_DIRS})

# Total shit. 
#
# 1. Consider adding cmake files into colorPrint and customWarning repos.
# Just imagine me using your lib. Why should I think about adding .cpp files from the colorPrint WHICH IS IN customWarning?????
# Nobody will know about this fucking file which is stored somewhere in your ass. Just fix that.
# Use STATIC lib for the colorPrint (think why) and INTERFACE lib for the customWarning as it's header-onlu
#
# 2. It's better to create cmake file inside src dir. Or at list use the list() funcion with TRANSFORM and PREPEND to add an src/ prefix
add_executable(gtkTest main.cpp
        src/application.cpp
        ../customWarning/colorPrint/colorPrint.cpp
        src/patcher.cpp)
TARGET_LINK_LIBRARIES(gtkTest ${GTK4_LIBRARIES} ${GSTREAMER_LIBRARIES} ${GSTREAMER_BASE_LIBRARIES})

# Where is target_include_directories()??? What if I change a header inside your project without changing any .cpp files?
# Hint: It won't fucking rebuild or you will encounter strange bug.

# Create a cmake file with add_subdirectory(patcher) in root of your project. Otherwise it's not clear how to build your shit.
