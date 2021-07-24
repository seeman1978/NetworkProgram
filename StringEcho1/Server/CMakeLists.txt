cmake_minimum_required(VERSION 3.19)
project(Server)

set(CMAKE_CXX_STANDARD 14)

add_executable(Server main.cpp ../../str_echo.cpp ../../wrapsock.cpp ../../writen.cpp ../../error.cpp)