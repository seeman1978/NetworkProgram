cmake_minimum_required(VERSION 3.20)
project(serv06)

set(CMAKE_CXX_STANDARD 14)

add_executable(serv06 main.cpp ../pr_cpu_time.cpp ../web_clild.cpp ../../wrapsock.cpp ../../tcp_listen.cpp ../../wrapunix.cpp ../../signal.cpp
        ../../error.cpp ../../readline.cpp ../../writen.cpp)

target_link_libraries(serv06 pthread)