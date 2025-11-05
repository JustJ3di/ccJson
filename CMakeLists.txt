cmake_minimum_required(VERSION 4.0)
project(ccJson)

set(CMAKE_CXX_STANDARD 20)

add_executable(ccJson main.cpp
        JSON.hpp
        JSON.cpp
)
