cmake_minimum_required(VERSION 3.20)
project(strserver01)

set(CMAKE_CXX_STANDARD 14)

add_executable(strserver01 main.cpp ../../error.cpp ../../wrapsock.cpp ../../wrapunix.cpp
        ../../str_echo.cpp ../../signal.cpp ../../writen.cpp)
