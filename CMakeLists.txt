CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(ALGO_ON_SERVER)
SET(CMAKE_CXX_FLAGS "-std=c++11 -O2")
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

INCLUDE_DIRECTORIES(./include)

# ADD_EXECUTABLE(bf_enum
#     src/bf_enum.cc
#     src/utils.cc)

ADD_EXECUTABLE(bf_search
    src/bf_search.cc
    src/utils.cc)

ADD_EXECUTABLE(adjacent_search
    src/adjacent_search.cc
    src/utils.cc)

ADD_EXECUTABLE(adjacent_search_opt
    src/adjacent_search_opt.cc
    src/utils.cc)
    