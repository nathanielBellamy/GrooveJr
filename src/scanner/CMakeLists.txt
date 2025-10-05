cmake_minimum_required(VERSION 3.29.6)

project(GjScanner VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(LLVM_INSTALL_DIR /opt/homebrew/opt/llvm)

# logging
find_package(Boost REQUIRED COMPONENTS log_setup log)

# TagLib
find_package(TagLib REQUIRED)

add_library(GjScanner SHARED
        Logging.h
        Logging.cpp
        Scanner.h
        Scanner.cpp
)

target_link_libraries(GjScanner
        PUBLIC
        Boost::log
        ${Boost_LIBRARIES}
        TagLib::TagLib
        GjDb
)
