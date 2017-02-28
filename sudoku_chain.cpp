#include "sudoku_chain.h"

#include <iostream>
#include <cmath>
#include <assert.h>
#include <fstream>

const bool DEBUG = false;

const int number_offset = 69;

int SudokuChain::sudoku_no_counter_ = 0;
void SudokuChain::generate()
{
srand(10);
srand(1);

  // sudoku chain specification
  if(false)
  {
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::freeform, 3, 0, 0));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 8, -4));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 8, -4));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 6, 6));
  }

  if(false)
    for(int i=0; i<20; i++)
    {
      if(i == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 6, -6));
      }
      else if(i %10 == 3)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 8, 0));
      }
      else if(i % 10 == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 4, 8, 5));
      }
      else if(i % 10 == 1 && i>1)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 13, -8));
      }
      else if(i % 5 == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::freeform, 3, 6, -8));
      }
      else if(i % 2 == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::diagonal, 3, 6, 6));
      }
      else
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 6+rand()%2, -5-rand()%4));
      }
    }

  if(false)
  {
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 0, 0, 1));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::freeform, 3, 6, 6));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::freeform, 3, 5, 5));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 4, 7, -10));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 12, -8));
  }

  // 2
  if(false)
  {
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 7, 3, 0));

    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 4, 7, 5));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 15, 11));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 6, -6));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::left_side_inverted, 3, 6, 0));

    for(int i=0; i<30; i++)
    {
      if(i == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 8, -3));
      }
      else if(i %10 == 3)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 8, 0));
      }
      else if(i % 10 == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 4, 8, 5));
      }
      else if(i % 10 == 1 && i>1)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 13, -8));
      }
      else if(i % 5 == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::diagonal, 3, 6, -8));
      }
      else if(i % 10 == 6)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::diagonal, 3, 6, 6));
      }
      else if(i % 2 == 0)
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::freeform, 3, 6, 6));
      }
      else
      {
        sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 3, 6+rand()%2, -5-rand()%4));
      }
    }
  }

  // part 3
  if(false)
  {
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 2, 0, 0, 2));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::diagonal, 3, 2, 2));
    //sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::diagonal, 4, 6, 6));
    sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 4, 8, -3));
  }

  sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 5, 0, 0, 0));



  //sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 5, 15, -9));
  //sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 5, 15, -25+8));

  //sudokuSpecifications_.push_back(std::make_shared<SudokuSpecification>(Sudoku::SudokuType::classical, 5, 12, 12));

  // set previous and next specification pointers
  std::vector<std::shared_ptr<SudokuSpecification>>::iterator previous_iter = sudokuSpecifications_.end();
  for(auto iter = sudokuSpecifications_.begin(); iter != sudokuSpecifications_.end(); iter++)
  {
    if(previous_iter != sudokuSpecifications_.end())
    {
      (*iter)->previous = *previous_iter;
      (*previous_iter)->next = *iter;
    }
    previous_iter = iter;
  }

  // create chained sudokus and compute bounding box size
  coordinateYbottom_ = 0;
  coordinateYtop_ = 0;
  nFieldsX_ = 0;
  int nFieldsX = 0;
  int nFieldsY = 0;
  // loop over sudoku specifications
  for(auto &specification : sudokuSpecifications_)
  {
    if(DEBUG)
      std::cout<<"create sudoku"<<std::endl;
    std::shared_ptr<SudokuSpecification> previousSpecification = specification->previous;

    // compute overlapping area to previous sudoku
    int xa, ya, xb, yb;
    computeOverlappingArea(xa, ya, xb, yb, specification);
    specification->xa = xa;
    specification->ya = ya;
    specification->xb = xb;
    specification->yb = yb;

    int overlapping_to_previous = (xb-xa) * (yb-ya);

    specification->firstOwnIndex = id_counter_;

    // update bounding box size
    nFieldsX += specification->offset_from_previous_x;
    nFieldsY += specification->offset_from_previous_y;
    nFieldsX_ = std::max(abs(nFieldsX + specification->squared_size), nFieldsX_);
    coordinateYtop_ = std::max(nFieldsY + specification->squared_size, coordinateYtop_);
    coordinateYbottom_ = std::min(nFieldsY, coordinateYbottom_);
    nFieldsY_ = coordinateYtop_ - coordinateYbottom_;

    if(DEBUG)
      std::cout<<"  number overlapping_to_previous: "<<overlapping_to_previous
        <<", firstOwnIndex: "<<specification->firstOwnIndex<<std::endl;

    // add new fields
    int nNewFields = specification->squared_size * specification->squared_size - overlapping_to_previous;
    int max_value = specification->squared_size;
    for(int i=0; i<nNewFields; i++)
    {
      fields_.push_back(std::make_shared<Field>(id_counter_++, max_value));
    }
    if(DEBUG)
      std::cout<<"  "<<nNewFields<<" new fields added"<<std::endl;

    // collect pointers to a new full sudoku
    std::vector<std::shared_ptr<Field>> currentSudokuFields;
    for(int y = 0; y < specification->squared_size; y++)
    {
      for(int _x = 0; _x < specification->squared_size; _x++)
      {
        int x = _x;
        // mirror fields in first blocks on the left
        if(specification->type == Sudoku::SudokuType::left_side_inverted)
        {
          if(_x < specification->size)
          {
            x = specification->size - 1 - _x;
            std::cout<<_x<<" -> "<<x<<std::endl;
          }
        }

        int index = getFieldIndex(x, y, specification);

        if(DEBUG)
          std::cout<<"    ("<<x<<","<<y<<") -> "<<index<<std::endl;

        std::shared_ptr<Field> currentField = fields_[index];

        if(currentField->max_value() > max_value && index < specification->firstOwnIndex)
        {
          if(DEBUG)
            std::cout<<"adjust max value from "<<currentField->max_value()<<" to "<<max_value<<std::endl;
          currentField->setMaxValue(max_value);
        }

        currentSudokuFields.push_back(currentField);
      }
    }

    if(specification->extra==1)
    {
      fields_[getFieldIndex(0, 0, specification)]->setValue(5);
      fields_[getFieldIndex(0, 1, specification)]->setValue(3);
      fields_[getFieldIndex(0, 2, specification)]->setValue(4);
      fields_[getFieldIndex(0, 3, specification)]->setValue(7);
      fields_[getFieldIndex(0, 0, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 1, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 2, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 3, specification)]->setFixed(true);
    }
    else if(specification->extra==2)
    {
      fields_[getFieldIndex(0, 0, specification)]->setValue(4);
      fields_[getFieldIndex(0, 0, specification)]->setFixed(true);
    }
    else if(specification->extra==3)
    {
      fields_[getFieldIndex(0, 9, specification)]->setValue(10);
      fields_[getFieldIndex(0, 10, specification)]->setValue(2);
      fields_[getFieldIndex(0, 11, specification)]->setValue(3);
      fields_[getFieldIndex(0, 12, specification)]->setValue(8);
      fields_[getFieldIndex(0, 13, specification)]->setValue(7);
      fields_[getFieldIndex(0, 14, specification)]->setValue(1);
      fields_[getFieldIndex(0, 15, specification)]->setValue(15);
      fields_[getFieldIndex(0, 16, specification)]->setValue(16);
      fields_[getFieldIndex(0, 17, specification)]->setValue(6);
      fields_[getFieldIndex(0, 18, specification)]->setValue(12);
      fields_[getFieldIndex(0, 19, specification)]->setValue(9);
      fields_[getFieldIndex(0, 20, specification)]->setValue(14);
      fields_[getFieldIndex(0, 21, specification)]->setValue(4);
      fields_[getFieldIndex(0, 22, specification)]->setValue(11);
      fields_[getFieldIndex(0, 23, specification)]->setValue(5);
      fields_[getFieldIndex(0, 24, specification)]->setValue(13);

      fields_[getFieldIndex(0, 9, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 10, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 11, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 12, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 13, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 14, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 15, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 16, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 17, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 18, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 19, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 20, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 21, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 22, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 23, specification)]->setFixed(true);
      fields_[getFieldIndex(0, 24, specification)]->setFixed(true);
    }


    if(specification->extra==1 && false)
    {
        int index = getFieldIndex(7, 6, specification);
        fields_[index]->addDependency(fields_[getFieldIndex(7, 3, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(7, 4, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(7, 5, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(8, 3, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(8, 4, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(8, 5, specification)]);

        index = getFieldIndex(8, 6, specification);
        fields_[index]->addDependency(fields_[getFieldIndex(7, 3, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(7, 4, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(7, 5, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(8, 3, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(8, 4, specification)]);
        fields_[index]->addDependency(fields_[getFieldIndex(8, 5, specification)]);
        std::cout<<"add dependencies"<<std::endl;
    }

    if(DEBUG)
      std::cout<<"  new collection currentSudokuFields contains "<<currentSudokuFields.size()<<" pointers"<<std::endl;

    // create sudoku with new pointers
    sudoku_.push_back(std::make_shared<ChainedSudoku>(currentSudokuFields, specification->size, specification->type, this));
  }

  outputSpecification(sudokuSpecifications_);

  // check if index of last field matches number of fields
  assert(fields_.back()->id() == fields_.size()-1);

  std::cout<<"bounding box: "<<nFieldsX_<<" x "<<nFieldsY_<<std::endl;

  print(ChainedSudoku::PrintTarget::svg_file_body, "output/frames.svg", ChainedSudoku::PrintMode::all_fixed);
  if(DEBUG)
  {
    std::cout<<"indices: "<<std::endl;
    print(ChainedSudoku::PrintTarget::cout, "", ChainedSudoku::PrintMode::id);
  }

  createSolution();

  if(DEBUG)
  {
    std::cout<<"solution: "<<std::endl;
    print(ChainedSudoku::PrintTarget::cout, "", ChainedSudoku::PrintMode::all_fixed);
  }

  std::cout<<"Fix fields"<<std::endl;

  fixSomeFieldsToGetASolvableSudoku();

  if(DEBUG)
  {
    std::cout<<"fixed values: "<<std::endl;
    print(ChainedSudoku::PrintTarget::cout, "", ChainedSudoku::PrintMode::all_fixed);
  }

  sudoku_[0]->permuteNumbers();
  collectSolutionHints("output/solution.txt");


  print(ChainedSudoku::PrintTarget::svg_file, "output/chained_sudoku.svg", ChainedSudoku::PrintMode::solved);
  print(ChainedSudoku::PrintTarget::svg_file_body, "output/solution.svg", ChainedSudoku::PrintMode::solved);
  print(ChainedSudoku::PrintTarget::svg_file_body, "output/exercise.svg", ChainedSudoku::PrintMode::all_fixed);
}

int SudokuChain::getFieldIndex(int x, int y, std::shared_ptr<SudokuSpecification> specification)
{
  bool debug = false;

  if(specification->previous == nullptr)
  {
    if(debug)
      std::cout<<"    (no previous)"<<std::endl;
    return specification->firstOwnIndex + y*specification->squared_size + x;
  }

  // compute coordinates of with previous overlapping area in current sudoku
  int &xa = specification->xa;
  int &ya = specification->ya;
  int &xb = specification->xb;
  int &yb = specification->yb;

  // check if position is inside with previous overlapping area in current sudoku
  if(x >= xa && x < xb && y >= ya && y < yb)
  {
    if(debug)
      std::cout<<"    (ask previous at (x,y)=("<<x+specification->offset_from_previous_x<<", "<<y+specification->offset_from_previous_y<<")"<<std::endl;
    return getFieldIndex(x+specification->offset_from_previous_x, y+specification->offset_from_previous_y, specification->previous);
  }
  else
  {
    int index = specification->firstOwnIndex;

    // number of elements in rows before current row (y)
    for(int y_index = 0; y_index < y; y_index++)
    {
      if(ya <= y_index && y_index < yb)
      {
        index += specification->squared_size - (xb-xa);
      }
      else
      {
        index += specification->squared_size;
      }
    }
    if(ya <= y && y < yb)
    {
      if(xb <= x)
      {
        index += x-xb;
      }
      else
      {
        index += x;
      }
    }
    else
    {
      index += x;
    }
    return index;
  }
}

void SudokuChain::computeOverlappingArea(int &xa, int &ya, int &xb, int &yb, std::shared_ptr<SudokuSpecification> specification)
{
  bool debug = false;

  // compute coordinates of with previous overlapping area in current sudoku
  std::shared_ptr<SudokuSpecification> previousSpecification = specification->previous;
  if(!previousSpecification)
  {
    xa = ya = xb = yb = 0;
    return;
  }

  if(specification->offset_from_previous_x >= 0)
  {
    xa = 0;
    xb = std::min(previousSpecification->squared_size - specification->offset_from_previous_x, specification->squared_size);
  }
  else
  {
    xa = -specification->offset_from_previous_x;
    xb = std::min(specification->squared_size, xa+previousSpecification->squared_size);
  }

  if(specification->offset_from_previous_y >= 0)
  {
    ya = 0;
    yb = std::min(previousSpecification->squared_size - specification->offset_from_previous_y, specification->squared_size);
  }
  else
  {
    ya = -specification->offset_from_previous_y;
    yb = std::min(specification->squared_size, ya+previousSpecification->squared_size);
  }
  if(debug)
    std::cout<<"  overlapping area: ("<<xa<<","<<ya<<")-("<<xb<<","<<yb<<")"<<std::endl;
}

SudokuChain::SudokuSpecification::SudokuSpecification(Sudoku::SudokuType _type, int _size, int _offset_from_previous_x, int _offset_from_previous_y, int _extra) :
  sudoku_no(sudoku_no_counter_++), size(_size), squared_size(size*size),
  offset_from_previous_x(_offset_from_previous_x), offset_from_previous_y(_offset_from_previous_y), type(_type), extra(_extra)
{
}

bool SudokuChain::fieldCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  // check if there are fields that can have only one possible value
  // loop over sudoku specifications
  for(auto &specification : sudokuSpecifications_)
  {
    // loop over rows
    for (int y = 0; y < specification->squared_size; y++)
    {
      // loop over columns
      for (int x = 0; x < specification->squared_size; x++)
      {
        int index = getFieldIndex(x,y,specification);
        std::shared_ptr<Field> &testField = testFields[index];
        if(testField->value() == 0)   // if value is not yet set
        {
          if(testField->n_possible() == 1)    // if there is exactly one possiblility
          {
            if(debug)
              std::cout<<"  Field check: field ("<<index<<") must be "<<testField->onePossibleValue()<<""<<std::endl;

            furtherWays.back().nFieldCheckMatches++;
            furtherWays.back().addWay(index, x, y, specification->sudoku_no,
              testField->onePossibleValue(), ChainedSudoku::FurtherWays::Reason::onlyPossibilityInField);

            if(!recordFullFurtherWays)
            {
              testField->setValue(testField->onePossibleValue());
              restart = true;
              break;
            }
          }
        }
        if(restart)
          break;
      }
      if(restart)
        break;
    }
    if(restart)
      break;
  }
  return restart;
}

bool SudokuChain::rowCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{

  bool restart = false;

  //check rows, if there are values that can only be at one position
  // loop over sudoku specifications
  for(auto &specification : sudokuSpecifications_)
  {
    // loop over rows
    for (int y = 0; y < specification->squared_size; y++)
    {
      // loop over all values
      for (int value = 1; value <= specification->squared_size; value++)
      {
        // check if value is already set (valueIsAlreadySet),
        // or can only be at exactly one position in this row (nPossiblePositionsForValue==1), then it is position positionX
        bool valueIsAlreadySet = false;
        int nPossiblePositionsForValue = 0;
        int positionX = 0;

        // loop over fields in row
        for (int x = 0; x < specification->squared_size; x++)
        {
          int index = getFieldIndex(x,y,specification);
          if(testFields[index]->value() == value)
          {
            valueIsAlreadySet = true;
            break;
          }

          if(testFields[index]->valuePossible(value))
          {
            nPossiblePositionsForValue++;
            positionX = x;
          }
          if(nPossiblePositionsForValue > 1)
            break;
        }

        //if Value can only be at one position, set it there
        if(nPossiblePositionsForValue == 1 && !valueIsAlreadySet)
        {
          if(debug)
            std::cout<<"  Row check (row "<<y<<"): value "<<value<<" has to be in field ("<<positionX<<","<<y<<","<<specification->sudoku_no<<")"<<std::endl;

          furtherWays.back().nRowCheckMatches++;    // increment number of occurences that a value can only be at one position in a row
          int index = getFieldIndex(positionX, y, specification);
          furtherWays.back().addWay(index, positionX, y, specification->sudoku_no, value, ChainedSudoku::FurtherWays::Reason::onlyPositionInRow);

          if(!recordFullFurtherWays)
          {
            int index = getFieldIndex(positionX, y, specification);
            testFields[index]->setValue(value);
            restart = true;
            break;
          }
        }
      }
      if(restart)
        break;
    }
    if(restart)
      break;

    // check diagonals
    if(specification->type == Sudoku::SudokuType::diagonal)
    {
      // diagonal lower left to upper right
      // loop over all values
      for (int value = 1; value <= specification->squared_size; value++)
      {
        // check if value is already set (valueIsAlreadySet),
        // or can only be at exactly one position in this diagonal (nPossiblePositionsForValue==1), then it is position positionX
        bool valueIsAlreadySet = false;
        int nPossiblePositionsForValue = 0;
        int positionX = 0;
        int positionY = 0;

        // loop over fields in diagonal
        for (int i = 0; i < specification->squared_size; i++)
        {
          int index = getFieldIndex(i,i,specification);
          if(testFields[index]->value() == value)
          {
            valueIsAlreadySet = true;
            break;
          }

          if(testFields[index]->valuePossible(value))
          {
            nPossiblePositionsForValue++;
            positionX = i;
            positionY = i;
          }
          if(nPossiblePositionsForValue > 1)
            break;
        }

        //if Value can only be at one position, set it there
        if(nPossiblePositionsForValue == 1 && !valueIsAlreadySet)
        {
          if(debug)
            std::cout<<"  Diagonal check (row "<<positionX<<"): value "<<value<<" has to be in field ("<<positionX<<","<<positionY<<","<<specification->sudoku_no<<")"<<std::endl;

          furtherWays.back().nDiagonalCheckMatches++;    // increment number of occurences that a value can only be at one position in a row
          int index = getFieldIndex(positionX, positionY, specification);
          furtherWays.back().addWay(index, positionX, positionY, specification->sudoku_no, value, ChainedSudoku::FurtherWays::Reason::onlyPositionInDiagonal);

          if(!recordFullFurtherWays)
          {
            int index = getFieldIndex(positionX, positionY, specification);
            testFields[index]->setValue(value);
            restart = true;
            break;
          }
        }
      }
      if(restart)
        break;

      // diagonal upper left to lower right
      // loop over all values
      for (int value = 1; value <= specification->squared_size; value++)
      {
        // check if value is already set (valueIsAlreadySet),
        // or can only be at exactly one position in this diagonal (nPossiblePositionsForValue==1), then it is position positionX
        bool valueIsAlreadySet = false;
        int nPossiblePositionsForValue = 0;
        int positionX = 0;
        int positionY = 0;

        // loop over fields in diagonal
        for (int i = 0; i < specification->squared_size; i++)
        {
          int index = getFieldIndex(i,specification->squared_size-1-i,specification);
          if(testFields[index]->value() == value)
          {
            valueIsAlreadySet = true;
            break;
          }

          if(testFields[index]->valuePossible(value))
          {
            nPossiblePositionsForValue++;
            positionX = i;
            positionY = specification->squared_size-1-i;
          }
          if(nPossiblePositionsForValue > 1)
            break;
        }

        //if Value can only be at one position, set it there
        if(nPossiblePositionsForValue == 1 && !valueIsAlreadySet)
        {
          if(debug)
            std::cout<<"  Diagonal check (row "<<positionX<<"): value "<<value<<" has to be in field ("<<positionX<<","<<positionY<<","<<specification->sudoku_no<<")"<<std::endl;

          furtherWays.back().nDiagonalCheckMatches++;    // increment number of occurences that a value can only be at one position in a row
          int index = getFieldIndex(positionX, positionY, specification);
          furtherWays.back().addWay(index, positionX, positionY, specification->sudoku_no, value, ChainedSudoku::FurtherWays::Reason::onlyPositionInDiagonal);

          if(!recordFullFurtherWays)
          {
            int index = getFieldIndex(positionX, positionY, specification);
            testFields[index]->setValue(value);
            restart = true;
            break;
          }
        }
      }
      if(restart)
        break;
    }
  }
  return restart;
}

bool SudokuChain::columnCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  //check columns, if there are values that can only be at one position
  // loop over sudoku specifications
  for(auto &specification : sudokuSpecifications_)
  {
    // loop over columns
    for (int x = 0; x < specification->squared_size; x++)
    {
      // loop over all values
      for (int value = 1; value <= specification->squared_size; value++)
      {
        // check if value is already set (valueIsAlreadySet),
        // or can only be at exactly one position in this column (nPossiblePositionsForValue==1), then it is position positionY
        bool valueIsAlreadySet = false;
        int nPossiblePositionsForValue = 0;
        int positionY = 0;

        // loop over fields in column
        for (int y = 0; y < specification->squared_size; y++)
        {
          int index = getFieldIndex(x,y,specification);
          if(testFields[index]->value() == value)
          {
            valueIsAlreadySet = true;
            break;
          }

          if(testFields[index]->valuePossible(value))
          {
            nPossiblePositionsForValue++;
            positionY = y;
          }
          if(nPossiblePositionsForValue > 1)
            break;
        }

        //if Value can only be at one position, set it there
        if(nPossiblePositionsForValue == 1 && !valueIsAlreadySet)
        {
          if(debug)
            std::cout<<"Column check (column "<<x<<"): value "<<value<<" has to be in field ("<<x<<","<<positionY<<","<<specification->sudoku_no<<")"<<std::endl;

          furtherWays.back().nColumnCheckMatches++;
          int index = getFieldIndex(x,positionY,specification);
          furtherWays.back().addWay(index, x, positionY, specification->sudoku_no, value, ChainedSudoku::FurtherWays::Reason::onlyPositionInColumn);

          if(!recordFullFurtherWays)
          {
            int index = getFieldIndex(x,positionY,specification);
            testFields[index]->setValue(value);
            restart = true;
            break;
          }
        }
      }
      if(restart)
        break;
    }
    if(restart)
      break;
  }

  return restart;
}

bool SudokuChain::boxCheck(std::list<ChainedSudoku::FurtherWays> &furtherWays, ChainedSudoku::FieldVector &testFields,
  bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  // check 3x3 boxes, if there are values that can only be at one position
  // loop over sudoku specifications
  for(auto &specification : sudokuSpecifications_)
  {
    if(specification->type != Sudoku::SudokuType::freeform || specification->size != 3)
    {
      // loop over columns of boxes
      for (int box_no_x = 0; box_no_x < specification->size; box_no_x++)
      {
        // loop over rows of boxes in that column
        for (int box_no_y = 0; box_no_y < specification->size; box_no_y++)
        {
          // loop over all values
          for (int value = 1; value <= specification->squared_size; value++)
          {
            // check if value is already set (valueIsAlreadySet),
            // or can only be at exactly one position in this box (nPossiblePositionsForValue==1), then it is at position (positionX, positionY)
            bool valueIsAlreadySet = false;
            int nPossiblePositionsForValue = 0;
            int positionX = 0;
            int positionY = 0;

            // loop over fields in box
            for (int box_x = 0; box_x < specification->size; box_x++)
            {
              for (int box_y = 0; box_y < specification->size; box_y++)
              {
                int indexX = box_no_x*specification->size + box_x;
                int indexY = box_no_y*specification->size + box_y;

                int index = getFieldIndex(indexX, indexY, specification);
                if(testFields[index]->value() == value)
                {
                  valueIsAlreadySet = true;
                  break;
                }

                if(testFields[index]->valuePossible(value))
                {
                  nPossiblePositionsForValue++;
                  positionX = indexX;
                  positionY = indexY;
                }

                if(nPossiblePositionsForValue > 1)
                  break;
              }
              if(nPossiblePositionsForValue > 1)
                break;
              if(valueIsAlreadySet)
                break;
            }

            //if Value can only be at one position, set it there
            if(nPossiblePositionsForValue == 1 && !valueIsAlreadySet)
            {
              if(debug)
                std::cout<<"Box check (box "<<box_no_x<<","<<box_no_y<<"): value "<<value
                  <<" has to be in field ("<<positionX<<","<<positionY<<","<<specification->sudoku_no<<")"<<std::endl;

              furtherWays.back().nBoxCheckMatches++;
              int index = getFieldIndex(positionX, positionY, specification);
              furtherWays.back().addWay(index, positionX, positionY, specification->sudoku_no, value, ChainedSudoku::FurtherWays::Reason::onlyPositionInBox);

              if(!recordFullFurtherWays)
              {
                int index = getFieldIndex(positionX, positionY, specification);
                testFields[index]->setValue(value);
                restart = true;
                break;
              }
            }
          }

          if(restart)
            break;
        }
        if(restart)
          break;
      }
      if(restart)
        break;
    }
    else  // freeform
    {
      char block[][9][2] = {
        {{3,3}, {4,3}, {5,3}, {3,4}, {4,4}, {5,4}, {3,5}, {4,5}, {5,5}},    // center block
        {{0,0}, {1,0}, {3,2}, {0,1}, {1,1}, {2,1}, {2,3}, {1,2}, {2,2}},    // lower left block
        {{3,0}, {4,0}, {5,0}, {3,1}, {4,1}, {5,1}, {2,0}, {4,2}, {6,0}},    // bottom block
        {{5,2}, {7,0}, {8,0}, {6,1}, {7,1}, {8,1}, {6,2}, {7,2}, {6,3}},    // lower right block
        {{8,2}, {7,3}, {8,3}, {6,4}, {7,4}, {8,4}, {8,6}, {7,5}, {8,5}},    // right block
        {{6,6}, {7,6}, {6,5}, {6,7}, {7,7}, {8,7}, {5,6}, {7,8}, {8,8}},    // upper right block
        {{2,8}, {4,6}, {6,8}, {3,7}, {4,7}, {5,7}, {3,8}, {4,8}, {5,8}},    // top block
        {{2,5}, {1,6}, {2,6}, {0,7}, {1,7}, {2,7}, {0,8}, {1,8}, {3,6}},    // upper left block
        {{0,3}, {1,3}, {0,2}, {0,4}, {1,4}, {2,4}, {0,5}, {1,5}, {0,6}}};   // left block

      // loop over blocks
      for(int block_no = 0; block_no < 9; block_no++)
      {
        // loop over all values
        for (int value = 1; value <= specification->squared_size; value++)
        {
          // check if value is already set (valueIsAlreadySet),
          // or can only be at exactly one position in this box (nPossiblePositionsForValue==1), then it is at position (positionX, positionY)
          bool valueIsAlreadySet = false;
          int nPossiblePositionsForValue = 0;
          int positionX = 0;
          int positionY = 0;

          // loop over fields in box
          for(int i = 0; i < 9; i++)
          {
            int indexX = block[block_no][i][0];
            int indexY = block[block_no][i][1];

            int index = getFieldIndex(indexX, indexY, specification);
            if(testFields[index]->value() == value)
            {
              valueIsAlreadySet = true;
              break;
            }

            if(testFields[index]->valuePossible(value))
            {
              nPossiblePositionsForValue++;
              positionX = indexX;
              positionY = indexY;
            }

            if(nPossiblePositionsForValue > 1)
              break;
          }
          if(nPossiblePositionsForValue > 1)
            break;
          if(valueIsAlreadySet)
            break;

          //if Value can only be at one position, set it there
          if(nPossiblePositionsForValue == 1 && !valueIsAlreadySet)
          {
            if(debug)
              std::cout<<"Box check: value "<<value
                <<" has to be in field ("<<positionX<<","<<positionY<<","<<specification->sudoku_no<<")"<<std::endl;

            furtherWays.back().nBoxCheckMatches++;
            int index = getFieldIndex(positionX, positionY, specification);
            furtherWays.back().addWay(index, positionX, positionY, specification->sudoku_no, value, ChainedSudoku::FurtherWays::Reason::onlyPositionInBox);

            if(!recordFullFurtherWays)
            {
              int index = getFieldIndex(positionX, positionY, specification);
              testFields[index]->setValue(value);
              restart = true;
              break;
            }
          }
        }
        if(restart)
          break;
      }
      if(restart)
        break;
    }
    if(restart)
      break;
  }
  return restart;
}

void SudokuChain::createSolution()
{
  currentSpecifications_.clear();
  currentSpecifications_.assign(sudokuSpecifications_.begin(), sudokuSpecifications_.end());

  for(int i = 0; i < sudoku_.size(); i++)
  {
    sudoku_[i]->createSolution(i);
  }
}

void SudokuChain::fixSomeFieldsToGetASolvableSudoku()
{
  if(sudoku_.empty())
    return;


  // prepare first sudoku object to fix fields for the complete sudoku chain
  sudoku_.front()->field_begin_ = fields_.begin();
  sudoku_.front()->field_end_ = fields_.end();
  sudoku_.front()->externFieldCheck_ = &SudokuChain::fieldCheck;
  sudoku_.front()->externRowCheck_ = &SudokuChain::rowCheck;
  sudoku_.front()->externColumnCheck_ = &SudokuChain::columnCheck;
  sudoku_.front()->externBoxCheck_ = &SudokuChain::boxCheck;

  currentSpecifications_.clear();
if(false)
  for(int i = 0; i < sudoku_.size(); i++)
  {

    std::shared_ptr<SudokuSpecification> currentSpecification = sudokuSpecifications_[i];

    int firstIndex = 0;
    int lastIndex = 81;
    if(currentSpecification->previous)
    {
      //firstIndex = currentSpecification->previous->firstOwnIndex;
    }
    if(currentSpecification->next)
    {
      lastIndex = currentSpecification->next->firstOwnIndex;
    }
    currentSpecifications_.push_back(currentSpecification);


    // prepare first sudoku object to fix fields for the complete sudoku chain
    //sudoku_.front()->field_begin_ = fields_.begin()+firstIndex;
    sudoku_.front()->field_end_ = fields_.begin()+lastIndex;

    std::cout<<i<<" index range: "<<firstIndex<<","<<lastIndex<<std::endl;

    // set some fields in fields_ to fixed, working fields for solving are testFields
    Sudoku::FieldVector testFields;
    sudoku_.front()->fixSomeFieldsToGetASolvableSudoku(false, testFields);

    std::cout<<"copy back"<<std::endl;

    // copy fixed status back
    auto field_iter = fields_.begin();
    for(auto &field : testFields)
    {
      if(field->fixed())
      {
        (*field_iter)->setFixed(true);
      }
      field_iter++;
    }
  }
  //return;


  currentSpecifications_.assign(sudokuSpecifications_.begin(), sudokuSpecifications_.end());

  // prepare first sudoku object to fix fields for the complete sudoku chain
  sudoku_.front()->field_begin_ = fields_.begin();
  sudoku_.front()->field_end_ = fields_.end();
  sudoku_.front()->externFieldCheck_ = &SudokuChain::fieldCheck;
  sudoku_.front()->externRowCheck_ = &SudokuChain::rowCheck;
  sudoku_.front()->externColumnCheck_ = &SudokuChain::columnCheck;
  sudoku_.front()->externBoxCheck_ = &SudokuChain::boxCheck;

  // set some fields in fields_ to fixed, working fields for solving are testFields
  Sudoku::FieldVector testFields;
  sudoku_.front()->fixSomeFieldsToGetASolvableSudoku(false, testFields);

  // copy fixed status back
  auto field_iter = fields_.begin();
  for(auto &field : testFields)
  {
    if(field->fixed())
    {
      (*field_iter)->setFixed(true);
    }
    field_iter++;
  }
}

void SudokuChain::collectSolutionHints(std::string filename)
{
  if(sudoku_.empty())
    return;

  sudoku_.front()->collectSolutionHints(filename);
}

void SudokuChain::outputSpecification(std::vector<std::shared_ptr<SudokuChain::SudokuSpecification>> specifications)
{
  std::cout<<"sudoku specification: "<<std::endl;
  int i=0;
  for(auto &spec : specifications)
  {
    std::cout<<"  "<<i<<" "<<spec->squared_size<<"x"<<spec->squared_size
      <<" "<<Sudoku::types[int(spec->type)]
      <<", offset: ("<<spec->offset_from_previous_x<<","<<spec->offset_from_previous_y<<") "
      <<"starting index: "<<spec->firstOwnIndex;
    if(spec->previous)
    {
      std::cout<<" (previous set)";
    }
    if(spec->next)
    {
      std::cout<<" (next set)";
    }
    std::cout<<std::endl;
    i++;
  }
}

void SudokuChain::print(ChainedSudoku::PrintTarget target, std::string filename, ChainedSudoku::PrintMode mode)
{
  if(target == ChainedSudoku::PrintTarget::svg_file_body)
  {
    // begin svg file
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
    double width = 30*nFieldsX_;
    double height = 30*nFieldsY_;
    file<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<svg width=\""<<width<<"\" height=\""<<height<<"\">\n<g>\n";
    file.close();
  }

  int offset_x = 0;
  int offset_y = -coordinateYbottom_;
  for(auto& specification : sudokuSpecifications_)
  {
    offset_x += specification->offset_from_previous_x;
    offset_y += specification->offset_from_previous_y;
    sudoku_[specification->sudoku_no]->print(target, filename, mode, offset_x, offset_y, specification->sudoku_no+number_offset);
  }

  if(target == ChainedSudoku::PrintTarget::svg_file_body)
  {
    // end svg file
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<"\n</g>\n</svg>\n";
    file.close();
  }
}



/*

ChainedSudoku::ChainedSudoku() : Sudoku(0)
{
  exit(0);
  n_sudokus_ = 4;

  int size_0 = 3;
  int squared_size_0 = size_0 * size_0;
  int n_fields_sudoku_0 = squared_size_0*squared_size_0;

  int size_1 = 4;
  int squared_size_1 = size_1 * size_1;
  int n_fields_sudoku_1 = squared_size_1*squared_size_1;
  int n_fields_overlapping_01 = 3*3;

  int size_2 = 3;
  int squared_size_2 = size_2 * size_2;
  int n_fields_sudoku_2 = squared_size_2*squared_size_2;
  int n_fields_overlapping_12 = 3;

  int size_3 = 3;
  int squared_size_3 = size_3 * size_3;
  int n_fields_sudoku_3 = squared_size_3*squared_size_3;
  int n_fields_overlapping_23 = 9;

  for(int z = 0; z < n_sudokus_; z++)
  {
    switch_to_current_z(z);
    for(int x = 0; x < squared_size_; x++)
    {
      for(int y = 0; y < squared_size_; y++)
      {
        int i = index(x,y);
        int pos_x, pos_y, pos_z;
        get_coordinates(pos_x, pos_y, pos_z, i);

        if(pos_x != x || pos_y != y || pos_z != z)
        {
          std::cout<<"index "<<i<<", (x,y,z)=("<<x<<","<<y<<","<<z<<"), get_coordinates("<<pos_x<<","<<pos_y<<","<<pos_z<<")"<<std::endl;

        }
      }
    }
  }

  // allocate total fields
  n_fields_ = n_fields_sudoku_0 + n_fields_sudoku_1 + n_fields_sudoku_2 + n_fields_sudoku_3
    - n_fields_overlapping_01 - n_fields_overlapping_12 - n_fields_overlapping_23;
  //n_fields_ = 468;

  std::cout<<"n_fields: "<<n_fields_;

  fields_.resize(n_fields_);

  //create all fields
  for(int index = 0; index < n_fields_; index++)
  {
    int x,y,z;
    get_coordinates(x, y, z, index);
    fields_[index] = std::move(std::unique_ptr<Field>(new Field(index, x, y, z, max_value(z))));
  }

//  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//  std::shuffle(fields_.begin(), fields_.end(), std::default_random_engine(seed));

//  for(int index = 0; index < n_fields_; index++)
//  {
//    fields_[index]->setId(index);
//  }


  addDependencies();
}


void ChainedSudoku::setFixed()
{
  const bool debug = false;
  const bool writeToFile = false;

  if(debug)
  {
    std::cout<<"ChainedSudoku::setFixed()"<<std::endl;
    std::cout<<"reference solution: "<<std::endl;
    print();
  }

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();

  //create second sudoku with same dependencies but without values
  ChainedSudoku test;

  std::vector<std::shared_ptr<Field>> &testFields = test.fields_;


  furtherWays_.clear();

  const bool recordFullFurtherWays = true;    //if at each stage all possible further ways should be counted and set in furtherWays_ (if not, it is faster)
  const bool fixFieldChooseRandomly = true;     //if new fixed fields should be chosen randomly

  bool lastLoopHadFix = false;
  for(;;)
  {
    if(debug)
    {
      for(int z = 0; z < n_sudokus_; z++)
      {
        switch_to_current_z(z);
        printFields(testFields);
      }
    }

    if(!lastLoopHadFix)
      furtherWays_.push_back(FurtherWays());

    lastLoopHadFix = false;


    int minPossible = 9;
    int maxPossible = 0;

    //check if there are fields with only one possible value
    //loop over not set not fixed fields
    int nFieldsNotSet = 0;
    for(auto &field : testFields)
    {
      if(field->fixed())
        continue;

      if(field->value() != 0)
        continue;

      nFieldsNotSet++;

      field->storePossible();
      int nPossible = field->n_possible();

      if(nPossible > maxPossible)
      {
        maxPossible = nPossible;
        //if(debug)std::cout<<" Field("<<field->pos()<<") has nPossible="<<nPossible<<" (new maximum)"<<std::endl;
      }
      if(nPossible < minPossible)
      {
        minPossible = nPossible;
        //if(debug)std::cout<<" Field("<<field->pos()<<") has nPossible="<<nPossible<<" (new minimum)"<<std::endl;
      }
    }

    if(debug)
      std::cout<<"minPossible="<<minPossible<<", maxPossible="<<maxPossible<<", nFieldsNotSet="<<nFieldsNotSet<<std::endl;

    if(nFieldsNotSet == 0)
      break;

    bool restart = false;

    if(minPossible == 0)
      std::cout<<"Error: Sudoku seems to be not solvable"<<std::endl;

    if(minPossible == 1)
    {
      //if there are fields that have only one possible value, set this value
      for(auto &field : testFields)
      {
        if(field->value() == 0)
        {
          if(field->n_possible() == 1)
          {
            if(debug)
              std::cout<<"Field check: field ("<<field->pos()<<") (index: "<<field->id()<<") must be "<<field->onePossibleValue()<<""<<std::endl;

            furtherWays_.back().nFieldCheckMatches++;
            furtherWays_.back().addWay(field->id(), field->pos_x(), field->pos_y(), field->pos_z(), field->onePossibleValue());

            if(!recordFullFurtherWays)
              field->setValue(field->onePossibleValue());
          }
        }
      }
      if(!recordFullFurtherWays)
        continue;
    }

    // check rows,cols,boxes
    for(int z = 0; z < n_sudokus_; z++)
    {
      switch_to_current_z(z);
      restart = Sudoku::rowCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

      if(restart)
        break;

      restart = Sudoku::columnCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

      if(restart)
        break;

      restart = Sudoku::boxCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

      if(restart)
        break;
    }

    if(restart)
        continue;

    if(recordFullFurtherWays)
    {
      int nPossibleFurtherWays = furtherWays_.back().nMatches();

      if(debug)
      {
        std::cout<<nPossibleFurtherWays<<" possible further ways:"<<std::endl
          <<furtherWays_.back().nFieldCheckMatches<<" field check matches"<<std::endl
          <<furtherWays_.back().nRowCheckMatches<<" row check matches"<<std::endl
          <<furtherWays_.back().nColumnCheckMatches<<" column check matches"<<std::endl
          <<furtherWays_.back().nBoxCheckMatches<<" box check matches"<<std::endl;
      }

      if(nPossibleFurtherWays > 0)
      {
        //choose random possible action to do
        int randomIndex = rand() % nPossibleFurtherWays;

        std::set<FurtherWays::Way>::iterator wayIter = furtherWays_.back().ways.begin();
        for(int i=0; i<randomIndex; i++)
          wayIter++;

        FurtherWays::Way way = (*wayIter);

        if(debug)
          std::cout<<"set field ("<<way.posX<<","<<way.posY<<","<<way.posZ<<") (index: "<<way.id<<") to value "<<way.value<<std::endl;

        switch_to_current_z(way.posZ);

        testFields[way.id]->setValue(way.value);

        if(debug)
        {
          std::cout<<"press a key";
          std::cin.get();
        }
        continue;
      }
    }

    if(debug)
      std::cout<<"No progress, set a new value fixed, maxPossible="<<maxPossible<<std::endl;


    //here no field has only one possible value and in no row,column or 3x3-box a value can be localized certainly
    //make a value with the most possible values available

    fixField(testFields, maxPossible, fixFieldChooseRandomly, debug);
    lastLoopHadFix = true;
  }

  //set fixed in current Sudoku
  std::vector<std::shared_ptr<Field>>::iterator fieldIter = fields_.begin();
  std::vector<std::shared_ptr<Field>>::iterator testFieldIter = testFields.begin();

  for(; fieldIter != fields_.end(); fieldIter++, testFieldIter++)
  {
    (*fieldIter)->setFixed((*testFieldIter)->fixed());
  }

  std::cout<<n_fixed()<<" Fields are fixed"<<std::endl;
  if(recordFullFurtherWays)
  {

    if(debug)
      std::cout<<"Number of matches in solution path: ";

    std::stringstream s;
    s<<"results/sudoku_"<<squared_size_<<"x"<<squared_size_<<"_path_";

    int i=0;
    for(auto &furtherWay : furtherWays_)
    {
      if(debug)
        std::cout<<furtherWay.nMatches()<<" ";

      if(i < 25)
        s<<furtherWay.nMatches()<<"_";
      i++;
    }
    if(debug)
      std::cout<<std::endl;

    s<<"fixed_"<<n_fixed()<<".txt";

    filename_ = s.str();
    if(writeToFile)
      print(PrintTarget::text_file, filename_);
  }

  std::chrono::time_point<std::chrono::system_clock> tEnd = std::chrono::system_clock::now();
  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);
  std::cout<<"Duration: "<<duration.count()<<" ms"<<std::endl;
}

void ChainedSudoku::fixField(std::vector<std::shared_ptr<Field>> &testFields, int maxPossible, bool fixFieldChooseRandomly, bool debug)
{
  int posXtoFix = 0;
  int posYtoFix = 0;
  int posZtoFix = 0;
  int fieldId = 0;

  if(fixFieldChooseRandomly)
  {
    for(;;)
    {
      fieldId = rand() % (n_fields_);
      if(testFields[fieldId]->value() == 0)
      {
        posXtoFix = testFields[fieldId]->pos_x();
        posYtoFix = testFields[fieldId]->pos_y();
        posZtoFix = testFields[fieldId]->pos_z();
        break;
      }
    }
  }
  else
  {
    //take the first field that has the maximum number of possibilities
    for(auto &field : testFields)
    {
      if(field->value() != 0)
        continue;

      if(field->n_possible() == maxPossible)
      {
        posXtoFix = field->pos_x();
        posYtoFix = field->pos_y();
        posZtoFix = field->pos_z();
        fieldId = field->id();
        break;
      }
    }
  }

  int correctValue = fields_[fieldId]->value();

  testFields[fieldId]->setValue(correctValue);
  testFields[fieldId]->setFixed(true);

  if(debug)
    std::cout<<"ChainedSudoku Fix field ("<<posXtoFix<<","<<posYtoFix<<","<<posZtoFix<<") (index "<<fieldId<<") to value "<<correctValue<<std::endl;
}

void ChainedSudoku::solveExisting()
{
  const bool debug = false;

  std::stringstream solvingHints;
  solvingHints<<"Möglicher Lösungsweg:"<<std::endl;

  if(debug)
  {
    std::cout<<"ChainedSudoku::solveExisting()"<<std::endl;
  }

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();

  furtherWays_.clear();

  const bool recordFullFurtherWays = true;    //if at each stage all possible further ways should be counted and set in furtherWays_ (if not, it is faster)
  const bool fixFieldChooseRandomly = true;     //if new fixed fields should be chosen randomly

  bool lastLoopHadFix = false;
  for(;;)
  {
    if(debug)
    {
      for(int z = 0; z < n_sudokus_; z++)
      {
        switch_to_current_z(z);
        printFields(fields_);
      }
    }

    if(!lastLoopHadFix)
    {
      furtherWays_.push_back(FurtherWays());
    }

    lastLoopHadFix = false;


    int minPossible = 9000;
    int maxPossible = 0;

    //check if there are fields with only one possible value
    //loop over not set not fixed fields
    int nFieldsNotSet = 0;
    for(auto &field : fields_)
    {
      if(field->fixed())
        continue;

      if(field->value() != 0)
        continue;

      nFieldsNotSet++;

      field->storePossible();
      int nPossible = field->n_possible();

      if(nPossible > maxPossible)
      {
        maxPossible = nPossible;
        //if(debug)std::cout<<" Field("<<field->pos()<<") has nPossible="<<nPossible<<" (new maximum)"<<std::endl;
      }
      if(nPossible < minPossible)
      {
        minPossible = nPossible;
        //if(debug)std::cout<<" Field("<<field->pos()<<") has nPossible="<<nPossible<<" (new minimum)"<<std::endl;
      }
    }

    if(debug)
      std::cout<<"minPossible="<<minPossible<<", maxPossible="<<maxPossible<<", nFieldsNotSet="<<nFieldsNotSet<<std::endl;

    if(nFieldsNotSet == 0)
      break;

    bool restart = false;

    if(minPossible == 0)
      std::cout<<"Error: Sudoku seems to be not solvable"<<std::endl;

    if(minPossible == 1)
    {
      //if there are fields that have only one possible value, set this value
      for(auto &field : fields_)
      {
        if(field->value() == 0)
        {
          if(field->n_possible() == 1)
          {
            if(debug)
              std::cout<<"Field check: field ("<<field->pos()<<") (index: "<<field->id()<<") must be "<<field->onePossibleValue()<<""<<std::endl;

            furtherWays_.back().nFieldCheckMatches++;
            furtherWays_.back().addWay(field->id(), field->pos_x(), field->pos_y(), field->pos_z(), field->onePossibleValue());

            if(!recordFullFurtherWays)
              field->setValue(field->onePossibleValue());
          }
        }
      }
      if(!recordFullFurtherWays)
        continue;
    }

    // check rows,cols,boxes
    for(int z = 0; z < n_sudokus_; z++)
    {
      switch_to_current_z(z);
      restart = Sudoku::rowCheck(furtherWays_, fields_, recordFullFurtherWays, debug);

      if(restart)
        break;

      restart = Sudoku::columnCheck(furtherWays_, fields_, recordFullFurtherWays, debug);

      if(restart)
        break;

      restart = Sudoku::boxCheck(furtherWays_, fields_, recordFullFurtherWays, debug);

      if(restart)
        break;
    }

    if(restart)
        continue;

    if(recordFullFurtherWays)
    {
      int nPossibleFurtherWays = furtherWays_.back().nMatches();

      if(debug)
      {
        std::cout<<nPossibleFurtherWays<<" possible further ways:"<<std::endl
          <<furtherWays_.back().nFieldCheckMatches<<" field check matches"<<std::endl
          <<furtherWays_.back().nRowCheckMatches<<" row check matches"<<std::endl
          <<furtherWays_.back().nColumnCheckMatches<<" column check matches"<<std::endl
          <<furtherWays_.back().nBoxCheckMatches<<" box check matches"<<std::endl;
      }

      if(nPossibleFurtherWays > 0)
      {
        //choose random possible action to do
        int randomIndex = rand() % nPossibleFurtherWays;

        std::set<FurtherWays::Way>::iterator wayIter = furtherWays_.back().ways.begin();
        for(int i=0; i<randomIndex; i++)
          wayIter++;

        FurtherWays::Way way = (*wayIter);

        if(debug)
          std::cout<<"set field ("<<way.posX<<","<<way.posY<<","<<way.posZ<<") (index: "<<way.id<<") to value "<<way.value<<std::endl;

        switch_to_current_z(way.posZ);

        switch(way.reason)
        {
          case 0: //field
            solvingHints<<"Sudoku "<<char('A'+way.posZ)<<", Feld ("<<way.posY+1<<","<<way.posX+1<<"). Hier kann nur "<<way.value<<" sein. ";
            break;
          case 1: //row
            solvingHints<<"Sudoku "<<char('A'+way.posZ)<<", Wert "<<way.value<<" in Zeile "<<way.posY+1<<". ";
            break;
          case 2: //column
            solvingHints<<"Sudoku "<<char('A'+way.posZ)<<", Wert "<<way.value<<" in Spalte "<<way.posX+1<<". ";
            break;
          case 3: //box

            solvingHints<<"Sudoku "<<char('A'+way.posZ)<<", Wert "<<way.value<<" im "<<size_<<"x"<<size_<<"-Quadrat";

            if(int(way.posX / size_) == 0)
              solvingHints<<" links";
            else if(int(way.posX / size_) == 1)
              solvingHints<<" Mitte";
            else
              solvingHints<<" rechts";

            if(int(way.posY / size_) == 0)
              solvingHints<<" oben";
            else if(int(way.posY / size_) == 1 && !(int(way.posX / size_) == 1))
              solvingHints<<" Mitte";
            else if(int(way.posY / size_) == 2)
              solvingHints<<" unten";
            solvingHints<<". ";

            break;
        }

        solvingHints<<"Schreibe "<<way.value<<" in Sudoku "<<char('A'+way.posZ)<<", Feld ("<<way.posY+1<<","<<way.posX+1<<")."<<std::endl;


        switch_to_current_z(way.posZ);

        fields_[way.id]->setValue(way.value);
        continue;
      }
    }

    //here no field has only one possible value and in no row,column or 3x3-box a value can be localized certainly

    if(debug)
      std::cout<<"No progress, set a new value fixed (should not happen here!), maxPossible="<<maxPossible<<std::endl;

    std::cout<<"Sudoku is either not solvable or needs backtracking to solve (=\"trying\"). This method cannot handle it. Aborting."<<std::endl;
    return;
  }

  if(recordFullFurtherWays)
  {
    if(debug || 1)
    {
      std::cout<<"Number of matches in solution path: ";
      std::stringstream s;
      s<<"results/sudoku_"<<squared_size_<<"x"<<squared_size_<<"_path_";

      int i=0;
      for(auto &furtherWay : furtherWays_)
      {
        std::cout<<furtherWay.nMatches()<<" ";
        if(i < 25)
          s<<furtherWay.nMatches()<<"_";
        i++;
      }
      std::cout<<std::endl;

      s<<"fixed_"<<n_fixed()<<".txt";

      std::string filename = s.str();
      print(PrintTarget::text_file, filename);
    }
  }

  std::chrono::time_point<std::chrono::system_clock> tEnd = std::chrono::system_clock::now();
  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);
  std::cout<<"Duration: "<<duration.count()<<" ms"<<std::endl;

  std::cout<<"Solution Hints are written to file \"Lösungsweg.txt\"."<<std::endl;

  std::ofstream file("Lösungsweg.txt");
  if(!file.is_open())
    std::cout<<"Could not write to file \"Lösungsweg.txt\"."<<std::endl;
  file<<solvingHints.str();
  file.close();
  solutionHints_ = solvingHints.str();

}

*/
