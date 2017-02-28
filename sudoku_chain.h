#pragma once

#include <memory>

#include "chained_sudoku.h"

class SudokuChain
{
public:
  ///! main function, create a chained sudoku with solution and solution hints
  void generate();

private:

  ///! the specification of a single sudoku in the chain
  struct SudokuSpecification
  {
    int sudoku_no;      ///< number of sudoku
    int size;           ///< size (smallest unit, e.g. 3)
    int squared_size;   ///< size^2
    int offset_from_previous_x;   ///< relative position of anchor to previous anchor (x direction)
    int offset_from_previous_y;   ///< relative position of anchor to previous anchor (y direction)
    int firstOwnIndex;               ///< index in global fields of first field that does not belong to previous sudoku
    int extra;

    Sudoku::SudokuType type;   ///< the type of the sudoku

    int xa, ya, xb, yb;       ///< coordinates of overlapping area to previous sudoku

    std::shared_ptr<SudokuSpecification> previous;    ///< pointer to the previous sudoku specification in the chain, nullptr for the first
    std::shared_ptr<SudokuSpecification> next;    ///< pointer to the next sudoku specification in the chain, nullptr for the last

    ///! create a sudoku specification, sets squared_size
    SudokuSpecification(Sudoku::SudokuType _type, int _size, int _offset_x, int _offset_y, int _extra=0);
  };

  ///! solution finding procedure
  void createSolution();

  ///! fixing fields procedure
  void fixSomeFieldsToGetASolvableSudoku();

  ///! creation of solution hints procedure
  void collectSolutionHints(std::string filename);

  ///! print the specifications of the sudoku chain to console
  void outputSpecification(std::vector<std::shared_ptr<SudokuChain::SudokuSpecification>> specifications);

  ///! print sudoku chain, to print an svg file with all sudokus in it, set target = ChainedSudoku::PrintTarget::svg_file_body
  void print(ChainedSudoku::PrintTarget target = ChainedSudoku::PrintTarget::cout,
    std::string filename = "", ChainedSudoku::PrintMode mode = ChainedSudoku::PrintMode::all_fixed);

  ///! compute coordinates in current sudoku (xa,xb), (ya,yb) of rectangle of with previous sudoku overlapping area
  void computeOverlappingArea(int &xa, int &ya, int &xb, int &yb, std::shared_ptr<SudokuSpecification> specification);

  ///! compute the index of vector fields_ for coordinate (x,y) in sudoku specification
  int getFieldIndex(int x, int y, std::shared_ptr<SudokuSpecification> specification);


  ///! check if there are fields that can have only one possible value, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool fieldCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays_, ChainedSudoku::FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! check if there are values in a row that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool rowCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! check if there are values in a column that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool columnCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! check if there are values in a size_ x size_ box that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  bool boxCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  std::vector<std::shared_ptr<SudokuSpecification>> sudokuSpecifications_;   ///< the specifications for the sudokus
  std::vector<std::shared_ptr<ChainedSudoku>> sudoku_;    ///< the sudokus the sudoku chain consists of
  std::vector<std::shared_ptr<Field>> fields_;      ///< all the fields that occur in sudokus

  int id_counter_ = 0;
  static int sudoku_no_counter_;
  int nFieldsX_;    ///< size of bounding box (x)
  int nFieldsY_;    ///< size of bounding box (y)
  int coordinateYtop_;    ///< lowest coordinate in y direction where 0 is at anchor of first sudoku
  int coordinateYbottom_; ///< highest coordinate in y direction where 0 is at anchor of first sudoku

  std::vector<std::shared_ptr<SudokuSpecification>> currentSpecifications_;
};
