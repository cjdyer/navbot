cmake_minimum_required(VERSION 3.3)
project(NAVBOT)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/build/debug")

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
include_directories(include)

file(GLOB sources src/*.cpp)

add_executable(NAVBOT ${sources}) 

target_link_libraries(NAVBOT PRIVATE Threads::Threads)