cmake_minimum_required(VERSION 3.20)
project(strclinonb)

set(CMAKE_CXX_STANDARD 14)

add_executable(strclinonb main.cpp ../str_cli_nonblock.cpp ../../wrapunix.cpp ../../error.cpp ../gf_time.cpp
        ../../wrapsock.cpp ../../wrapstdio.cpp ../../writen.cpp ../../readline.cpp)
