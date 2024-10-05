cmake_minimum_required(VERSION 3.5.0)
project(myn VERSION 0.1.0 LANGUAGES C CXX)

set(INCLUDE_DIRECTORIES include)
set(SOURCES src/main.cpp src/parser.cpp src/lexer.cpp)
include_directories(${INCLUDE_DIRECTORIES})

include(CTest)
enable_testing()

add_executable(myn ${SOURCES})

set_property(TARGET myn PROPERTY CXX_STANDARD 17)