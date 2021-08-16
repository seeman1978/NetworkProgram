cmake_minimum_required(VERSION 3.20)
project(dgclient01)

set(CMAKE_CXX_STANDARD 14)

add_executable(dgclient01 main.cpp ../../dg_cli.cpp ../../error.cpp ../../wraplib.cpp ../../wrapsock.cpp ../../wrapstdio.cpp)
