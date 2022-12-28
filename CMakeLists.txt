cmake_minimum_required(VERSION 3.5)

project(
  GdsViewer
  VERSION 0.1
  LANGUAGES CXX C)

if(NOT DEFINED ${CMAKE_BUILD_TYPE})
  set(CMAKE_BUILD_TYPE Debug)
endif()

add_subdirectory(thirdparty)
add_subdirectory(src)
