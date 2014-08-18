cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -fcxx-exceptions -fexceptions -Weverything -pedantic -Wno-c++98-compat -Wno-padded")
set(LIBRARY_OUTPUT_PATH "../../lib/")

set(PROJECT zrpc)
set(CXX "/usr/bin/clang++")
project(${PROJECT} CXX)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(src/)

find_package(Boost COMPONENTS thread system REQUIRED)
find_library(ZMQ_LIB zmq)
set(LIBS ${ZMQ_LIB} ${Boost_LIBRARIES})

aux_source_directory(src/transport SRC_TRANSPORT)
set(SRCS ${SRC_TRANSPORT})

add_library(${PROJECT} STATIC ${SRCS})
target_link_libraries(${PROJECT} ${LIBS})
set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME ${PROJECT}_static)
