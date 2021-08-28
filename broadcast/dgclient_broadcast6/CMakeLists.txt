cmake_minimum_required(VERSION 3.20)
project(dgclient_broadcast6)

set(CMAKE_CXX_STANDARD 14)

add_executable(dgclient_broadcast6 main.cpp ../../error.cpp ../../wrapunix.cpp ../../wrapsock.cpp
        ../../sock_ntop_host.cpp ../../wrapstdio.cpp ../../wraplib.cpp ../../signal.cpp)
