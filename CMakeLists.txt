CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ALGO_ON_SERVER)
SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

INCLUDE_DIRECTORIES(./include)

ADD_EXECUTABLE(algo_on_server
    src/algo_on_server.cc
    src/base_on_server.cc)

ADD_EXECUTABLE(bf_on_server
    src/bf_on_server.cc
    src/base_on_server.cc)
    