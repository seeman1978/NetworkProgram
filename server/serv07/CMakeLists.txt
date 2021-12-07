cmake_minimum_required(VERSION 3.20)
project(serv07)

set(CMAKE_CXX_STANDARD 14)

add_executable(serv07 main.cpp ../../error.cpp ../web_clild.cpp ../../tcp_listen.cpp ../../signal.cpp ../../wrapunix.cpp
        ../../wrapsock.cpp ../pr_cpu_time.cpp ../../readline.cpp ../../writen.cpp)
target_link_libraries(serv07 pthread)