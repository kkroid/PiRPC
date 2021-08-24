cmake_minimum_required(VERSION 3.13)

project(PiRPC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/evpp/libevent/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include/evpp)

file(GLOB pirpc_s_srcs ${CMAKE_CURRENT_LIST_DIR}/server/*.cpp)
file(GLOB pirpc_c_srcs ${CMAKE_CURRENT_LIST_DIR}/client/*.cpp)
file(GLOB evpp_srcs ${CMAKE_CURRENT_LIST_DIR}/evpp/*.cc)

add_library(libevent SHARED IMPORTED)
set(libevent_dir ${CMAKE_CURRENT_LIST_DIR}/evpp/libevent/lib/)
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${libevent_dir}/pi/libevent-2.1.so.7.0.1)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${libevent_dir}/macos/libevent-2.1.7.dylib)
ENDIF()
add_library(PiRPC SHARED ${pirpc_s_srcs} ${pirpc_c_srcs} ${evpp_srcs})
target_link_libraries(PiRPC
        libevent
        )