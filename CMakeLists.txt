cmake_minimum_required(VERSION 3.20)

project(
  labwork8
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
enable_testing()
add_subdirectory(tests)
