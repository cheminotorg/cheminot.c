CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE(../protobuf.cmake)

project(cheminot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -DHAVE_PTHREAD=1")

include_directories(${CMAKE_CURRENT_BINARY_DIR}/lib/jsoncpp)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/cheminotc)
#include_directories(/usr/lib/jvm/java-7-openjdk-amd64/include/)
include_directories(/Library/Java/JavaVirtualMachines/jdk1.8.0_40.jdk/Contents/Home/include)
include_directories(/Library/Java/JavaVirtualMachines/jdk1.8.0_40.jdk/Contents/Home/include/darwin)

add_library(jsoncpp "lib/jsoncpp/jsoncpp.cpp")
add_library(cheminotc "src/cheminotc/cheminotc.cpp" "src/cheminotc/protobuf/cheminotBuf.pb.cc")
add_library(cheminot MODULE "src/cheminot.cpp")

target_link_libraries(cheminot cheminotc sqlite3 jsoncpp protobuf)
