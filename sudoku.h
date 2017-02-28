#pragma once

#include <vector>
#include <memory>
#include <list>

#include "field.h"

class Sudoku
{
public:
  enum class PrintTarget
  {
    cout = 0,
    text_file = 1,
    svg_file = 2,
    svg_file_body = 3     // for Sudoku: without svg header and footer, for SudokuChain: all in one file
  };

  enum class PrintMode
  {
    all_fixed = 0,    // if printTarget = svg_file, only fixed
    max_value = 1,
    index = 2,
    solved = 3,
    id = 4
  };

  enum class SudokuType
  {
    classical,            // the standard sudoku rules
    diagonal,        // standard sudoku with addional diagonal dependencies
    left_side_inverted,   // mirror fields in first blocks on the left
    freeform,             // sudoku with differently shaped blocks
    sum,                   // sum sudoku
  };

  static const std::string types[5];

  typedef std::vector<std::shared_ptr<Field>> FieldVector;
  typedef FieldVector::iterator FieldIterator;

  ///! setup a sudoku, size is the length of an inner square (e.g. 3 for a standard 9x9 sudoku)
  Sudoku(int size, SudokuType type);

  ///! Write the sudoku to cout, a text file or a svg file.
  virtual void print(PrintTarget target = PrintTarget::cout, std::string filename = "", PrintMode mode = PrintMode::all_fixed,
    int x_offset=0, int y_offset=0);

  ///! Create a solution of the sudoku, that matches all dependencies and fixed values, number is only for debugging output
  virtual void createSolution(int number=0);

  ///! Automatically set some fields as fixed, \pre Sudoku is solved (by createSolution) \param testFields can be empty
  virtual void fixSomeFieldsToGetASolvableSudoku(bool createSolutionHints, FieldVector &testFields, bool quiet=true,
    std::string filename="");

  ///! create solution hints
  virtual void collectSolutionHints(std::string filename);

  ///! randomly permute the numbers
  virtual void permuteNumbers();


  ///! Reset triedValues for all non fixed fields. Then the sudoku is like unsolved.
  virtual void eraseNonFixed();

  ///! fill in the solution in the existing sudoku (deprecated)
  virtual void solveExisting();

  ///! count number of fixed values in the current sudoku
  int nFixed();

  ///! set field (pos_x, pos_y) fixed
  void setFixed(int pos_x, int pos_y, bool fixed);

  ///! set field (pos_x, pos_y) to value and optionally fixed
  void set(int pos_x, int pos_y, int value, bool fixed=false);

  struct FurtherWays
  {
    int nRowCheckMatches = 0;       ///< number of occurences that a value can only be at one position in a row
    int nColumnCheckMatches = 0;    ///< number of occurences that a value can only be at one position in a column
    int nBoxCheckMatches = 0;       ///< number of occurences that a value can only be at one position in a size_ x size_ box
    int nFieldCheckMatches = 0;     ///< number of occurences that in a field only one value is possible because of dependencies
    int nDiagonalCheckMatches = 0;  ///< number of occurences that a value can only be at one position in a diagonal

    enum Reason
    {
      none = -1,
      onlyPossibilityInField = 0,
      onlyPositionInRow = 1,
      onlyPositionInColumn = 2,
      onlyPositionInBox = 3,
      onlyPositionInDiagonal = 4
    };

    /// one possible field to set next
    struct Way
    {
      int index;
      int posX;
      int posY;
      int posZ;
      int value;
      Reason reason;

      bool operator<(const Way &rhs) const;
    };
    std::set<Way> ways;		// all possible things to do at this stage

    void addWay(int index, int posX, int posY, int posZ, int value, Reason reason = Reason::none);  //reason: 0: field, 1: row, 2:column, 3:box
    int nMatches();
  };

protected:
  ///! map from (x,y) to contiguous index, row major
  virtual int index(int x, int y);

  ///! add classical dependencies (row, column, box) in variable fields_
  virtual void addDependencies();

  ///! add classical dependencies (row, column, box)
  virtual void addDependenciesFields(FieldVector &fields);

  ///! print all fields and numbers to the target (cout, txt or svg)
  virtual void printFields(std::vector< std::shared_ptr< Field > >& fields, PrintTarget target = PrintTarget::cout,
    std::string filename = "", PrintMode mode = PrintMode::all_fixed, int x_offset = 0, int y_offset = 0);

  ///! check if there are fields that can have only one possible value, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  virtual bool fieldCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! check if there are values in a row that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  virtual bool rowCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! check if there are values in a column that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  virtual bool columnCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! check if there are values in a size_ x size_ box that can only be at one position, set these values in testFields
  ///! \return if one such value was found and !recordFullFurtherWays, return true
  virtual bool boxCheck(std::list<FurtherWays> &furtherWays, FieldVector &testFields,
    bool recordFullFurtherWays, bool debug);

  ///! choose a not fixed field, either one with chooseFieldThatHasNPossibilities possibilities (in case !fixFieldChooseRandomly)
  ///! or choose randomly (in case fixFieldChooseRandomly)
  ///! and set this field to the correct value of the solution at that index
  virtual void chooseOpenFieldInTestFieldsAndFixToSolution(FieldVector &testFields,
    int chooseFieldThatHasNPossibilities, bool fixFieldChooseRandomly, bool debug);

  ///! increment the iterator nextField until it reaches the end of fields_ or until it points to a not fixed field
  void incrementFieldIteratorToNextUnfixed(FieldIterator &nextField);

  ///! decrement the iterator nextField until it reaches the begin of fields_ or until it points to a not fixed field
  void decrementFieldIteratorToNextUnfixed(FieldIterator &nextField);

  ///! create copy of fields with dependencies that point to the copy
  void createFieldCopy(FieldVector &copiedFields);

  FieldVector fields_;     ///< the fields that this sudoku consists of. The sudoku can also use an external vector, \
   when called with the respective constructor. Then some functions such as index(x,y) are not available. \
   createSolution() works.
  FieldIterator field_begin_;   ///< iterator to the begin of the used fields. This is usually equal to fields_.begin().
  FieldIterator field_end_;     ///< iterator to the end of the used fields. This is usually equal to fields_.end().
  std::list<FurtherWays> furtherWays_;    ///< a list for each solving step which actions can be taken

  SudokuType type_;   ///< the type of the sudoku
  int size_;  ///< the basic size of the sudoku, e.g. 3
  int squared_size_; ///< = size_*size_, also the maximum value that can be filled in in a field, e.g. 9
  int current_z_;     ///< This is an index of the sudoku for systems of multiple sudokus (as inherited class). It does not do anything here
  std::string filename_;
  std::string solutionHints_;   ///< a string containing solution hints

  std::vector<long long> nPossibilities_;   ///< number of possibilities for each step
  long nRuns_;        ///< number of summands for each index in nPossiblities
};
