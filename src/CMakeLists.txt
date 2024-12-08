
cmake_minimum_required(VERSION 3.29.6)
project(GrooveJr VERSION 0.0.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (VST3SDK_HOME "/Users/ns/code/vst3sdk")
set (C_LIBS_HOME "/opt/homebrew/Cellar")

#set(QT_NO_STANDARD_PROJECT_SETUP TRUE)

list(APPEND CMAKE_PREFIX_PATH "/Users/ns/code/qt6-build/qtbase/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets gui)
if (Qt6_FOUND)
    message(INFO " --Qt6--FOUND--")
endif()

qt_standard_project_setup()
set (INCLUDE_FLAGS
        -I/Users/ns/code/actor-framework/libcaf_core
        -I/Users/ns/code/actor-framework/libcaf_io
        -I${VST3SDK_HOME}
        -I${C_LIBS_HOME}/libsndfile/1.2.2/include
        -I${C_LIBS_HOME}/portaudio/19.7.0/include
)
# utilize vst3sdk cmake modules
list (APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)

set (CAF_ROOT
        /Users/ns/code/actor-framework/build
)
find_package (CAF
        REQUIRED)

add_subdirectory(
    /Users/ns/code/GrooveJr/src/audio
)
add_subdirectory(
        /Users/ns/code/GrooveJr/src/gui
)

add_executable(GrooveJr
        main.h
        main.cpp
        AppState.h
        AppState.cpp
        actors/Supervisor.h
        actors/Playback.h
        actors/ActorIds.h
        actors/AppStateManager.h
        actors/AudioThread.h
        enums/PlayState.h
        messaging/atoms.h
        messaging/EnvelopeQtPtr.h
        gui/TransportControl.cpp
        gui/TransportControl.h
        actors/Display.h
)
target_link_libraries(GrooveJr
        PUBLIC
        CAF::core
        gsAudio
        gsGui
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
)
target_compile_options(GrooveJr PRIVATE ${INCLUDE_FLAGS})
set_target_properties(GrooveJr PROPERTIES LINKER_LANGUAGE CXX)
