#pragma once

#include "sudoku.h"

class InterleavedSudoku : public Sudoku 
{
public:
  InterleavedSudoku(int size);
  
  void print(int target=0, std::string filename="");
  void permuteNumbers();
  void createSolution();
  void solveExisting();
  void eraseNonFixed();
  void setFixed();
  void set(int posX, int posY, int posZ, int value);
  
private:
  
  void addDependencies();
  int index(int x, int y);
  int index(int x, int y, int z);
  void printFields(std::vector< std::shared_ptr< Field > >& fields, int target=0, std::string filename="");
  
  bool rowCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  bool columnCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  bool boxCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  void fixField(std::vector<std::shared_ptr<Field>> &testFields, int maxPossible, bool fixFieldChooseRandomly, bool debug);
};