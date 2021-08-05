cmake_minimum_required(VERSION 3.20)
project(Client)

set(CMAKE_CXX_STANDARD 14)

add_executable(Client main.cpp ../../error.cpp ../../wrapsock.cpp ../../wrapstdio.cpp
        ../../wrapunix.cpp ../../sock_ntop.cpp)
