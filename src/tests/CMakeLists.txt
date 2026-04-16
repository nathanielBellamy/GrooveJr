cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Prevent GTest from overriding compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Project source root for clean includes (e.g. #include "audio/engine/Math.h")
set(GJ_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Find optional system headers needed by some entity headers
find_path(SNDFILE_INCLUDE_DIR sndfile.h)
find_path(JACK_INCLUDE_DIR jack/jack.h)

# ── gj_test_pure: header-only tests, no project library dependencies ──
add_executable(gj_test_pure
    test_math.cpp
    test_playstate.cpp
    test_atomic_str.cpp
    test_color.cpp
    test_paths.cpp
)
target_include_directories(gj_test_pure PRIVATE ${GJ_SRC_ROOT})
target_link_libraries(gj_test_pure PRIVATE GTest::gtest_main)

# ── gj_test_db: entity tests, links GjDb ──
add_executable(gj_test_db
    test_entities.cpp
)
target_include_directories(gj_test_db PRIVATE ${GJ_SRC_ROOT})
if(SNDFILE_INCLUDE_DIR)
    target_include_directories(gj_test_db PRIVATE ${SNDFILE_INCLUDE_DIR})
endif()
if(JACK_INCLUDE_DIR)
    target_include_directories(gj_test_db PRIVATE ${JACK_INCLUDE_DIR})
endif()
target_link_libraries(gj_test_db PRIVATE GTest::gtest_main GjDb)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(gj_test_pure)
gtest_discover_tests(gj_test_db)
