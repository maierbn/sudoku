cmake_minimum_required(VERSION 2.6)
project(sudoku)

add_definitions(--std=c++0x -g)
add_executable(sudoku 
  src/main.cpp 
  src/sudoku.cpp 
  src/interleaved_sudoku.cpp 
  src/field.cpp 
  src/compare_fields.cpp 
  src/chained_sudoku.cpp 
  src/sudoku_chain.cpp
  src/train_sudoku.cpp
)


#install(TARGETS sudoku RUNTIME DESTINATION bin)
