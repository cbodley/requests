cmake_minimum_required (VERSION 3.5.1)
project(requests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(requests INTERFACE)
target_include_directories(requests INTERFACE include)

enable_testing()
add_subdirectory(test EXCLUDE_FROM_ALL)
