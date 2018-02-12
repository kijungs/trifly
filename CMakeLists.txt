cmake_minimum_required(VERSION 3.6)
project(17_triangle_basic)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        include/catch.hpp
        src/base_struct.hpp
        src/io.cpp
        src/io.hpp
        src/macro.hpp
        src/main.cpp
        src/option.cpp
        src/option.hpp
        src/run.cpp
        src/run.hpp
        src/source.cpp
        src/source.hpp
        src/worker.cpp
        src/worker.hpp)

add_executable(17_triangle_basic ${SOURCE_FILES})