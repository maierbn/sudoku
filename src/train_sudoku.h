#pragma once

#include "sudoku.h"

class TrainSudoku : public Sudoku
{
public:
  TrainSudoku(int size);
  
protected:
  void addDependencies();

  //! add group dependency for a "train" at locations a and b
  void addTrainDependencies(const std::set<std::pair<int, int>>& a, const std::set<std::pair<int, int>>& b);
};
