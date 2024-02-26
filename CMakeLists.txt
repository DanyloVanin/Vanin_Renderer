cmake_minimum_required(VERSION 3.27)
project(Vanin_Renderer)

set(CMAKE_CXX_STANDARD 17)

add_executable(Vanin_Renderer main.cpp
        tgaimage.h
        tgaimage.cpp)
