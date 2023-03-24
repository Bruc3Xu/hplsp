project(unix_network)
cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
include_directories(${CMAKE_SOURCE_DIR}/netbridge)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/libs)
link_directories(${CMAKE_SOURCE_DIR}/libs)

add_subdirectory(chapter5)
add_subdirectory(chapter7)

add_subdirectory(netbridge)

add_subdirectory(tests)