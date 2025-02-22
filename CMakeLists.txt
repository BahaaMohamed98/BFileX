cmake_minimum_required(VERSION 3.25)
project(BFileX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

include_directories(include/Terminal++/src src/include)

add_executable(BFileX src/main.cpp
        include/Terminal++/src/Terminal++.hpp
        src/include/App.hpp
        src/include/BFileX.hpp
        src/include/FileManager.hpp
        src/include/FileProperties.hpp
        src/include/InputHandler.hpp
        src/include/UI.hpp
        src/include/FilePreview.hpp
        src/App.cpp
        src/BFileX.cpp
        src/FileManager.cpp
        src/FileProperties.cpp
        src/InputHandler.cpp
        src/UI.cpp
        src/FilePreview.cpp
        src/CommandLineParser.cpp
        src/include/CommandLineParser.hpp
        src/include/debug.hpp
)
