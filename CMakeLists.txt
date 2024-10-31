cmake_minimum_required(VERSION 3.25)
project(BfileX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

include_directories(src/TerminalPlusPlus/src)

add_executable(BfileX src/main.cpp)
