cmake_minimum_required(VERSION 3.29.6)

project(gsGui VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_INSTALL_DIR /opt/homebrew/opt/llvm)


list(APPEND CMAKE_PREFIX_PATH "/Users/ns/code/qt6-build/qtbase/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()

set (CAF_ROOT
        /Users/ns/code/actor-framework/build
)
find_package (CAF
        REQUIRED)

qt_add_library(gsGui STATIC
        MainWindow.h
        MainWindow.cpp
)

target_compile_options(gsGui
        PRIVATE
        -I/Users/ns/code/qt6-build/qtbase/include
        -I/Users/ns/code/actor-framework/libcaf_core
        -I/Users/ns/code/actor-framework/libcaf_io
)
target_link_libraries(gsGui
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        CAF::core
)
