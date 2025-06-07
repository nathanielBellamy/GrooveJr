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
    entity/ChannelEntity.h.h
    entity/ChannelEntity.cpp
    entity/Effect.h
    entity/Effect.cpp
    repository/EffectRepository.h
    repository/EffectRepository.cpp
    entity/Scene.h
    entity/Scene.cpp
    repository/SceneRepository.h
    repository/SceneRepository.cpp
    entity/Track.h
    entity/Track.cpp
    repository/TrackRepository.h
    repository/TrackRepository.cpp
)

target_link_libraries(GjDb
  PUBLIC
  Boost::log
  ${Boost_LIBRARIES}
  SQLite::SQLite3
)