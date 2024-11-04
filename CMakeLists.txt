cmake_minimum_required(VERSION 3.25)
project(BFileX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

include_directories(src/TerminalPlusPlus/src)
include_directories(src/include)
include_directories(src)

add_executable(BFileX src/main.cpp
        src/TerminalPlusPlus/src/Terminal++.hpp
        src/include/UI.hpp
        src/include/FileManager.hpp
        src/include/InputHandler.hpp
        src/include/App.hpp
        src/include/BFileX.hpp
        src/include/FileProperties.hpp
        src/App.cpp
        src/BFileX.cpp
        src/FileManager.cpp
        src/InputHandler.cpp
        src/FileProperties.cpp
        src/UI.cpp
        src/Preview.cpp
        src/include/Preview.hpp
)

add_executable(Build src/Build.cpp)

add_executable(test src/test.cpp)
