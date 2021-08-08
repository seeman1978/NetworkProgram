cmake_minimum_required(VERSION 3.20)
project(udp_server1)

set(CMAKE_CXX_STANDARD 14)

add_executable(udp_server1 main.cpp ../../wrapsock.cpp ../../error.cpp ../../sock_ntop.cpp
        ../../wrapunix.cpp ../../udp_server.cpp)
