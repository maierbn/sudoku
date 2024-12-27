#include "train_sudoku.h"

#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <chrono>


TrainSudoku::TrainSudoku(int size) : Sudoku(size, Sudoku::SudokuType::classical)
{
  addDependencies();
}


void TrainSudoku::addDependencies()
{
  Sudoku::addDependencies();
  
  std::cout<<"Add group dependencies"<<std::endl;

  // add group dependencies
  // Each row is one train (x,y) coordinates. The trains consist of the same numbers.
  
#if 0
  // 2
  addTrainDependencies(
    {{0, 0}, {1, 0}, {2, 0}},
    {{2, 4}, {3, 4}, {4, 4}}
  );

  // 3
  addTrainDependencies(
    {{3, 0}, {4, 0}, {5, 0}},
    {{5, 4}, {6, 4}, {7, 4}}
  );

  // 4
  addTrainDependencies(
    {{5, 2}, {6, 2}, {7, 2}, {8, 2}},
    {{1, 8}, {2, 8}, {3, 8}, {4, 8}}
  );


  // 5
  addTrainDependencies(
    {{0, 3}, {1, 3}, {2, 3}, {3, 3}},
    {{1, 6}, {2, 6}, {3, 6}, {4, 6}}
  );

#else
  // 2
  addTrainDependencies(
    {{3, 1}, {4, 1}, {5, 1}},
    {{0, 3}, {1, 3}, {2, 3}}
  );

  // 3
  addTrainDependencies(
    {{6, 1}, {7, 1}, {8, 1}},
    {{3, 3}, {4, 3}, {5, 3}}
  );

  // 4
  addTrainDependencies(
    {{5, 5}, {6, 5}, {7, 5}, {8, 5}},
    {{2, 7}, {3, 7}, {4, 7}, {5, 7}}
  );


if (false)
{

  // 5
  addTrainDependencies(
    {{1, 6}, {2, 6}, {3, 6}},
    {{2, 8}, {3, 8}, {4, 8}}
  );
}

#endif

  std::cout<<"done"<<std::endl;
}


void TrainSudoku::addTrainDependencies(
  const std::set<std::pair<int, int>>& a,
  const std::set<std::pair<int, int>>& b)
{
  // Convert coordinate pairs to shared_ptr<Field> for the fields.
  auto toFieldSet = [this](const std::set<std::pair<int, int>>& coords) {
      std::set<std::shared_ptr<Field>, CompareFields> fieldSet;
      for (const auto& coord : coords) {
          fieldSet.insert(fields_[index(coord.first, coord.second)]);
      }
      return fieldSet;
  };

  // Create the field sets for a and b
  auto aFields = toFieldSet(a);
  auto bFields = toFieldSet(b);

  // Add dependencies from a to b
  for (const auto& fieldA : aFields) {
      fieldA->addGroupDependency(bFields);
  }

  // Add dependencies from b to a
  for (const auto& fieldB : bFields) {
      fieldB->addGroupDependency(aFields);
  }
}
