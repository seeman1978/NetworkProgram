cmake_minimum_required(VERSION 3.20)
project(serv02)

set(CMAKE_CXX_STANDARD 17)

add_executable(serv02 main.cpp ../../tcp_listen.cpp ../../error.cpp ../pr_cpu_time.cpp ../../signal.cpp ../../wrapsock.cpp
        ../../wrapunix.cpp ../web_clild.cpp ../child02.cpp ../../writen.cpp ../../readline.cpp ../meter.cpp)
