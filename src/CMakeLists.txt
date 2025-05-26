
cmake_minimum_required(VERSION 3.29.6)
project(GrooveJr VERSION 0.0.0)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: detect OS and set
# - for linux: -shared
# - for macOS: -dynamicLib
set (OS_LIB_FLAG "-dynamicLib")

# TODO: detect OS and set
# - for linux:
# - for macO_sonoma: /opt/homebrew/Cellar
# NOTE:
#  - on MacOS it may be helpful to use `brew info portaudio` to view home directory
set (C_LIBS_HOME "/opt/homebrew/Cellar")

set (GJ_BASE_DIR "/Users/ns/code/")
set (VST3SDK_HOME "${GJ_BASE_DIR}/vst3sdk")

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)

# setup Qt
list(APPEND CMAKE_PREFIX_PATH "${GJ_BASE_DIR}/qt6-build/qtbase/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets gui)
if (Qt6_FOUND)
    message(INFO " --Qt6--FOUND--")
endif()

qt_standard_project_setup()
set (INCLUDE_FLAGS
        -I${GJ_BASE_DIR}/actor-framework/libcaf_core
        -I${GJ_BASE_DIR}/actor-framework/libcaf_io
        -I${VST3SDK_HOME}
        -I${C_LIBS_HOME}/libsndfile/1.2.2/include
        -I${C_LIBS_HOME}/portaudio/19.7.0/include
)
# utilize vst3sdk cmake modules
list (APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)

# setup CAF
set (CAF_ROOT
        ${GJ_BASE_DIR}/actor-framework/build
)
find_package (CAF
        REQUIRED)

# setup SQLite
find_package(SQLite3 REQUIRED)

# setup local libraries
add_subdirectory(
    /Users/ns/code/GrooveJr/src/audio
)
add_subdirectory(
        /Users/ns/code/GrooveJr/src/gui
)
add_definitions(-DQT_NO_VERSION_TAGGING)

add_executable(GrooveJr
        main.h
        main.cpp
        AppState.h
        AppState.cpp
        Logging.h
        Logging.cpp
        actors/Supervisor.h
        actors/Playback.h
        actors/ActorIds.h
        actors/AppStateManager.h
        actors/AudioThread.h
        actors/EffectsManager.h
        enums/PlayState.h
        messaging/atoms.h
        messaging/EnvelopeQtPtr.h
        gui/MainToolbar/TransportControl.cpp
        gui/MainToolbar/TransportControl.h
        actors/Display.h
        db/Dao.h
        db/Dao.cpp
        db/entity/Track.h
        db/entity/Track.cpp
        db/repository/TrackRepository.h
        db/repository/TrackRepository.cpp
)
target_link_libraries(GrooveJr
        PUBLIC
        CAF::core
        GjAudio
        GjGui
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Boost::log
        ${Boost_LIBRARIES}
        SQLite::SQLite3
)
target_compile_options(GrooveJr PRIVATE ${INCLUDE_FLAGS})
set_target_properties(GrooveJr PROPERTIES LINKER_LANGUAGE CXX)
