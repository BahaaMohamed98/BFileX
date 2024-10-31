cmake_minimum_required(VERSION 3.28)
project(TerminalPlusPlus)

set(CMAKE_CXX_STANDARD 17)

add_executable(TerminalPlusPlus src/examples/example.cpp)
add_executable(CountDown src/examples/countDown.cpp)
add_executable(test test.cpp)
include_directories(src)
