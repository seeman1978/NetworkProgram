cmake_minimum_required(VERSION 3.21)
project(HelloWorld)

set(CMAKE_CXX_STANDARD 17)

add_executable(HelloWorld main.cpp)

target_link_libraries(HelloWorld event)
