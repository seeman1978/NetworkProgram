cmake_minimum_required(VERSION 3.20)
project(Client)

set(CMAKE_CXX_STANDARD 14)

add_executable(Client main.cpp ../../../readable_timeo.cpp ../../../wraplib.cpp ../../../error.cpp
        ../../../wrapstdio.cpp ../../../wrapsock.cpp)
