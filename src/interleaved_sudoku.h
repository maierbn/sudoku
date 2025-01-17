#pragma once

#include "sudoku.h"

class InterleavedSudoku : public Sudoku
{
public:
  InterleavedSudoku(int size);

  ///! Write the sudoku to cout, a text file or a svg file.
  void print(PrintTarget target = PrintTarget::cout, std::string filename = "");

  void permuteNumbers();
  void createSolution();
  void solveExisting();
  void eraseNonFixed();
  void fixSomeFieldsToGetASolvableSudoku();
  void set(int posX, int posY, int posZ, int value);

private:

  void addDependencies();
  int index(int x, int y);
  int index(int x, int y, int z);

  ///! print all fields and numbers to the target (cout, txt or svg)
  void printFields(std::vector< std::shared_ptr< Field > >& fields, PrintTarget target = PrintTarget::cout,
    std::string filename = "");

  bool rowCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  bool columnCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  bool boxCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  void chooseOpenFieldInTestFieldsAndFixToSolution(std::vector<std::shared_ptr<Field>> &testFields, int maxPossible, bool fixFieldChooseRandomly, bool debug);
};
