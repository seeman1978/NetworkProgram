cmake_minimum_required(VERSION 3.20)
project(udpchsum)

set(CMAKE_CXX_STANDARD 14)

add_executable(udpchsum main.cpp pcap.cpp ../error.cpp ../host_serv.cpp ../sock_ntop.cpp ../wraplib.cpp
        ../sock_ntop_host.cpp ../sock_get_port.cpp ../wrapsock.cpp ../signal.cpp cleanup.cpp senddnsquery-raw.cpp udpchsum.cpp
        udpread.cpp udpwrite.cpp)
