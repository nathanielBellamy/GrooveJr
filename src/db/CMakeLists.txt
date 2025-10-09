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
        Dao.h
        Dao.cpp
        Logging.h
        Logging.cpp
        Types.h
        dto/musicLibrary/TrackDecoratedDto.h
        dto/musicLibrary/TrackDecoratedDto.cpp
        entity/AppStateEntity.h
        entity/AppStateEntity.cpp
        entity/mixer/ChannelEntity.h
        entity/mixer/ChannelEntity.cpp
        entity/mixer/Effect.h
        entity/mixer/Effect.cpp
        entity/mixer/Scene.h
        entity/mixer/Scene.cpp
        entity/musicLibrary/Track.h
        entity/musicLibrary/Track.cpp
        entity/musicLibrary/AudioFile.h
        entity/musicLibrary/AudioFile.cpp
        entity/musicLibrary/Artist.h
        entity/musicLibrary/Artist.cpp
        entity/musicLibrary/Album.h
        entity/musicLibrary/Album.cpp
        entity/musicLibrary/Genre.h
        entity/musicLibrary/Genre.cpp
        repository/AppStateRepository.h
        repository/AppStateRepository.cpp
        repository/mixer/ChannelRepository.h
        repository/mixer/ChannelRepository.cpp
        repository/mixer/EffectRepository.h
        repository/mixer/EffectRepository.cpp
        repository/mixer/SceneRepository.h
        repository/mixer/SceneRepository.cpp
        repository/musicLibrary/AlbumRepository.h
        repository/musicLibrary/AlbumRepository.cpp
        repository/musicLibrary/ArtistRepository.h
        repository/musicLibrary/ArtistRepository.cpp
        repository/musicLibrary/AudioFileRepository.h
        repository/musicLibrary/AudioFileRepository.cpp
        repository/musicLibrary/GenreRepository.h
        repository/musicLibrary/GenreRepository.cpp
        repository/musicLibrary/TrackRepository.h
        repository/musicLibrary/TrackRepository.cpp
)

target_link_libraries(GjDb
  PUBLIC
  Boost::log
  ${Boost_LIBRARIES}
  SQLite::SQLite3
  -L${C_LIBS_HOME}/libsndfile/1.2.2/lib
  -lsndfile
)