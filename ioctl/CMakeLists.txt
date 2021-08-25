cmake_minimum_required(VERSION 3.20)
project(ioctl)

set(CMAKE_CXX_STANDARD 14)

add_executable(ioctl main.cpp ../error.cpp ../wrapsock.cpp ../sock_ntop.cpp ../get_ifi_info.cpp ../wrapunix.cpp)
