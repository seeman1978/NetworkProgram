cmake_minimum_required(VERSION 3.20)
project(serv05)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-DHAVE_MSGHDR_MSG_CONTROL)

add_executable(serv05 main.cpp ../../error.cpp ../../write_fd.cpp ../../tcp_listen.cpp ../../wrapstdio.cpp ../../wrapsock.cpp
        ../../wrapunix.cpp ../child05.cpp ../../signal.cpp ../../read_fd.cpp ../web_clild.cpp ../pr_cpu_time.cpp ../../readline.cpp ../../writen.cpp)
