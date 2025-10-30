cmake_minimum_required(VERSION 3.29.6)

project(GjGui VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_INSTALL_DIR /opt/homebrew/opt/llvm)

list (APPEND CMAKE_MODULE_PATH "${VST3SDK_HOME}/cmake/modules")
include(SMTG_DetectPlatform)
include(SMTG_FindJack)

# jack audio
if(NOT ${LIBJACK_FOUND})
    message(INFO " --GjGui--LIBJACK--NOT--FOUND--")
endif()

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Sql)
qt_standard_project_setup()

set (CAF_ROOT
        ${GJ_BASE_DIR}/actor-framework/build
)
find_package (CAF
        REQUIRED)

qt_add_library(GjGui STATIC
        Color.h
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
        Shared/VstSelect.h
        Shared/VstSelect.cpp
        Mixer/MixerWindow.h
        Mixer/MixerWindow.cpp
        Mixer/Channel/MainChannelContainer.h
        Mixer/Channel/MainChannelContainer.cpp
        Mixer/Channel/MuteButton.h
        Mixer/Channel/MuteButton.cpp
        Mixer/Channel/SoloButton.h
        Mixer/Channel/SoloButton.cpp
        Mixer/Channel/EffectsChannel/AddEffectsChannelButton.h
        Mixer/Channel/EffectsChannel/AddEffectsChannelButton.cpp
        Mixer/Channel/EffectsChannel/RemoveEffectsChannelButton.h
        Mixer/Channel/EffectsChannel/RemoveEffectsChannelButton.cpp
        Mixer/Channel/EffectsChannel/AddEffectButton.h
        Mixer/Channel/EffectsChannel/AddEffectButton.cpp
        Mixer/Channel/EffectsChannel/RemoveEffectButton.h
        Mixer/Channel/EffectsChannel/RemoveEffectButton.cpp
        Mixer/Channel/EffectsChannel/ReplaceEffectButton.h
        Mixer/Channel/EffectsChannel/ReplaceEffectButton.cpp
        Mixer/Channel/EffectsChannel/EffectsButton.h
        Mixer/Channel/EffectsChannel/EffectsButton.cpp
        Mixer/Channel/EffectsChannel/EffectsChannelsContainer.h
        Mixer/Channel/EffectsChannel/EffectsChannelsContainer.cpp
        Mixer/Channel/EffectsChannel/EffectsChannel.h
        Mixer/Channel/EffectsChannel/EffectsChannel.cpp
        Mixer/Channel/EffectsChannel/EffectSlot.h
        Mixer/Channel/EffectsChannel/EffectSlot.cpp
        Mixer/Channel/EffectsChannel/EffectsSlots.h
        Mixer/Channel/EffectsChannel/EffectsSlots.cpp
        Mixer/Channel/EffectsChannel/MuteSoloContainer.h
        Mixer/Channel/EffectsChannel/MuteSoloContainer.cpp
        Mixer/Channel/EffectsChannel/VuMeter.h
        Mixer/Channel/EffectsChannel/VuMeter.cpp
        Mixer/Channel/EffectsChannel/Effects/EffectsContainer.h
        Mixer/Channel/EffectsChannel/Effects/EffectsContainer.cpp
        Mixer/Channel/EffectsChannel/Effects/VstWindow.h
        Mixer/Channel/EffectsChannel/Effects/VstWindow.cpp
        Mixer/Channel/EffectsChannel/Effects/VstWindowSelectButton.h
        Mixer/Channel/EffectsChannel/Effects/VstWindowSelectButton.cpp
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
        MainToolBar/scenes/SceneSaveButton.h
        MainToolBar/scenes/SceneSaveButton.cpp
        MainToolBar/scenes/SceneSelectButton.h
        MainToolBar/scenes/SceneSelectButton.cpp
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
