cmake_minimum_required(VERSION 3.20)
project(tcp_listen_srv1)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_listen_srv1 main.cpp ../../tcp_listen.cpp ../../wrapsock.cpp ../../error.cpp ../../sock_ntop.cpp
        ../../wrapunix.cpp)
