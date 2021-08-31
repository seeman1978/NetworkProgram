cmake_minimum_required(VERSION 3.20)
project(sntp)

set(CMAKE_CXX_STANDARD 14)

add_executable(sntp main.cpp sntp_proc.cpp ../../wrapunix.cpp ../../sock_set_wild.cpp ../../sock_set_addr.cpp ../../error.cpp ../../udp_client.cpp
        ../../sock_ntop.cpp ../../wrapsock.cpp)
