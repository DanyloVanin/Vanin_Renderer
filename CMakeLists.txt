cmake_minimum_required(VERSION 3.27)
project(Vanin_Renderer)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(Vanin_Renderer main.cpp
        tgaimage.h
        tgaimage.cpp
        model.h
        geometry.h
        model.cpp
        geometry.cpp)

target_link_libraries(Vanin_Renderer sfml-graphics sfml-audio)