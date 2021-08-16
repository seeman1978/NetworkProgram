cmake_minimum_required(VERSION 3.20)
project(strclient01)

set(CMAKE_CXX_STANDARD 14)

add_executable(strclient01 main.cpp ../../str_cli.cpp ../../wrapsock.cpp ../../error.cpp ../../wrapunix.cpp
        ../../writen.cpp ../../wraplib.cpp ../../wrapstdio.cpp ../../readline.cpp)
