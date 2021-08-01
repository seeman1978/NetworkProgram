cmake_minimum_required(VERSION 3.19)
project(client)

set(CMAKE_CXX_STANDARD 14)

add_executable(client main.cpp ../../wraplib.cpp ../../error.cpp ../../wrapstdio.cpp
        ../../wrapsock.cpp ../../sock_ntop.cpp ../../wrapunix.cpp)