#include <iostream>

#include "sudoku.h"

#include "interleaved_sudoku.h"
#include "chained_sudoku.h"

int main(int argc, char **argv)
{
  // create sudoku
#if 1
  Sudoku sudoku(3);

  sudoku.set(0, 5, 3, true);
  sudoku.set(1, 5, 6, true);
  sudoku.set(2, 5, 9, true);
  sudoku.set(0, 6, 2, true);
  sudoku.set(1, 6, 1, true);
  sudoku.set(2, 6, 8, true);
  sudoku.set(0, 7, 5, true);
  sudoku.set(1, 7, 4, true);
  sudoku.set(2, 7, 7, true);

  sudoku.createSolution();
  sudoku.print(2, "E_solution.svg", 3);

  //sudoku.permuteNumbers();
  sudoku.setOneFixed(0, 5, 3, false);
  sudoku.setOneFixed(1, 5, 6, false);
  sudoku.setOneFixed(2, 5, 9, false);
  sudoku.setOneFixed(0, 6, 2, false);
  sudoku.setOneFixed(1, 6, 1, false);
  sudoku.setOneFixed(2, 6, 8, false);
  sudoku.setOneFixed(0, 7, 5, false);
  sudoku.setOneFixed(1, 7, 4, false);
  sudoku.setOneFixed(2, 7, 7, false);
  sudoku.setFixed();
  sudoku.print(2, "EE.svg", 0);

  sudoku.eraseNonFixed();
  sudoku.solveExisting();
  sudoku.print(2, "E.svg", 0);

  exit(0);
#endif
  ChainedSudoku chainedSudoku;

  chainedSudoku.print(0, "", 1);
  chainedSudoku.print(0, "", 2);

  chainedSudoku.set(0,5);
  chainedSudoku.set(4,3);
  chainedSudoku.set(5,9);
  chainedSudoku.set(10,4);
  chainedSudoku.set(17,9);
  chainedSudoku.set(19,8);
  chainedSudoku.set(20,7);
  chainedSudoku.set(22,2);
  chainedSudoku.set(23,4);
  chainedSudoku.set(24,5);
  chainedSudoku.set(25,3);
  chainedSudoku.set(31,6);
  chainedSudoku.set(32,3);
  chainedSudoku.set(37,5);
  chainedSudoku.set(40,9);
  chainedSudoku.set(43,1);
  chainedSudoku.set(46,9);
  chainedSudoku.set(47,4);
  chainedSudoku.set(48,7);
  chainedSudoku.set(49,5);
  chainedSudoku.set(51,6);
  chainedSudoku.set(52,8);
  chainedSudoku.set(54,2);
  chainedSudoku.set(56,5);
  chainedSudoku.set(63,4);
  chainedSudoku.set(64,6);
  chainedSudoku.set(65,9);
  chainedSudoku.set(80,1);

  chainedSudoku.set(81,10);
  chainedSudoku.set(83,11);
  chainedSudoku.set(84,2);
  chainedSudoku.set(86,12);
  chainedSudoku.set(88,3);
  chainedSudoku.set(91,7);
  chainedSudoku.set(93,5);
  chainedSudoku.set(95,10);
  chainedSudoku.set(96,13);
  chainedSudoku.set(97,14);
  chainedSudoku.set(98,4);
  chainedSudoku.set(99,1);
  chainedSudoku.set(101,6);
  chainedSudoku.set(103,9);
  chainedSudoku.set(104,15);
  chainedSudoku.set(105,8);

  chainedSudoku.createSolution();
  chainedSudoku.print();

  chainedSudoku.setFixed();
  chainedSudoku.print();

  chainedSudoku.print(2, "sudoku.svg", 0);

  chainedSudoku.eraseNonFixed();
  chainedSudoku.solveExisting();
  chainedSudoku.print();

  chainedSudoku.print(2, "sudoku_solution.svg", 3);


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
