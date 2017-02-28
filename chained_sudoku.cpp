#include "chained_sudoku.h"

#include "sudoku.h"

#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <random>
#include <assert.h>

ChainedSudoku::ChainedSudoku(std::vector<std::shared_ptr<Field>> fields, int size, SudokuType type, SudokuChain *sudokuChain) :
  Sudoku(0, type), sudokuChain_(sudokuChain)
{
  // copy the given fields (vector of pointers) to own fields_
  // these contain all the fields in standard order (whether they are shared with different ChainedSudokus
  // is transparent here
  fields_.assign(fields.begin(), fields.end());

  // initialize size_ and squared_size_
  size_ = size;
  squared_size_ = size_*size_;

  // now the number of fields should be squared_size_^2
  assert(fields.size() == squared_size_*squared_size_);

  field_begin_ = fields_.begin();
  field_end_ = fields_.end();

  // add the classical dependencies
  Sudoku::addDependencies();
}

bool ChainedSudoku::fieldCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  if(externFieldCheck_ != NULL)
  {
    return (sudokuChain_->*externFieldCheck_)(furtherWays, testFields, recordFullFurtherWays, debug);
  }
  else return Sudoku::fieldCheck(furtherWays, testFields, recordFullFurtherWays, debug);
}

bool ChainedSudoku::rowCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  if(externRowCheck_ != NULL)
  {
    return (sudokuChain_->*externRowCheck_)(furtherWays, testFields, recordFullFurtherWays, debug);
  }
  else return Sudoku::rowCheck(furtherWays, testFields, recordFullFurtherWays, debug);
}

bool ChainedSudoku::columnCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  if(externColumnCheck_ != NULL)
  {
    return (sudokuChain_->*externColumnCheck_)(furtherWays, testFields, recordFullFurtherWays, debug);
  }
  else return Sudoku::columnCheck(furtherWays, testFields, recordFullFurtherWays, debug);
}

bool ChainedSudoku::boxCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  if(externBoxCheck_ != NULL)
  {
    return (sudokuChain_->*externBoxCheck_)(furtherWays, testFields, recordFullFurtherWays, debug);
  }
  else return Sudoku::boxCheck(furtherWays, testFields, recordFullFurtherWays, debug);
}


void ChainedSudoku::print(PrintTarget target, std::string filename, PrintMode mode,  int offset_x, int offset_y, int z)
{
  // compose specific filename for that sudoku
  std::stringstream specific_filename;

  if(target == PrintTarget::svg_file_body)
  {
    specific_filename << filename;
  }
  else
  {
    // add suffix to make filename depend on z
    std::string suffix = "";
    if(target == PrintTarget::svg_file)
    {
      suffix = ".svg";
    }
    else if(target == PrintTarget::text_file)
    {
      suffix = ".txt";
    }
    if(filename.find(".") != std::string::npos)
    {
      specific_filename << filename.substr(0, filename.find(".")) << "_" << std::setfill('0') << std::setw(2) << (z+1) << suffix;
    }
    else
    {
      specific_filename << filename << "_" << std::setfill('0') << std::setw(2) << (z+1) << suffix;
    }
  }

  current_z_ = z;

  // print that sudoku
  Sudoku::print(target, specific_filename.str(), mode, offset_x, offset_y);
}
