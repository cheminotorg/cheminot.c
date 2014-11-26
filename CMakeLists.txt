CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(./protobuf.cmake)

project(Cheminot)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp/lib)
add_library(cheminotc "cheminotc.cpp" "protobuf/cheminotBuf.pb.cc")
target_link_libraries(cheminotc sqlite3 jsoncpp protobuf)

add_subdirectory (lib/gtest-1.7.0)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(runUnitTests tests/unittests.cpp)
target_link_libraries(runUnitTests gtest gtest_main cheminotc)
add_test(rununittests runUnitTests)
add_executable(cheminotc_units tests/unittests.cpp)
target_link_libraries(cheminotc_units gtest_main cheminotc)
