cmake_minimum_required(VERSION 3.20)
project(serv08)

set(CMAKE_CXX_STANDARD 20)

add_executable(serv08 main.cpp ../../error.cpp ../../tcp_listen.cpp ../../wrapsock.cpp ../../wrapunix.cpp ../web_clild.cpp
        ../pr_cpu_time.cpp ../../readline.cpp ../../writen.cpp ../../signal.cpp)

target_link_libraries(serv08 pthread)
