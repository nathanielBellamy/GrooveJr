
### In order to run this program within Intellij
### we need to tell the JVM where to find the the compiled .so/.dylib library.
### To do so, edit the Run Configuration for the `gs_desktop_application` by adding the following VM option
### -Djava.library.path="/absolute/path/to/repo/GrooveSprings/gs-jni/src/main/java/dev/nateschieber/groovesprings/jni/build"

cmake_minimum_required(VERSION 3.29.6)
project(GjAudio VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
# utilize vst3sdk cmake modules
list(APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)
include(SMTG_FindJack)

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)

# handle Vst3 Windows in Qt
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# jack audio
if (NOT ${LIBJACK_FOUND})
    message(INFO " --GjAudio--LIBJACK--NOT--FOUND--")
endif ()

set(C_LIBS_INCLUDE_FLAGS
        -I${C_LIBS_HOME}/libsndfile/1.2.2/include
        -I${C_LIBS_HOME}/fftw/3.3.10_2/include
)

set(CXX_LIBS_INCLUDE_FLAGS
        -I${VST3SDK_HOME}
)

set(INCLUDE_FLAGS
        ${C_LIBS_INCLUDE_FLAGS}
        ${CXX_LIBS_INCLUDE_FLAGS}
)

set(LINK_FLAGS
        -L${C_LIBS_HOME}/libsndfile/1.2.2/lib
        -lsndfile
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk_hosting
        -L${VST3SDK_HOME}/build/lib/Release
        -lpluginterfaces
        -L${C_LIBS_HOME}/fftw/3.3.10_2/lib
        -lfftw3f
)

# compile vst3 audiohost
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${VST3SDK_HOME}/cmake/modules)
add_subdirectory(plugins/vst3/host)

# make caf available
set(CAF_ROOT
        /Users/ns/code/actor-framework/build
)
find_package(CAF
        REQUIRED)

# compile library
add_library(GjAudio SHARED
        engine/AudioCore.h
        engine/AudioDeck.h
        engine/AudioPlayer.h
        engine/Cassette.h
        engine/Cassette.cpp
        engine/Math.h
        engine/JackClient.h
        engine/JackClient.cpp
        Logging.h
        Logging.cpp
        Constants.h
        plugins/vst3/Plugin.h
        plugins/vst3/Plugin.cpp
        plugins/vst3/Util.h
        mixer/ChannelProcessData.h
        mixer/Channel.h
        mixer/Channel.cpp
        mixer/ChannelSettings.h
        mixer/Core.h
        mixer/Core.cpp
)
target_compile_options(GjAudio PRIVATE ${INCLUDE_FLAGS})
add_definitions(-DQT_NO_VERSION_TAGGING)
set(LINK_LIBRARIES
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk_common
        vst3host
        audiohost
        editorhost
        CAF::core
        ${LINK_FLAGS}
)

if (SMTG_MAC)
    set(LINK_LIBRARIES "${LINK_LIBRARIES}
    -framework CoreFoundation
    -framework Foundation"
    )
endif (SMTG_MAC)
target_link_libraries(GjAudio PUBLIC
        ${LINK_LIBRARIES}
        Boost::log
        ${Boost_LIBRARIES}
        Qt6::Core
        Qt6::Widgets
        ${LIBJACK_LIBRARY}
        GjDb
)
target_include_directories(GjAudio
        PRIVATE
        ${INCLUDE_FLAGS}
        ${LIBJACK_INCLUDE_DIR}
)
