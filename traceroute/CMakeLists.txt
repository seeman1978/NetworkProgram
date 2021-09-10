cmake_minimum_required(VERSION 3.20)
project(traceroute)

set(CMAKE_CXX_STANDARD 14)

add_executable(traceroute main.cpp ../error.cpp ../sock_ntop_host.cpp ../host_serv.cpp ../wrapunix.cpp ../wrapsock.cpp ../signal.cpp)
