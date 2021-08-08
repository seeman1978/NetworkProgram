cmake_minimum_required(VERSION 3.20)
project(tcp_connect_cli)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_connect_cli tcp_connect_cli.cpp ../../tcp_connect.cpp ../../wrapsock.cpp ../../sock_ntop_host.cpp
        ../../error.cpp ../../wrapstdio.cpp ../../wrapunix.cpp)
