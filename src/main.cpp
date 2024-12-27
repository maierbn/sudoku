#include <iostream>
#include <sstream>

#include "sudoku.h"
#include "sudoku_chain.h"
#include "train_sudoku.h"

#include "interleaved_sudoku.h"
#include "chained_sudoku.h"

#include <ctime>

int main(int argc, char **argv)
{
  srand(std::time(NULL));

  TrainSudoku trainSudoku(3);

  trainSudoku.createSolution();
  trainSudoku.print();
  trainSudoku.print(Sudoku::PrintTarget::svg_file, "output/sudoku_solution.svg", Sudoku::PrintMode::solved);

  for (int i = 0; i < 10; i++)
  {
    trainSudoku.resetFixed();
    
    std::stringstream filename_a;
    filename_a << "output/sudoku_" << i << ".svg";
    
    std::stringstream filename_b;
    filename_b << "output/solution_" << i << ".txt";
    
    trainSudoku.collectSolutionHints(filename_b.str());
    trainSudoku.print(Sudoku::PrintTarget::svg_file, filename_a.str(), Sudoku::PrintMode::all_fixed);
  }

  exit(0);
  // create sudoku
#if 1
  Sudoku sudoku(4, Sudoku::SudokuType::classical);

  sudoku.createSolution();
  sudoku.print();

  //sudoku.eraseNonFixed();
  sudoku.collectSolutionHints("a.txt");

  sudoku.print(Sudoku::PrintTarget::text_file);
  sudoku.permuteNumbers();
  sudoku.print();

  exit(0);

  sudoku.createSolution();
  sudoku.print(Sudoku::PrintTarget::svg_file, "E_solution.svg", Sudoku::PrintMode::solved);

  //sudoku.permuteNumbers();
  sudoku.setFixed(0, 5, false);
  sudoku.setFixed(1, 5, false);
  sudoku.setFixed(2, 5, false);
  sudoku.setFixed(0, 6, false);
  sudoku.setFixed(1, 6, false);
  sudoku.setFixed(2, 6, false);
  sudoku.setFixed(0, 7, false);
  sudoku.setFixed(1, 7, false);
  sudoku.setFixed(2, 7, false);
  sudoku.print(Sudoku::PrintTarget::svg_file, "EE.svg", Sudoku::PrintMode::all_fixed);

  sudoku.eraseNonFixed();
  sudoku.solveExisting();
  sudoku.print(Sudoku::PrintTarget::svg_file, "E.svg", Sudoku::PrintMode::all_fixed);

  exit(0);
#endif

#if 0
  ChainedSudoku chainedSudoku;

  chainedSudoku.print(Sudoku::PrintTarget::cout, "", Sudoku::PrintMode::max_value);
  chainedSudoku.print(Sudoku::PrintTarget::cout, "", Sudoku::PrintMode::index);

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

  chainedSudoku.print(Sudoku::PrintTarget::svg_file, "sudoku.svg", Sudoku::PrintMode::all_fixed);

  chainedSudoku.eraseNonFixed();
  chainedSudoku.solveExisting();
  chainedSudoku.print();

  chainedSudoku.print(Sudoku::PrintTarget::svg_file, "sudoku_solution.svg", Sudoku::PrintMode::solved);

#endif
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
  /*for(;;)
  {

  InterleavedSudoku isudoku(4);
  isudoku.createSolution();
  //isudoku.print();
  isudoku.fixSomeFieldsToGetASolvableSudoku();
  isudoku.eraseNonFixed();
  isudoku.solveExisting();
  isudoku.print(Sudoku::PrintTarget::text_file);
  //isudoku.permuteNumbers();
  //isudoku.print();

  }*/

  return 0;
}
