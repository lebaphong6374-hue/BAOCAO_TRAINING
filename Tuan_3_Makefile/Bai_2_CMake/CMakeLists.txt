cmake_minimum_required(VERSION 3.16)

project(MathProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main
    main.cpp
    math_utils.cpp
)