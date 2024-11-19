cmake_minimum_required(VERSION 3.0.0)
project(Interpreter VERSION 0.1.0)

find_package(spdlog REQUIRED)
find_package(GTest CONFIG REQUIRED)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

set(SOURCES main.cpp
    src/Ast.cpp
    src/Interpreter.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Token.cpp
)

add_executable(Interpreter ${SOURCES})

target_include_directories(Interpreter PUBLIC ./src)

target_link_libraries(Interpreter
    spdlog::spdlog
    spdlog
)
