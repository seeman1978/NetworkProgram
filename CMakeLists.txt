cmake_minimum_required(VERSION 3.18)
project(NetworkProgram)

set(CMAKE_CXX_STANDARD 14)

add_executable(NetworkProgram main.cpp "udpchsum/pcap.cpp")