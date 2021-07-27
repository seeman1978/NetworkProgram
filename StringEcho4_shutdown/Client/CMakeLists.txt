cmake_minimum_required(VERSION 3.19)
project(Client)

set(CMAKE_CXX_STANDARD 14)

add_executable(Client main.cpp ../../str_cli.cpp ../../error.cpp ../../readline.cpp ../../wrapstdio.cpp
        ../../writen.cpp ../../wrapsock.cpp ../../wrapunix.cpp)