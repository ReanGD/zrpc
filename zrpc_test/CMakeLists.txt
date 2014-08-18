cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(EXECUTABLE_OUTPUT_PATH "../../bin/")

set(PROJECT zrpc_test)
set(CXX "/usr/bin/clang++")
project(${PROJECT} CXX)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIR})
include_directories(../zrpc/src/)

find_package(Boost COMPONENTS thread system REQUIRED)
find_package(GTest REQUIRED)
find_library(ZMQ_LIB zmq)
find_library(ZRPC_LIB zrpc_static ../lib/)
set(LIBS ${ZRPC_LIB} ${ZMQ_LIB} ${Boost_LIBRARIES} ${GTEST_LIBRARY})

aux_source_directory(. SRC_MAIN)
aux_source_directory(test/ SRC_TEST)
aux_source_directory(test/helpers/ SRC_HELPERS)
set(SRCS ${SRC_MAIN} ${SRC_TEST} ${SRC_HELPERS})

set(GTEST_FLAGS "-Wno-global-constructors -Wno-weak-vtables")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GTEST_FLAGS} -std=c++11 -pthread -fcxx-exceptions -fexceptions -Weverything -pedantic -Wno-c++98-compat -Wno-padded")

add_executable(${PROJECT} ${SRCS})
target_link_libraries(${PROJECT} ${LIBS})
