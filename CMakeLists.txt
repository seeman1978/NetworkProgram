cmake_minimum_required(VERSION 3.19)
project(NetworkProgram)

set(CMAKE_CXX_STANDARD 14)

add_executable(NetworkProgram main.cpp writen.cpp names/tcp_connect_cli/tcp_connect_cli.cpp)