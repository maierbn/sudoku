#pragma once

#include "sudoku.h"

#include <functional>

class SudokuChain;

/**
*  A ChainedSudoku is a wrapper for a single Sudoku, that is part of a SudokuChain
*/
class ChainedSudoku : public Sudoku
{
public:

  //ChainedSudoku();
 ChainedSudoku(std::vector<std::shared_ptr<Field>> fields, int size, SudokuType type, SudokuChain *sudokuChain);

  ///! Write the sudoku to cout, a text file or a svg file.
  void print(PrintTarget target = PrintTarget::cout, std::string filename = "", PrintMode mode = PrintMode::all_fixed, int offset_x=0, int offset_y=0, int z=0);

  friend class SudokuChain;
private:

  ///! check if there are fields that can have only one possible value, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool fieldCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug) override;

  ///! check if there are values in a row that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool rowCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug) override;

  ///! check if there are values in a column that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool columnCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug) override;

  ///! check if there are values in a size_ x size_ box that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool boxCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug) override;

  bool (SudokuChain::*externRowCheck_)(std::list<FurtherWays> &furtherWays, FieldVector &testFields, bool recordFullFurtherWays, bool debug) = NULL;
  bool (SudokuChain::*externColumnCheck_)(std::list<FurtherWays> &furtherWays, FieldVector &testFields, bool recordFullFurtherWays, bool debug) = NULL;
  bool (SudokuChain::*externBoxCheck_)(std::list<FurtherWays> &furtherWays, FieldVector &testFields, bool recordFullFurtherWays, bool debug) = NULL;
  bool (SudokuChain::*externFieldCheck_)(std::list<FurtherWays> &furtherWays, FieldVector &testFields, bool recordFullFurtherWays, bool debug) = NULL;
  SudokuChain *sudokuChain_;
};
