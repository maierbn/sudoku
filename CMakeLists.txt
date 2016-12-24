cmake_minimum_required(VERSION 2.6)
project(sudoku)

add_executable(sudoku main.cpp sudoku.cpp interleaved_sudoku.cpp field.cpp)

add_definitions(--std=c++0x)

install(TARGETS sudoku RUNTIME DESTINATION bin)
