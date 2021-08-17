cmake_minimum_required(VERSION 3.20)
project(readcred)

set(CMAKE_CXX_STANDARD 14)

add_executable(readcred main.cpp readcred.cpp ../../error.cpp ../str_echo.cpp ../../wrapsock.cpp ../../writen.cpp)
