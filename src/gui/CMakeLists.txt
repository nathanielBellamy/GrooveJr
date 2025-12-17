cmake_minimum_required(VERSION 3.29.6)

project(GjGui VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_INSTALL_DIR /opt/homebrew/opt/llvm)

list(APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)
include(SMTG_FindJack)

# jack audio
if (NOT ${LIBJACK_FOUND})
    message(INFO " --GjGui--LIBJACK--NOT--FOUND--")
endif ()

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Sql)
qt_standard_project_setup()

set(CAF_ROOT
        ${GJ_BASE_DIR}/actor-framework/build
)
find_package(CAF
        REQUIRED)

qt_add_library(GjGui STATIC
        Color.h
        MainWindow.h
        MainWindow.cpp
        Hydrater.h
        QSql/SqlQueryModel.h
        QSql/SqlQueryModel.cpp
        QSql/SqlTableView.h
        QSql/SqlTableView.cpp
        QSql/SqlWorker.h
        QSql/SqlWorker.cpp
        QSql/SqlWorkerPool.h
        QSql/SqlWorkerPool.cpp
        QSql/SqlWorkerPoolClient.h
        QSql/SqlWorkerPoolClient.cpp
        QSql/SqlWorkerPoolHost.h
        QSql/SqlWorkerPoolHost.cpp
        MenuBar/MenuBar.h
        MenuBar/MenuBar.cpp
        MenuBar/Menus/FileMenu.h
        MenuBar/Menus/FileMenu.cpp
        MenuBar/Menus/Music/MusicMenu.h
        MenuBar/Menus/Music/MusicMenu.cpp
        MenuBar/Menus/Music/FolderSelect.h
        MenuBar/Menus/Music/FolderSelect.cpp
        Shared/VstSelect.h
        Shared/VstSelect.cpp
        Mixer/Body.h
        Mixer/Body.cpp
        Mixer/Channels/MainChannelContainer.h
        Mixer/Channels/MainChannelContainer.cpp
        Mixer/Channels/Channel/MuteButton.h
        Mixer/Channels/Channel/MuteButton.cpp
        Mixer/Channels/Channel/SoloButton.h
        Mixer/Channels/Channel/SoloButton.cpp
        Mixer/Channels/AddChannelButton.h
        Mixer/Channels/AddChannelButton.cpp
        Mixer/Channels/Channel/RemoveChannelButton.h
        Mixer/Channels/Channel/RemoveChannelButton.cpp
        Mixer/Channels/Channel/Plugins/AddPluginButton.h
        Mixer/Channels/Channel/Plugins/AddPluginButton.cpp
        Mixer/Channels/Channel/Plugins/RemovePluginButton.h
        Mixer/Channels/Channel/Plugins/RemovePluginButton.cpp
        Mixer/Channels/Channel/Plugins/ReplacePluginButton.h
        Mixer/Channels/Channel/Plugins/ReplacePluginButton.cpp
        Mixer/Channels/Channel/Plugins/PluginsButton.h
        Mixer/Channels/Channel/Plugins/PluginsButton.cpp
        Mixer/Channels/ChannelsContainer.h
        Mixer/Channels/ChannelsContainer.cpp
        Mixer/Channels/Channel/Channel.h
        Mixer/Channels/Channel/Channel.cpp
        Mixer/Channels/Channel/Plugins/PluginSlot.h
        Mixer/Channels/Channel/Plugins/PluginSlot.cpp
        Mixer/Channels/Channel/Plugins/PluginSlots.h
        Mixer/Channels/Channel/Plugins/PluginSlots.cpp
        Mixer/Channels/Channel/MuteSoloContainer.h
        Mixer/Channels/Channel/MuteSoloContainer.cpp
        Mixer/Channels/Channel/VuMeter.h
        Mixer/Channels/Channel/VuMeter.cpp
        Mixer/Channels/Channel/Plugins/PluginsContainer.h
        Mixer/Channels/Channel/Plugins/PluginsContainer.cpp
        Mixer/Channels/Channel/Plugins/VstWindow.h
        Mixer/Channels/Channel/Plugins/VstWindow.cpp
        Mixer/Channels/Channel/Plugins/VstWindowSelectButton.h
        Mixer/Channels/Channel/Plugins/VstWindowSelectButton.cpp
        MusicLibrary/Constants.h
        MusicLibrary/MusicLibraryWindow.h
        MusicLibrary/MusicLibraryFilters.h
        MusicLibrary/MusicLibraryQueryModel.h
        MusicLibrary/MusicLibraryTableView.h
        MusicLibrary/MusicLibraryWindow.cpp
        MusicLibrary/albums/AlbumTableView.h
        MusicLibrary/albums/AlbumQueryModel.h
        MusicLibrary/albums/AlbumQueryModel.cpp
        MusicLibrary/artists/ArtistTableView.h
        MusicLibrary/artists/ArtistQueryModel.h
        MusicLibrary/artists/ArtistQueryModel.cpp
        MusicLibrary/audioFiles/AudioFileTableView.h
        MusicLibrary/audioFiles/AudioFileTableView.cpp
        MusicLibrary/audioFiles/AudioFileQueryModel.h
        MusicLibrary/audioFiles/AudioFileQueryModel.cpp
        MusicLibrary/cache/CacheTableView.h
        MusicLibrary/cache/CacheQueryModel.h
        MusicLibrary/cache/CacheQueryModel.cpp
        MusicLibrary/genres/GenreTableView.h
        MusicLibrary/genres/GenreQueryModel.h
        MusicLibrary/genres/GenreQueryModel.cpp
        MusicLibrary/playlists/PlaylistTableView.h
        MusicLibrary/playlists/PlaylistQueryModel.h
        MusicLibrary/playlists/PlaylistQueryModel.cpp
        MusicLibrary/queue/QueueTableView.h
        MusicLibrary/queue/QueueQueryModel.h
        MusicLibrary/queue/QueueQueryModel.cpp
        MainToolBar/CurrentlyPlaying.h
        MainToolBar/CurrentlyPlaying.cpp
        MainToolBar/MainToolBar.h
        MainToolBar/MainToolBar.cpp
        MainToolBar/eq/EqGraph.h
        MainToolBar/eq/EqGraph.cpp
        MainToolBar/transport/PlaybackSpeedSlider.h
        MainToolBar/transport/PlaybackSpeedSlider.cpp
        MainToolBar/transport/ProgressBar.h
        MainToolBar/transport/ProgressBar.cpp
        MainToolBar/transport/TransportControl.h
        MainToolBar/transport/TransportControl.cpp
        MainToolBar/transport/TransportControlButton.h
        MainToolBar/transport/TransportControlButton.cpp
        MainToolBar/scenes/Scenes.h
        MainToolBar/scenes/Scenes.cpp
        MainToolBar/scenes/SceneButton.h
        MainToolBar/scenes/SceneButton.cpp
        MainToolBar/scenes/ScenesTableView.h
        MainToolBar/scenes/ScenesQueryModel.h
        MainToolBar/scenes/ScenesQueryModel.cpp
)

message(GUI_VST3_SDK="${VST3SDK_HOME}")

target_compile_options(GjGui
        PRIVATE
        -I${VST3SDK_HOME}
        -I/${GJ_BASE_DIR}/qt6-build/qtbase/include
        -I/${GJ_BASE_DIR}/actor-framework/libcaf_core
        -I/${GJ_BASE_DIR}/actor-framework/libcaf_io
)
add_definitions(-DQT_NO_VERSION_TAGGING)

target_link_libraries(GjGui
        PRIVATE
        Qt6::Core
        Qt6::Widgets
        Qt6::Gui
        Qt6::Sql
        CAF::core
        -L${VST3SDK_HOME}/build/lib/Release
        -lsdk
        Boost::log
        ${Boost_LIBRARIES}
        ${LIBJACK_LIBRARY}
)

target_include_directories(GjGui
        PRIVATE
        ${LIBJACK_INCLUDE_DIR}
)
