cmake_minimum_required(VERSION 3.20)
project(mycat)

set(CMAKE_CXX_STANDARD 14)

add_definitions(-DHAVE_MSGHDR_MSG_CONTROL)

add_executable(mycat main.cpp ../myopen.cpp ../../wrapsock.cpp ../../read_fd.cpp ../../error.cpp ../../wrapunix.cpp)
