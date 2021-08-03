cmake_minimum_required(VERSION 3.20)
project(udp_getsockname)

set(CMAKE_CXX_STANDARD 14)

add_executable(udp_getsockname main.cpp ../wrapsock.cpp ../error.cpp ../wraplib.cpp  ../sock_ntop.cpp)
