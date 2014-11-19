CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(Cheminot)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -std=c++11")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp)
LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp/lib)
ADD_LIBRARY(cheminotc "cheminotc.cpp")
TARGET_LINK_LIBRARIES(cheminotc sqlite3)
TARGET_LINK_LIBRARIES(cheminotc jsoncpp)

ADD_SUBDIRECTORY (lib/gtest-1.7.0)
ENABLE_TESTING()
INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
ADD_EXECUTABLE( runUnitTests tests/unittests.cpp )
TARGET_LINK_LIBRARIES(runUnitTests gtest gtest_main cheminotc)
ADD_TEST( runUnitTests runUnitTests )

add_executable(cheminotc_units tests/unittests.cpp)
target_link_libraries(cheminotc_units gtest_main cheminotc sqlite3)
