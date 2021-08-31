cmake_minimum_required(VERSION 3.20)
project(recv_send)

set(CMAKE_CXX_STANDARD 14)

add_executable(recv_send main.cpp ../../mcast_set_loop.cpp ../../sockfd_to_family.cpp ../../mcast_set_loop.cpp
        ../../error.cpp ../../sock_ntop.cpp ../../wrapsock.cpp ../../udp_client.cpp ../../wrapunix.cpp ../../mcast_join.cpp
        ../../family_to_level.cpp)
