
cmake_minimum_required(VERSION 3.29.6)
project(GrooveJr VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# detect compilers
find_package(LLVM)
if (NOT LLVM_FOUND)
    message(INFO " \n LLVM NOT found ")
else ()
    message(INFO " \n LLVM FOUND ")
endif ()

option(GJ_ENABLE_ASAN "Enable AddressSanitizer" ON)
if (GJ_ENABLE_ASAN AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU"))
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif ()

message(INFO " Compiling GrooveJr with CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

# GJ_BASE_DIR: parent directory containing GrooveJr/, vst3sdk/, actor-framework/, qt6-build/.
# Override via: cmake -DGJ_BASE_DIR=/path/to/parent ..
if(NOT DEFINED GJ_BASE_DIR)
    get_filename_component(GJ_BASE_DIR "${CMAKE_SOURCE_DIR}/../.." ABSOLUTE)
endif()

set(VST3SDK_HOME "${GJ_BASE_DIR}/vst3sdk")

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)

# setup Qt
list(APPEND CMAKE_PREFIX_PATH "${GJ_BASE_DIR}/qt6-build/qtbase/lib/cmake")
find_package(Qt6 REQUIRED COMPONENTS Core Widgets OpenGLWidgets gui)
if (Qt6_FOUND)
    message(INFO " --Qt6--FOUND--")
endif ()

qt_standard_project_setup()

find_path(SNDFILE_INCLUDE_DIR sndfile.h REQUIRED)

set(INCLUDE_FLAGS
        -I${GJ_BASE_DIR}/actor-framework/libcaf_core
        -I${GJ_BASE_DIR}/actor-framework/libcaf_io
        -I${VST3SDK_HOME}
        -I${SNDFILE_INCLUDE_DIR}
)
# utilize vst3sdk cmake modules
list(APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)

# setup CAF — override via: cmake -DCAF_ROOT=/path/to/caf ..
if(NOT DEFINED CAF_ROOT)
    set(CAF_ROOT ${GJ_BASE_DIR}/actor-framework/build)
endif()
find_package(CAF REQUIRED)

# setup SQLite
find_package(SQLite3 REQUIRED)

# setup local libraries
add_subdirectory(audio)
add_subdirectory(gui)
add_subdirectory(db)
add_subdirectory(scanner)
add_definitions(-DQT_NO_VERSION_TAGGING)

add_executable(GrooveJr
        main.h
        main.cpp
        state/Core.h
        state/Core.cpp
        state/Packet.h
        state/mixer/ChannelPacket.h
        state/mixer/Packet.h
        state/mixer/PluginSlotPacket.h
        Logging.h
        Logging.cpp
        Paths.h
        actors/ActorIds.h
        actors/AppStateManager.h
        actors/AudioThread.h
        actors/Display.h
        actors/MusicLibraryManager.h
        actors/Playback.h
        actors/Supervisor.h
        enums/AudioCodec.h
        enums/PlayState.h
        enums/Result.h
        messaging/atoms.h
        messaging/DecksState.h
        messaging/EnvelopeQtPtr.h
        gui/MainToolBar/transport/TransportControl.cpp
        gui/MainToolBar/transport/TransportControl.h
        types/AtomicStr.h
)
target_link_libraries(GrooveJr
        PUBLIC
        CAF::core
        GjAudio
        GjDb
        GjGui
        GjScanner
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::OpenGLWidgets
        Boost::log
        ${Boost_LIBRARIES}
        SQLite::SQLite3
)
target_compile_options(GrooveJr PRIVATE ${INCLUDE_FLAGS})
set_target_properties(GrooveJr PROPERTIES LINKER_LANGUAGE CXX)
