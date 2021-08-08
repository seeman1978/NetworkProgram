cmake_minimum_required(VERSION 3.20)
project(udp_client_cli1)

set(CMAKE_CXX_STANDARD 14)

add_executable(udp_client_cli1 main.cpp ../../error.cpp ../../udp_client.cpp ../../sock_ntop_host.cpp ../../wrapstdio.cpp
        ../../wrapsock.cpp ../../wrapunix.cpp)
