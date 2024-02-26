cmake_minimum_required(VERSION 3.8)

project(asio-tutorial)

add_executable(repeater_ex repeater/main.cpp)
add_executable(multithreaded_handlers multithreaded_handlers/main.cpp)
add_executable(client networking/main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -I C:\\SDKs\\asio-1.28.0\\include")