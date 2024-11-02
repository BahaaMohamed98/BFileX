cmake_minimum_required(VERSION 3.25)
project(BfileX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

include_directories(src/TerminalPlusPlus/src)

add_executable(BfileX src/BFileX.cpp
        src/TerminalPlusPlus/src/Terminal++.hpp
        src/UI.hpp
        src/FileManager.hpp
        src/InputHandler.hpp
        src/App.hpp
        src/BFileX.hpp
        src/Config.hpp)
add_executable(test src/test.cpp)
