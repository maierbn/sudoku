#include <iostream>

#include "sudoku.h"

#include "interleaved_sudoku.h"

int main(int argc, char **argv)
{
  // create sudoku

  Sudoku sudoku(5);
  sudoku.createSolution();
  sudoku.print();

  //sudoku.permuteNumbers();
  //sudoku.setFixed();
  //sudoku.print();
  exit(0);
  //
  //sudoku.print();


    /*
  InterleavedSudoku isudoku(3);
  isudoku.set(0,0,0,3);
  isudoku.set(5,0,0,4);
  isudoku.set(8,0,0,9);
  isudoku.set(4,1,0,2);
  isudoku.set(8,1,0,6);
  isudoku.set(2,2,0,2);
  isudoku.set(6,2,0,3);
  isudoku.set(0,3,0,8);
  isudoku.set(1,3,0,4);
  isudoku.set(4,3,0,9);
  isudoku.set(5,3,0,5);
  isudoku.set(6,3,0,2);
  isudoku.set(7,3,0,6);
  isudoku.set(8,3,0,3);
  isudoku.set(0,5,0,5);
  isudoku.set(1,5,0,2);
  isudoku.set(7,5,0,9);
  isudoku.set(1,6,0,7);
  isudoku.set(4,6,0,4);
  isudoku.set(2,7,0,1);
  isudoku.set(3,7,0,2);
  isudoku.set(7,8,0,1);
  isudoku.set(8,8,0,7);

  isudoku.set(2,0,1,6);
  isudoku.set(1,1,1,3);
  isudoku.set(3,1,1,1);
  isudoku.set(5,1,1,2);
  isudoku.set(8,1,1,8);
  isudoku.set(5,2,1,9);
  isudoku.set(6,2,1,7);
  isudoku.set(8,2,1,4);
  isudoku.set(4,3,1,8);
  isudoku.set(7,3,1,9);
  isudoku.set(1,6,1,5);
  isudoku.set(4,6,1,2);
  isudoku.set(6,6,1,1);
  isudoku.set(7,6,1,7);
  isudoku.set(3,7,1,8);
  isudoku.set(4,7,1,6);
  isudoku.set(6,7,1,2);
  isudoku.set(2,8,1,9);
  isudoku.set(5,8,1,7);
  isudoku.set(6,8,1,5);
  isudoku.set(7,8,1,8);
  isudoku.print();
  isudoku.solveExisting();
  exit(0);
  */
  for(;;)
  {

  InterleavedSudoku isudoku(4);
  isudoku.createSolution();
  //isudoku.print();
  isudoku.setFixed();
  isudoku.eraseNonFixed();
  isudoku.solveExisting();
  isudoku.print(1);
  //isudoku.permuteNumbers();
  //isudoku.print();

  }

  return 0;
}
