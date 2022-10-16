cmake_minimum_required(VERSION 3.24)
project(project2)

set(CMAKE_CXX_STANDARD 20)
include_directories(src/headers)
file(GLOB_RECURSE headers src/*.h)
file(GLOB_RECURSE sources src/*.cpp)
add_executable(project2 ${headers} ${sources})
