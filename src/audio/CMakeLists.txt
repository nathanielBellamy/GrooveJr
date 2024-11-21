
### In order to run this program within Intellij
### we need to tell the JVM where to find the the compiled .so/.dylib library.
### To do so, edit the Run Configuration for the `gs_desktop_application` by adding the following VM option
### -Djava.library.path="/absolute/path/to/repo/GrooveSprings/gs-jni/src/main/java/dev/nateschieber/groovesprings/jni/build"

cmake_minimum_required(VERSION 3.29.6)
project(gsAudio VERSION 0.0.0)

set (CMAKE_CXX_STANDARD 17)
set (VST3SDK_HOME "/Users/ns/code/vst3sdk")
# utilize vst3sdk cmake modules
list (APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform) # to detect platform


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


set (C_LIBS_INCLUDE_FLAGS
        -I${C_LIBS_HOME}/libsndfile/1.2.2/include
        -I${C_LIBS_HOME}/portaudio/19.7.0/include
)

set (CXX_LIBS_INCLUDE_FLAGS
        -I${VST3SDK_HOME}
)

set (INCLUDE_FLAGS
        ${C_LIBS_INCLUDE_FLAGS}
        ${CXX_LIBS_INCLUDE_FLAGS}
)

set (LINK_FLAGS
        -L${C_LIBS_HOME}/libsndfile/1.2.2/lib
        -lsndfile
        -L${C_LIBS_HOME}/portaudio/19.7.0/lib
        -lportaudio
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk_hosting
        -L${VST3SDK_HOME}/build/lib/Release
        -lpluginterfaces
)

# compile vst3 audiohost
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${VST3SDK_HOME}/cmake/modules)
add_subdirectory(effects/vst3/host)

# make caf available
set (CAF_ROOT
        /Users/ns/code/actor-framework/build
)
find_package (CAF
        REQUIRED)

# compile library
add_library(gsAudio SHARED
        audio.h
        audio.cpp
)
target_compile_options(gsAudio PRIVATE ${INCLUDE_FLAGS})
set(LINK_LIBRARIES
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk_common
        audiohost
        editorhost
        CAF::core
        ${LINK_FLAGS}
)

if (SMTG_MAC)
    set (LINK_LIBRARIES "${LINK_LIBRARIES}
    -framework CoreFoundation
    -framework Foundation"
    )
endif (SMTG_MAC)
target_link_libraries(gsAudio PUBLIC ${LINK_LIBRARIES})
