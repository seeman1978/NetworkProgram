cmake_minimum_required(VERSION 3.20)
project(heartbeatcli)

set(CMAKE_CXX_STANDARD 14)

add_executable(heartbeatcli main.cpp ../../error.cpp ../../wrapsock.cpp ../../wrapunix.cpp ../../tcp_connect.cpp
        ../../signal.cpp ../../writen.cpp ../../wraplib.cpp)
