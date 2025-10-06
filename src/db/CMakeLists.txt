cmake_minimum_required(VERSION 3.29.6)

project(GjDb VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_INSTALL_DIR /opt/homebrew/opt/llvm)

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)
# setup SQLite
find_package(SQLite3 REQUIRED)

add_library(GjDb SHARED
    Logging.h
    Logging.cpp
    Dao.h
    Dao.cpp
    entity/AppStateEntity.h
    entity/AppStateEntity.cpp
    repository/AppStateRepository.h
    repository/AppStateRepository.cpp
    entity/mixer/ChannelEntity.h
    entity/mixer/ChannelEntity.cpp
    repository/mixer/ChannelRepository.h
    repository/mixer/ChannelRepository.cpp
    entity/mixer/Effect.h
    entity/mixer/Effect.cpp
    repository/mixer/EffectRepository.h
    repository/mixer/EffectRepository.cpp
    entity/mixer/Scene.h
    entity/mixer/Scene.cpp
    repository/mixer/SceneRepository.h
    repository/mixer/SceneRepository.cpp
    entity/musicLibrary/Track.h
    entity/musicLibrary/Track.cpp
    entity/musicLibrary/AudioFile.h
    entity/musicLibrary/AudioFile.cpp
    repository/musicLibrary/TrackRepository.h
    repository/musicLibrary/TrackRepository.cpp
    decorate/musicLibrary/TrackDecorate.h
    decorate/musicLibrary/TrackDecorate.cpp
)

target_link_libraries(GjDb
  PUBLIC
  Boost::log
  ${Boost_LIBRARIES}
  SQLite::SQLite3
)