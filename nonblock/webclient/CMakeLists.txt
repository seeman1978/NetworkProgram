cmake_minimum_required(VERSION 3.20)
project(webclient)

set(CMAKE_CXX_STANDARD 14)

add_executable(webclient web.cpp web.h ../../error.cpp ../../tcp_connect.cpp ../../wrapstdio.cpp ../../writen.cpp
        ../../host_serv.cpp ../../wrapsock.cpp ../../wrapunix.cpp)
