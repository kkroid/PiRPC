set(CMAKE_MACOSX_RPATH 0)

include_directories(${CMAKE_CURRENT_LIST_DIR}/../libevent/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/public)
file(GLOB evpp_srcs ${CMAKE_CURRENT_LIST_DIR}/private/*.cc)

add_library(libevent SHARED IMPORTED)
IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "current platform: Linux ")
    set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/../libevent/lib/pi/libevent-2.1.so.7.0.1)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "current platform: Windows")
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    MESSAGE(STATUS "current platform: Darwin ")
    set_target_properties(libevent PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/../libevent/lib/macos/libevent-2.1.7.dylib)
ENDIF ()

add_library(evpp SHARED ${evpp_srcs})
target_link_libraries(evpp
        libevent
        pthread)

#add_subdirectory(examples)