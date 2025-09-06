cmake_minimum_required(VERSION 3.5)
project(LeetCode)

set(CMAKE_CXX_STANDARD 17)

include_directories(.
    ./vcpkg_installed/x64-windows/include)

add_executable(LeetCode
        ProgramingEaxm.cpp
        ProgramingEaxm.h
        solutions.cpp
        helper.cpp
        helper.h)
