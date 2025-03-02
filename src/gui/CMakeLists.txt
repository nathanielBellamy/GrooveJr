cmake_minimum_required(VERSION 3.29.6)

project(GjGui VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_INSTALL_DIR /opt/homebrew/opt/llvm)


find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)
qt_standard_project_setup()

set (CAF_ROOT
        ${GJ_BASE_DIR}/actor-framework/build
)
find_package (CAF
        REQUIRED)

qt_add_library(GjGui STATIC
        MainWindow.h
        MainWindow.cpp
        MenuBar/MenuBar.h
        MenuBar/MenuBar.cpp
        MenuBar/Menus/FileMenu.h
        MenuBar/Menus/FileMenu.cpp
        MenuBar/Menus/Music/MusicMenu.h
        MenuBar/Menus/Music/MusicMenu.cpp
        MenuBar/Menus/Music/FolderSelect.h
        MenuBar/Menus/Music/FolderSelect.cpp
        MenuBar/Menus/Effects/EffectsMenu.h
        MenuBar/Menus/Effects/EffectsMenu.cpp
        MenuBar/Menus/Effects/VstSelect.h
        MenuBar/Menus/Effects/VstSelect.cpp
        Mixer/MixerWindow.h
        Mixer/MixerWindow.cpp
        MusicLibrary/MusicLibraryWindow.h
        MusicLibrary/MusicLibraryWindow.cpp
        TransportControl.h
        TransportControl.cpp
)

target_compile_options(GjGui
        PRIVATE
        -I/Users/ns/code/qt6-build/qtbase/include
        -I/Users/ns/code/actor-framework/libcaf_core
        -I/Users/ns/code/actor-framework/libcaf_io
)
target_link_libraries(GjGui
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        CAF::core
)
