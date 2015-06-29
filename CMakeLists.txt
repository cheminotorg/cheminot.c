CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(./protobuf.cmake)

set_source_files_properties(settings.h PROPERTIES GENERATED true)
add_custom_command(OUTPUT settings.h
    COMMAND sh settings.sh src/
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

project(Cheminot)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DHAVE_PTHREAD=1")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp)
add_library(jsoncpp "lib/jsoncpp/jsoncpp.cpp")
add_library(cheminotc settings.h "src/cheminotc.cpp" "src/play.cpp" "src/protobuf/cheminotBuf.pb.cc")
target_link_libraries(cheminotc sqlite3 jsoncpp protobuf)

add_executable(main tests/main.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
target_link_libraries(main cheminotc)

enable_testing()
add_subdirectory (lib/gtest-1.7.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(cheminotc_units tests/unittests.cpp)
target_link_libraries(cheminotc_units gtest_main cheminotc)
