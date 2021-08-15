cmake_minimum_required(VERSION 3.20)
project(unixbind)

set(CMAKE_CXX_STANDARD 14)

add_executable(unixbind main.cpp ../../wrapsock.cpp ../../error.cpp)
