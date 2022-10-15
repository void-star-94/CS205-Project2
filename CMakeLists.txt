cmake_minimum_required(VERSION 3.24)
project(project2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpthread -lncurses")
include_directories(src/headers)
file(GLOB_RECURSE headers src/*.h)
file(GLOB_RECURSE sources src/*.cpp)
add_executable(project2 ${headers} ${sources})
