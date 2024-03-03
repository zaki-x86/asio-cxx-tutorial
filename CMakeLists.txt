cmake_minimum_required(VERSION 3.8)

project(asio-tutorial)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -I C:\\SDKs\\asio-1.28.0\\include")

add_executable(repeater_ex repeater/main.cpp)
add_executable(multithreaded_handlers multithreaded_handlers/main.cpp)

add_executable(client_sync networking/client_sync.cpp)
add_executable(client_async networking/client_async.cpp)

add_executable(server_sync networking/server_sync.cpp)
add_executable(server_async networking/server_async.cpp)
