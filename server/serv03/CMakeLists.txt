cmake_minimum_required(VERSION 3.20)
project(serv03)

set(CMAKE_CXX_STANDARD 14)

add_executable(serv03 main.cpp ../../tcp_listen.cpp ../../error.cpp ../pr_cpu_time.cpp ../../signal.cpp ../../wrapsock.cpp
        ../../wrapunix.cpp ../web_clild.cpp ../child03.cpp ../../writen.cpp ../../readline.cpp ../meter.cpp)
