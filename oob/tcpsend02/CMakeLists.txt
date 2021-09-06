cmake_minimum_required(VERSION 3.20)
project(tcpsend02)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcpsend02 main.cpp  ../../error.cpp ../../wrapsock.cpp ../../wrapunix.cpp ../../tcp_connect.cpp)
