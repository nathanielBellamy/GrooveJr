
cmake_minimum_required(VERSION 3.29.6)
project(vst3host)
set(VST3SDK_HOME "/Users/ns/code/vst3sdk")
list (APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)
include(SMTG_UniversalBinary)
add_subdirectory(audiohost)
add_subdirectory(editorhost)

add_library(vst3host
        SHARED
        hostclasses.hpp
        hostclasses.cpp
)
target_compile_options(vst3host
        PUBLIC
        -I${VST3SDK_HOME}
        -I${VST3SDK_HOME}/public.sdk/source/vst/utility
)
target_link_libraries(vst3host PRIVATE
        audiohost
        editorhost
)
target_link_libraries(vst3host
        PUBLIC
        -L${VST3SDK_HOME}/build/lib/Release
        -lpluginterfaces
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk_hosting
        "-framework CoreFoundation" # macOS
        "-framework Foundation"     #  and Objective C
)
