cmake_minimum_required(VERSION 3.20)
project(strclifork)

set(CMAKE_CXX_STANDARD 14)

add_executable(strclifork main.cpp ../../wrapunix.cpp ../../error.cpp
        ../../wrapsock.cpp ../../wrapstdio.cpp ../../writen.cpp ../../readline.cpp ../strclifork.cpp)
