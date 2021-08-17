cmake_minimum_required(VERSION 3.20)
project(openfile)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DHAVE_MSGHDR_MSG_CONTROL)

add_executable(openfile main.cpp ../../error.cpp ../../write_fd.cpp)
