
find_package(Boost REQUIRED COMPONENTS log_setup log)

set(editorhost_sources
    ${VST3SDK_HOME}/public.sdk/source/vst/hosting/plugprovider.cpp
    ${VST3SDK_HOME}/public.sdk/source/vst/hosting/plugprovider.h
        source/editorhost.cpp
        source/editorhost.h
        source/ComponentHandler.cpp
        source/ComponentHandler.h
        source/WindowController.cpp
        source/WindowController.h
        source/platform/appinit.h
        source/platform/iapplication.h
        source/platform/iplatform.h
        source/platform/iwindow.h
        source/usediids.cpp
)

#    if(SMTG_MAC)
    set(editorhost_sources
        "${editorhost_sources}"
            source/platform/mac/platform.mm
            source/platform/mac/window.h
            source/platform/mac/window.mm
        "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_mac.mm"
    )
    set_source_files_properties(
            source/platform/mac/window.mm
            source/platform/mac/platform.mm
        "${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_mac.mm" PROPERTIES
        COMPILE_FLAGS "-fobjc-arc"
    )
    set(editorhost_PLATFORM_LIBS
        "-framework Cocoa"
    )
    get_filename_component(InfoPlistFile "resource/Info.plist" ABSOLUTE)
    set(APP_PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${InfoPlistFile}
    )
#    elseif(SMTG_LINUX)
#        find_package(X11 REQUIRED)
#        find_package(PkgConfig REQUIRED)
#        pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
#        pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
#        include_directories(${X11_INCLUDE_DIR} ${GTKMM3_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS})
#        set(editorhost_sources
#            ${editorhost_sources}
#            source/platform/linux/platform.cpp
#            source/platform/linux/runloop.h
#            source/platform/linux/runloop.cpp
#            source/platform/linux/window.h
#            source/platform/linux/window.cpp
#            ${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_linux.cpp
#        )
#        set(editorhost_PLATFORM_LIBS
#            ${X11_LIBRARIES}
#            ${GTK3_LIBRARIES}
#            ${GTKMM3_LIBRARIES}
#        )
#        set(APP_PROPERTIES
#        )
#    elseif(SMTG_WIN)
#        set(editorhost_sources
#            ${editorhost_sources}
#           source/platform/win32/platform.cpp
#           source/platform/win32/window.h
#           source/platform/win32/window.cpp
#           ${VST3SDK_HOME}/public.sdk/source/vst/hosting/module_win32.cpp
#       )
#       set(editorhost_PLATFORM_LIBS
#       )
#       if(MSVC)
#           set(APP_PROPERTIES
#                  LINK_FLAGS "/SUBSYSTEM:windows"
#           )
#       endif(MSVC)
#       if(MINGW)
#           # It causes the UNICODE preprocessor macro to be predefined, and chooses Unicode-capable runtime startup code.
#           set(APP_PROPERTIES
#                  LINK_FLAGS -municode
#           )
#       endif(MINGW)
#   endif(SMTG_MAC)

set(target editorhost)
add_library(${target} STATIC ${editorhost_sources})
set_target_properties(${target} PROPERTIES SDK_IDE_HOSTING_EXAMPLES_FOLDER "${VST3SDK_HOME}/public.sdk/source/vst/hosting")
target_compile_features(${target}
    PUBLIC
        cxx_std_17
)
target_compile_options(${target} PRIVATE -I${VST3SDK_HOME})
target_link_libraries(${target}
    PRIVATE
#            sdk_hosting
        sdk_common
        ${editorhost_PLATFORM_LIBS}
)
target_link_libraries(${target}
    PUBLIC
    "-framework CoreFoundation" # macOS
    "-framework Foundation"     #  and Objective C
    Boost::log
    ${Boost_LIBRARIES}
)
smtg_target_setup_universal_binary(${target})
if (APP_PROPERTIES)
    set_target_properties(${target}
        PROPERTIES
            ${APP_PROPERTIES}
    )
endif(APP_PROPERTIES)