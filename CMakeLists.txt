cmake_minimum_required(VERSION 3.25)
project(BFileX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

include_directories(include/Terminal++/src src/include)

add_executable(BFileX src/main.cpp
        include/Terminal++/src/Terminal++.hpp
        src/App.hpp
        src/BFileX.hpp
        src/FileManager.hpp
        src/FileProperties.hpp
        src/InputHandler.hpp
        src/UI.hpp
        src/FilePreview.hpp
        src/App.cpp
        src/BFileX.cpp
        src/FileManager.cpp
        src/FileProperties.cpp
        src/InputHandler.cpp
        src/UI.cpp
        src/FilePreview.cpp
        src/CommandLineParser.cpp
        src/CommandLineParser.hpp
        src/debug.hpp
)
