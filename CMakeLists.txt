cmake_minimum_required(VERSION 3.5)

project(PiRPC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
include_directories(${CMAKE_CURRENT_LIST_DIR}/evpp/public)
include_directories(${CMAKE_CURRENT_LIST_DIR}/common/public)
include_directories(${CMAKE_CURRENT_LIST_DIR}/server/public)
include_directories(${CMAKE_CURRENT_LIST_DIR}/client/public)
include_directories(${CMAKE_CURRENT_LIST_DIR}/spdlog)
include_directories(${CMAKE_CURRENT_LIST_DIR}/cptl)
include_directories(${CMAKE_CURRENT_LIST_DIR}/json)

file(GLOB pirpc_s_srcs ${CMAKE_CURRENT_LIST_DIR}/server/private/*.cpp)
file(GLOB pirpc_c_srcs ${CMAKE_CURRENT_LIST_DIR}/client/private/*.cpp)

add_subdirectory(evpp)

add_library(PiRPC SHARED ${pirpc_s_srcs} ${pirpc_c_srcs})
target_link_libraries(PiRPC
        evpp)