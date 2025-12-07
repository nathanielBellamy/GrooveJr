
list(APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)
include(SMTG_FindJack)

find_package(Boost REQUIRED COMPONENTS log_setup log)

if (${LIBJACK_FOUND})
    set(audiohost_sources
            "${VST3SDK_HOME}/public.sdk/samples/vst-hosting/editorhost/source/platform/iapplication.h"
            "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module.h"
            "${VST3SDK_HOME}/public.sdk/source/vst/hosting/plugprovider.h"
            "${VST3SDK_HOME}/public.sdk/source/vst/hosting/plugprovider.cpp"
            source/audiohost.cpp
            source/audiohost.h
            source/media/audioclient.cpp
            source/media/audioclient.h
            source/media/imediaserver.h
            source/media/iparameterclient.h
            source/media/jack/jackclient.cpp
            source/media/miditovst.h
            source/platform/appinit.h
            source/usediids.cpp
    )

    # TODO: debug detect platform
    # if(SMTG_MAC)
    set(audiohost_sources
            ${audiohost_sources}
            "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_mac.mm"
    )
    set_source_files_properties(
            "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_mac.mm" PROPERTIES
            COMPILE_FLAGS "-fobjc-arc"
    )
    set(audiohost_PLATFORM_LIBS "-framework CoreFoundation")
    get_filename_component(InfoPlistFile "resource/Info.plist" ABSOLUTE)
    set(APP_PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile}
    )
    # elseif(SMTG_LINUX)
    #    set(audiohost_sources
    #        ${audiohost_sources}
    #        "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_linux.cpp"
    #    )
    #    set(audiohost_PLATFORM_LIBS)
    #    set(APP_PROPERTIES)
    # elseif(SMTG_WIN)
    #     set(audiohost_sources
    #        ${audiohost_sources}
    #        "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_win32.cpp"
    #    )
    #    set(audiohost_PLATFORM_LIBS)
    #    if(MSVC)
    #        set(APP_PROPERTIES
    #            LINK_FLAGS
    #                "/SUBSYSTEM:console"
    #        )
    #    endif(MSVC)
    #endif(SMTG_MAC)

    set(target audiohost)

    add_library(${target} SHARED ${audiohost_sources})
    #     set_target_properties(${target} PROPERTIES ${SDK_IDE_HOSTING_EXAMPLES_FOLDER} "${VST3SDK_HOME}/build/public.sdk/CMakeFiles/sdk_hosting.dir")
    target_include_directories(${target}
            PRIVATE
            ${VST3SDK_HOME}
            -I${VST3SDK_HOME}/public.sdk/source/vst/utility)
    target_compile_features(${target}
            PUBLIC
            cxx_std_17
    )

    target_link_libraries(${target}
            PRIVATE
            -L${VST3SDK_HOME}/build/lib/Release
            -lsdk
            -L${VST3SDK_HOME}/build/lib/Release
            -lsdk_common
            -L${VST3SDK_HOME}/build/lib/Release
            -lsdk_hosting
            -L${VST3SDK_HOME}/build/lib/Release
            -lpluginterfaces
            "-framework CoreFoundation" # macOS
            "-framework Foundation"     #  and Objective C
            ${LIBJACK_LIBRARY}
            Boost::log
            ${Boost_LIBRARIES}
    )
    target_include_directories(${target}
            PRIVATE
            ${LIBJACK_INCLUDE_DIR}
    )
    if (APP_PROPERTIES)
        set_target_properties(${target}
                PROPERTIES
                ${APP_PROPERTIES}
        )
    endif (APP_PROPERTIES)
else ()
    message(STATUS "[SMTG] LIBJACK_FOUND is not set.")
endif (${LIBJACK_FOUND})
