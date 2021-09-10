cmake_minimum_required(VERSION 3.20)
project(ping)

set(CMAKE_CXX_STANDARD 14)

add_executable(ping main.cpp ../error.cpp ../sock_ntop_host.cpp ../signal.cpp ../host_serv.cpp ../wrapsock.cpp ../wrapunix.cpp)
