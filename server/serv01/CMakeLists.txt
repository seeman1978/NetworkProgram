cmake_minimum_required(VERSION 3.20)
project(serv01)

set(CMAKE_CXX_STANDARD 14)

add_executable(serv01 main.cpp ../../error.cpp ../../tcp_listen.cpp ../../wrapsock.cpp ../../wrapunix.cpp ../../signal.cpp
        ../pr_cpu_time.cpp ../web_clild.cpp ../../readline.cpp ../../writen.cpp)
