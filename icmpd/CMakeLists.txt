cmake_minimum_required(VERSION 3.18)
project(icmpd)

set(CMAKE_CXX_STANDARD 14)
ADD_DEFINITIONS(-DHAVE_MSGHDR_MSG_CONTROL)
add_executable(icmpd "main.cpp" "../write_fd.cpp" "../wrapsock.cpp" "../sock_bind_wild.cpp" "../error.cpp" "../wrapunix.cpp" 
        "../wrapstdio.cpp" "../sock_ntop.cpp")
