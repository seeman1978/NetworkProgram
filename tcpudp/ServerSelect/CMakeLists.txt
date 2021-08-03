cmake_minimum_required(VERSION 3.20)
project(ServerSelect)

set(CMAKE_CXX_STANDARD 14)

add_executable(ServerSelect main.cpp ../../str_echo.cpp ../../wrapsock.cpp ../../writen.cpp
        ../../error.cpp ../../signal.cpp ../../wrapunix.cpp)
