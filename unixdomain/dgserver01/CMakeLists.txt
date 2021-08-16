cmake_minimum_required(VERSION 3.20)
project(dgserver01)

set(CMAKE_CXX_STANDARD 14)

add_executable(dgserver01 main.cpp ../../dg_echo.cpp ../../wrapsock.cpp ../../error.cpp)
