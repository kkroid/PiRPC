cmake_minimum_required(VERSION 3.5)

project(PiRPC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
include_directories(${CMAKE_CURRENT_LIST_DIR}/evpp/public)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB pirpc_s_srcs ${CMAKE_CURRENT_LIST_DIR}/server/*.cpp)
file(GLOB pirpc_c_srcs ${CMAKE_CURRENT_LIST_DIR}/client/*.cpp)

add_subdirectory(evpp)

add_library(PiRPC SHARED ${pirpc_s_srcs} ${pirpc_c_srcs})
target_link_libraries(PiRPC
        evpp)