CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(./protobuf.cmake)

project(Cheminot)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/fastmktime)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp/lib)
add_library(cheminotc "src/cheminotc.cpp" "src/play.cpp" "src/protobuf/cheminotBuf.pb.cc" "src/fastmktime/fastmktime.cpp")
target_link_libraries(cheminotc sqlite3 jsoncpp protobuf)

add_executable(main tests/main.cpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
target_link_libraries(main cheminotc)

enable_testing()
add_subdirectory (lib/gtest-1.7.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_executable(cheminotc_units tests/unittests.cpp)
target_link_libraries(cheminotc_units gtest_main cheminotc)
