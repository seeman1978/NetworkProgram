cmake_minimum_required(VERSION 3.20)
project(mysdr)

set(CMAKE_CXX_STANDARD 14)

add_executable(mysdr main.cpp ../../mcast_join.cpp ../../family_to_level.cpp ../../error.cpp ../../udp_client.cpp
        ../../wrapsock.cpp ../../wrapunix.cpp ../../sock_ntop.cpp)
