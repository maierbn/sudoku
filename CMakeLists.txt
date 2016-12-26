cmake_minimum_required(VERSION 2.6)
project(sudoku)

add_definitions(--std=c++0x -g)
add_executable(sudoku main.cpp sudoku.cpp interleaved_sudoku.cpp field.cpp compare_fields.cpp chained_sudoku.cpp)


install(TARGETS sudoku RUNTIME DESTINATION bin)
