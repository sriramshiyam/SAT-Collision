cmake_minimum_required(VERSION 3.15)

project(SAT VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib CONFIG REQUIRED)

add_executable(sat main.cpp)

target_link_libraries(sat PRIVATE raylib)
