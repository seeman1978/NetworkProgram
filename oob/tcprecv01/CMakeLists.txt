cmake_minimum_required(VERSION 3.20)
project(tcprecv01)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcprecv01 main.cpp ../../error.cpp ../../wrapsock.cpp ../../tcp_listen.cpp ../../signal.cpp ../../wrapunix.cpp
        ../../wraplib.cpp)
