#include "sudoku.h"

#include <stack>
#include <chrono>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <list>
#include <algorithm>
#include <ctime>
#include <random>

#include "field.h"

const int RESTART_THRESHOLD = 20000; // 1000000

const std::string Sudoku::types[5] = {
    "classical",            // the standard sudoku rules
    "diagonal",        // standard sudoku with addional diagonal dependencies
    "left_side_inverted",   // mirror fields in first blocks on the left
    "freeform",             // sudoku with differently shaped blocks
    "sum"                   // sum sudoku
    };

Sudoku::Sudoku(int size, SudokuType type) : type_(type), size_(size), squared_size_(size*size), current_z_(0), filename_("out.txt")
{
  fields_.resize(squared_size_*squared_size_);

  //srand(std::time(NULL));

  //create fields
  for(int x = 0; x < squared_size_; x++)
  {
    for (int y = 0; y < squared_size_; y++)
    {
      fields_[index(x,y)] = std::move(std::unique_ptr<Field>(new Field(index(x,y),x,y,current_z_,squared_size_)));
    }
  }
  /*
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(fields_.begin(), fields_.end(), std::default_random_engine(seed));
  */

  field_begin_= fields_.begin();
  field_end_ = fields_.end();
  addDependencies();
}

void Sudoku::addDependencies()
{
  addDependenciesFields(fields_);
}

void Sudoku::addDependenciesFields(std::vector<std::shared_ptr<Field>> &fields)
{
  if(size_ == 0)
    return;

  //add dependencies
  // loop over all fields
  for(int x = 0; x < squared_size_; x++)    // x,y: global index of field for which dependencies are added
  {
    for (int y = 0; y < squared_size_; y++)
    {
      // loop over all fields in same block3x3
      for (int field3x3_x = 0; field3x3_x < size_; field3x3_x++)  // field3x3_x, field3x3_y: local index in own block3x3
      {
        for (int field3x3_y = 0; field3x3_y < size_; field3x3_y++)
        {

          int block3x3_no_x = int(x / size_);     // own index of block3x3
          int block3x3_no_y = int(y / size_);

          //add dependencies to all of 3x3 block
          if(type_ != SudokuType::freeform || size_ != 3)
          {
            int dependency_x = block3x3_no_x*size_ + field3x3_x;
            int dependency_y = block3x3_no_y*size_ + field3x3_y;

            if(dependency_x != x || dependency_y != y)
              fields[index(x,y)]->addDependency(fields_[index(dependency_x,dependency_y)]);
          }

          //add dependencies to all of row
          int dependency_x = field3x3_x*size_ + field3x3_y;   // this is actually an own loop over x=0..squared_size
          int dependency_y = y;

          if(dependency_x != x || dependency_y != y)
            fields[index(x,y)]->addDependency(fields_[index(dependency_x,dependency_y)]);

          //add dependencies to all of column
          dependency_x = x;
          dependency_y = field3x3_x*size_ + field3x3_y; // this is actually an own loop over y=0..squared_size

          if(dependency_x != x || dependency_y != y)
            fields[index(x,y)]->addDependency(fields_[index(dependency_x,dependency_y)]);
        }
      }
    }
  }

  // add diagonal dependencies
  if(type_ == SudokuType::diagonal)
  {
    for(int i = 0; i < squared_size_; i++)
    {
      for(int j = 0; j < squared_size_; j++)
      {
        // diagonal lower left to upper right
        if(i != j)
          fields[index(i,i)]->addDependency(fields_[index(j,j)]);

        // diagonal upper left to lower right
        if(i != j)
          fields[index(i,squared_size_-1-i)]->addDependency(fields_[index(j,squared_size_-1-j)]);
      }
    }
  }

  // add block dependencies for freeform
  if(type_ == SudokuType::freeform && size_ == 3)
  {
    // center block
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

    for(int block_no = 0; block_no < 9; block_no++)
    {
      for(int i = 0; i < 9; i++)
      {
        int x = block[block_no][i][0];
        int y = block[block_no][i][1];

        for(int j = 0; j < 9; j++)
        {
          int dependency_x = block[block_no][j][0];
          int dependency_y = block[block_no][j][1];

          if(dependency_x != x || dependency_y != y)
          {
            fields[index(x,y)]->addDependency(fields_[index(dependency_x,dependency_y)]);
          }
        }
      }
    }
  }

}

void Sudoku::permuteNumbers()
{
  //generate random permutation
  //srand(std::time(NULL));
  //std::vector<int> permutation(squared_size_) = {21, 8, 1, 19, 14, 10, 4, 18, 24, 5, 16, 9, 3, 7, 6, 22};
  //std::vector<int> permutation = {21, 8, 1, 19, 14, 10, 4, 18, 24, 5, 16, 9, 3, 7, 6, 22, };
  //                              1, 2,  3,  4,  5, 6,  7,  8, 9, 10, 11, 12, 13,14, 15, 16, 17,18,19, 20,21, 22, 23,24, 25
  std::vector<int> permutation = {3, 24, 13, 7, 10, 15, 14, 2, 12, 6, 22, 21, 13, 5, 15, 11, 17, 8, 4, 20, 1, 16, 23, 9, 25};

  if(false)
  {
    for(int i=0; i<squared_size_; i++)
    {
      permutation[i] = i+1;
    }

    for(int i=0; i<squared_size_; i++)
    {
      int index = rand() % (squared_size_-i) + i;
      std::swap(permutation[i], permutation[index]);
    }
  }

  // apply permutation
  for(int x = 0; x < squared_size_; x++)
  {
    for (int y = 0; y < squared_size_; y++)
    {
      if(fields_[index(x,y)]->value() > 0)
        fields_[index(x,y)]->setValue(permutation[fields_[index(x,y)]->value()-1]);
    }
  }
}

int Sudoku::index(int x, int y)
{
  return y*squared_size_ + x;
}

void Sudoku::incrementFieldIteratorToNextUnfixed(std::vector<std::shared_ptr<Field>>::iterator &nextField)
{
  do
  {
    nextField++;
    if(nextField == field_end_)
      break;
  }
  while((*nextField)->fixed());

}

void Sudoku::decrementFieldIteratorToNextUnfixed(std::vector<std::shared_ptr<Field>>::iterator &nextField)
{
  do
  {
    nextField--;
    if(nextField == field_begin_)
      break;
  }
  while((*nextField)->fixed());
}

int Sudoku::nFixed()
{
  int nFixed = 0;
  //for(auto &field : fields_)
  for(auto field_iter = field_begin_; field_iter != field_end_; field_iter++)
  {
    nFixed += (*field_iter)->fixed();
  }
  return nFixed;
}

void Sudoku::setFixed(int pos_x, int pos_y, bool fixed)
{
  fields_[index(pos_x,pos_y)]->setFixed(fixed);
}

void Sudoku::set(int pos_x, int pos_y, int value, bool fixed)
{
  fields_[index(pos_x,pos_y)]->setValue(value);
  fields_[index(pos_x,pos_y)]->setFixed(fixed);
}

void Sudoku::createSolution(int number)
{
  bool debug = false;
  //if(number >= 9)
  //  debug = true;
  std::cout<<number<<" create solution, type: "<<types[int(type_)]<<", size: "<<squared_size_<<"x"<<squared_size_<<std::endl;

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();

  // normally all fields should have a value 0 and triedValues should be completely empty for each field
  // if there are fields that already have a value != 0, set triedValues to just exclude this value, such that it is the last missing to be tried
  for(auto fieldIter = field_begin_; fieldIter != field_end_; fieldIter++)
  {
    if((*fieldIter)->max_value() != squared_size_)
      std::cout<<"Warning, max value="<<(*fieldIter)->max_value()<<" != "<<squared_size_<<std::endl;
    if((*fieldIter)->value() != 0)
      (*fieldIter)->setAllTriedExceptValue();
  }

  //std::random_shuffle(fields_.begin(), fields_.end());
  // iterator that points to the next unfixed field
  std::vector<std::shared_ptr<Field>>::iterator nextField = field_begin_;

  // stack trace
  std::stack<std::shared_ptr<Field>> trace;

  trace.push(*nextField);

  incrementFieldIteratorToNextUnfixed(nextField);

  if(debug)
  {
    print();
    std::cout<<"current top field: "<<trace.top()->pos()<<" (next Field: "<<(*nextField)->pos()<<")"<<std::endl;
  }

  int visitedFieldsCount = 0;
  bool first400FieldSolved = false;
  do
  {
    // try to fill in a new value to the field on top of the stack
    if(trace.top()->setNewValue())    // new value found
    {
      if(debug)
        std::cout<<"success"<<std::endl;

      // if all fields are filled in, method is done
      if(nextField == field_end_)
        break;
      else
      {
        // push recently filled in value to stack trace
        trace.push(*nextField);
      }
      incrementFieldIteratorToNextUnfixed(nextField);
    }
    else    // no new value possible
    {
      if(debug)
        std::cout<<"no success, tracking back"<<std::endl;
      trace.top()->resetTriedValues();
      trace.pop();
      decrementFieldIteratorToNextUnfixed(nextField);
    }
    visitedFieldsCount++;

    if(debug)
    {
      current_z_ = trace.top()->pos_z();		//only for output
      print();
      std::cout<<"current top field: "<<trace.top()->pos();
      if(nextField != field_end_)
        std::cout<<" (next Field: "<<(*nextField)->pos()<<")"<<std::endl;
      else
        std::cout<<std::endl;
      std::cout<<std::string(80,'-')<<std::endl;

      if(visitedFieldsCount % 10 == 0)
      {
        std::cout<<"(press any key to continue) "<<std::endl;
        std::cin.get();
      }
    }

    if(visitedFieldsCount % 10000 == 0)
      std::cout<<std::string(80, '\b')<<visitedFieldsCount<<" steps, trace size: "<<trace.size()<<"      ";

    if(visitedFieldsCount >= (size_>=4? 5*RESTART_THRESHOLD : RESTART_THRESHOLD) && !first400FieldSolved)
    {
      if(trace.size() >= 400)
      {
        first400FieldSolved = true;
      }
      else
      {
        // restart whole procedure
        std::cout<<"restart at "<<visitedFieldsCount<<" ["<<std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()-tBegin).count()<<"s]   ";
        visitedFieldsCount = 0;

        // clear trace
        while(!trace.empty())
        {
          trace.top()->resetTriedValues();
          trace.pop();
        }
        nextField = field_begin_;
        trace.push(*nextField);
        incrementFieldIteratorToNextUnfixed(nextField);
      }
    }
  }
  while(!trace.empty());
  std::cout<<"  done after "<<visitedFieldsCount<<" steps. ";
  /*
  auto compare = [](std::shared_ptr<Field> &a, std::shared_ptr<Field> &b)
  {
    return a->pos_z() < b->pos_z() ||
      (a->pos_z() == b->pos_z() && (
	a->pos_y() < b->pos_y()
	|| (a->pos_y() == b->pos_y() && a->pos_x() < b->pos_x())));
  };

  std::sort(fields_.begin(), fields_.end(), compare);
  */

  std::chrono::time_point<std::chrono::system_clock> tEnd = std::chrono::system_clock::now();
  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);
  std::cout<<"Duration: "<<duration.count()<<" ms, ";

  if(trace.empty())
  {
    std::cout<<"Sudoku has no solution!"<<std::endl;
    print();
    std::cout<<"Sudoku has no solution!"<<std::endl;
    exit(0);
  }
  else
  {
    std::cout<<"Trace size: "<<trace.size()<<std::endl;
    if(debug
    || number == 8)
    {
      std::cout<<number<<" solution, type: "<<types[int(type_)]<<", size: "<<squared_size_<<"x"<<squared_size_<<":"<<std::endl;
      print();
    }
  }
}

bool Sudoku::fieldCheck(std::list<FurtherWays> &furtherWays_, FieldVector &testFields, bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  // check if there are fields that can have only one possible value
  restart = false;

  // loop over fields
  for(auto &testField : testFields)
  {
    if(testField->value() == 0)   // if value is not yet set
    {
      if(testField->n_possible() == 1)    // if there is exactly one possiblility
      {
        if(debug)
          std::cout<<"  Field check: field ("<<testField->pos()<<") must be "<<testField->onePossibleValue()<<""<<std::endl;

        furtherWays_.back().nFieldCheckMatches++;
        furtherWays_.back().addWay(testField->id(), testField->pos_x(), testField->pos_y(), testField->pos_z(),
          testField->onePossibleValue(), FurtherWays::Reason::onlyPossibilityInField);

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
  return restart;
}

bool Sudoku::rowCheck(std::list<FurtherWays> &furtherWays_, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  //check rows, if there are values that can only be at one position
  restart = false;

  // loop over rows
  for (int y = 0; y < squared_size_; y++)
  {
    // loop over all values
    for (int value = 1; value <= squared_size_; value++)
    {
      // check if value is already set (valueIsAlreadySet),
      // or can only be at exactly one position in this row (nPossiblePositionsForValue==1), then it is position positionX
      bool valueIsAlreadySet = false;
      int nPossiblePositionsForValue = 0;
      int positionX = 0;

      // loop over fields in row
      for (int x = 0; x < squared_size_; x++)
      {
        int i = index(x,y);
        int si = testFields.size();
        if(testFields[index(x,y)]->value() == value)
        {
          valueIsAlreadySet = true;
          break;
        }

        if(testFields[index(x,y)]->valuePossible(value))
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
          std::cout<<"  Row check (row "<<y<<"): value "<<value<<" has to be in field ("<<positionX<<","<<y<<","<<current_z_<<")"<<std::endl;

        furtherWays_.back().nRowCheckMatches++;    // increment number of occurences that a value can only be at one position in a row
        furtherWays_.back().addWay(index(positionX,y), positionX, y, current_z_, value, FurtherWays::Reason::onlyPositionInRow);

        if(!recordFullFurtherWays)
        {
          testFields[index(positionX,y)]->setValue(value);
          restart = true;
          break;
        }
      }
    }
    if(restart)
      break;
  }
  return restart;
}

bool Sudoku::columnCheck(std::list<FurtherWays> &furtherWays_, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  //check columns, if there are values that can only be at one position
  restart = false;
  for (int x = 0; x < squared_size_; x++)
  {

    for (int value = 1; value <= squared_size_; value++)
    {
      bool valueIsAlreadySet = false;
      int nPossiblePositionsForValue = 0;
      int positionY = 0;
      for (int y = 0; y < squared_size_; y++)
      {
        if(testFields[index(x,y)]->value() == value)
        {
          valueIsAlreadySet = true;
          break;
        }

        if(testFields[index(x,y)]->valuePossible(value))
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
          std::cout<<"Column check (column "<<x<<"): value "<<value<<" has to be in field ("<<x<<","<<positionY<<","<<current_z_<<")"<<std::endl;

        furtherWays_.back().nColumnCheckMatches++;
        furtherWays_.back().addWay(index(x,positionY), x, positionY, current_z_, value, FurtherWays::Reason::onlyPositionInColumn);

        if(!recordFullFurtherWays)
        {
          testFields[index(x,positionY)]->setValue(value);
          restart = true;
          break;
        }
      }
    }
    if(restart)
      break;
  }

  return restart;
}

bool Sudoku::boxCheck(std::list<FurtherWays> &furtherWays_, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

  //check 3x3 boxes, if there are values that can only be at one position
  restart = false;
  for (int field3x3_no_x = 0; field3x3_no_x < size_; field3x3_no_x++)
  {
    for (int field3x3_no_y = 0; field3x3_no_y < size_; field3x3_no_y++)
    {
      for (int value = 1; value <= squared_size_; value++)
      {
        bool valueIsAlreadySet = false;
        int nPossiblePositionsForValue = 0;
        int positionX = 0;
        int positionY = 0;

        for (int field3x3_x = 0; field3x3_x < size_; field3x3_x++)
        {
          for (int field3x3_y = 0; field3x3_y < size_; field3x3_y++)
          {
            int indexX = field3x3_no_x*size_ + field3x3_x;
            int indexY = field3x3_no_y*size_ + field3x3_y;

            if(testFields[index(indexX,indexY)]->value() == value)
            {
              valueIsAlreadySet = true;
              break;
            }

            if(testFields[index(indexX,indexY)]->valuePossible(value))
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
            std::cout<<"Box check (box "<<field3x3_no_x<<","<<field3x3_no_y<<"): value "<<value<<" has to be in field ("<<positionX<<","<<positionY<<","<<current_z_<<")"<<std::endl;

          furtherWays_.back().nBoxCheckMatches++;
          furtherWays_.back().addWay(index(positionX, positionY), positionX, positionY, current_z_, value, FurtherWays::Reason::onlyPositionInBox);

          if(!recordFullFurtherWays)
          {
            testFields[index(positionX,positionY)]->setValue(value);
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

void Sudoku::chooseOpenFieldInTestFieldsAndFixToSolution(FieldVector &testFields, int chooseFieldThatHasNPossibilities, bool fixFieldChooseRandomly, bool debug)
{
  int posXtoFix = 0;
  int posYtoFix = 0;
  int index = 0;

  int nTestFields = testFields.size();

  // choose field coordinates (posXtoFix, posYtoFix) that gets fixed afterwards
  if(fixFieldChooseRandomly)
  {
    for(;;)
    {
      // choose a random field and check if it is not already set
      index = rand() % nTestFields;
      if(testFields[index]->value() == 0)
      {
        // store these coordinates
        posXtoFix = testFields[index]->pos_x();
        posYtoFix = testFields[index]->pos_y();

        //std::cout<<"r"<<testFields[index]->n_possible()<<" ";
        break;
      }
    }
  }
  else    // do not choose completely randomly
  {
    //take the first field that has the given number of posibilities
    //for(index = 0; index < testFields.size(); index++)

    //take a random field that has the given number of posibilities
    while(true)
    {
      index = rand() % nTestFields;
      std::shared_ptr<Field> &testField = testFields[index];

      if(testField->value() != 0)
        continue;

      if(testField->n_possible() == chooseFieldThatHasNPossibilities)
      {
        //std::cout<<"a"<<testFields[index]->n_possible()<<" ";
        posXtoFix = testField->pos_x();
        posYtoFix = testField->pos_y();
        break;
      }
    }
  }

  //int correctValue = fields_[index]->value();   // select correct value from solved sudoku
  int correctValue = (*(field_begin_+index))->value();   // select correct value from solved sudoku

  testFields[index]->setValue(correctValue);
  testFields[index]->setFixed(true);

  if(debug)
    std::cout<<"Fix field ("<<posXtoFix<<","<<posYtoFix<<") (index "<<index<<") to value "<<correctValue<<", fixFieldChooseRandomly="<<int(fixFieldChooseRandomly)<<std::endl;
}

void Sudoku::FurtherWays::addWay(int index, int posX, int posY, int posZ, int value, Sudoku::FurtherWays::Reason reason)
{
  Way way;
  way.index = index;
  way.posX = posX;
  way.posY = posY;
  way.posZ = posZ;
  way.value = value;
  way.reason = reason;
  ways.insert(way);
}

int Sudoku::FurtherWays::nMatches()
{
  return ways.size();
}

bool Sudoku::FurtherWays::Way::operator<(const Way &rhs) const
{
  //return posX < rhs.posX || (posX == rhs.posX && (posY < rhs.posY || (posY == rhs.posY && posZ < rhs.posZ)));
  return index < rhs.index;
}

void Sudoku::fixSomeFieldsToGetASolvableSudoku(bool createSolutionHints, FieldVector &testFields, bool quiet, std::string filename)
{
  const bool noDebug = true;
  const bool IWANTTOHAVEDEBUGGINGOUTPUT_OK = false;
  const bool writeToFile = false;

  bool debug = createSolutionHints;
  if(noDebug)
    debug = false;

  if(IWANTTOHAVEDEBUGGINGOUTPUT_OK)
  {
    quiet = false;
    debug = true;
  }

  if(!quiet)
  {
    if(!createSolutionHints)
      std::cout<<"fix some fields to get a solvable sudoku."<<std::endl;
    else
      std::cout<<"collect solution hints."<<std::endl;
  }

  std::stringstream solvingHints;
  solvingHints<<"Möglicher Lösungsweg:"<<std::endl;

  if(debug)
  {
    std::cout<<"Sudoku::fixSomeFieldsToGetASolvableSudoku()"<<std::endl;
    std::cout<<"reference solution: "<<std::endl;
    print();
  }

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();

  // create sums
/*  if(type_ == sum && !createSolutionHints)
  {
    int positionX = rand() % squared_size_;
    int positionY = rand() % squared_size_;

    if(!field_[index(positionX, positionY)]->hasSumBox())
    {
      sumBox.addSummand(field_[index(positionX, positionY)]);
    }
  }*/

  //create fields like of a second sudoku of the same size with classical dependencies and without solution or fixed values
  //this uses field_begin_ and field_end_
  if(testFields.empty())
    createFieldCopy(testFields);

  furtherWays_.clear();

  //if at each stage all possible further ways should be counted and set in furtherWays_. afterwards one random of those ways is selected and executed
  //if this is set to false, the first found further way is considered. This is done by checking in the order field, row, column, box
  const bool recordFullFurtherWays = true;

  //if new fields to become fixed are chosen completely randomly, without considering the number of possibilities in that field
  const bool fixFieldChooseRandomly = false;

  bool fixOccuredInLastIteration = false;
  for(;;)
  {
    if(debug)
    {
      std::cout<<"testFields:"<<std::endl;
      printFields(testFields);

      if(createSolutionHints||1)
      {
        std::cout<<"press enter"<<std::endl;
        std::cin.get();
      }
    }

    if(!fixOccuredInLastIteration)
      furtherWays_.push_back(FurtherWays());

    fixOccuredInLastIteration = false;

    int minPossible = 9;
    int maxPossible = 0;

    //for all open fields find out the maximum and minimum number of possibilities for a field

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

    // if all fields are set, break the loop
    if(nFieldsNotSet == 0)
    {
      break;
    }

    bool restart = false;

    // if an open field has no possibilities, sudoku is not solvable
    if(minPossible == 0)
      std::cout<<"Error: Sudoku seems to be not solvable"<<std::endl;

    //if there are fields that have only one possible value, set this value in test sudoku
    // check if at a field there can only be one value
    restart = fieldCheck(furtherWays_, testFields, recordFullFurtherWays, debug);  // Sudoku::fieldCheck is capable of arbitrary shaped sudokus

    if(restart)
      continue;

    // check if a value can only be at one position in a row
    restart = rowCheck(furtherWays_, testFields, recordFullFurtherWays, debug);   // Sudoku::rowCheck is not capable of arbitrary shaped sudokus

    if(restart)
      continue;

    // check if a value can only be at one position in a column
    restart = columnCheck(furtherWays_, testFields, recordFullFurtherWays, debug);   // Sudoku::columnCheck is not capable of arbitrary shaped sudokus

    if(restart)
      continue;

    // check if a value can only be at one position in a box
    restart = boxCheck(furtherWays_, testFields, recordFullFurtherWays, debug);     // Sudoku::boxCheck is not capable of arbitrary shaped sudokus

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
          <<furtherWays_.back().nBoxCheckMatches<<" box check matches"<<std::endl
          <<furtherWays_.back().nDiagonalCheckMatches<<" diagonal check matches"<<std::endl;
      }

      if(nPossibleFurtherWays > 0)
      {
        //choose a random possible action to do
        int randomIndex = rand() % nPossibleFurtherWays;

        // if solution hints are created, take always the first possibility
        if(createSolutionHints)
          randomIndex = 0;

        std::set<FurtherWays::Way>::iterator wayIter = furtherWays_.back().ways.begin();
        for(int i=0; i<randomIndex; i++)
          wayIter++;

        FurtherWays::Way way = (*wayIter);

        if(debug)
          std::cout<<"set field ("<<way.posX<<","<<way.posY<<","<<way.posZ<<") to value "<<way.value<<std::endl;
        current_z_ = way.posZ;

        // continue output of the solution path
        if(createSolutionHints)
        {
          switch(way.reason)
          {
            case FurtherWays::Reason::onlyPossibilityInField: //field
              solvingHints<<"Betrachte Sudoku "<<(way.posZ+1)<<", Feld ("<<way.posY+1<<","<<way.posX+1<<"). Hier kann nur "<<way.value<<" sein. ";
              break;
            case FurtherWays::Reason::onlyPositionInRow: //row
              solvingHints<<"Betrachte Sudoku "<<(way.posZ+1)<<", Wert "<<way.value<<" in Zeile "<<way.posY+1<<". ";
              break;
            case FurtherWays::Reason::onlyPositionInColumn: //column
              solvingHints<<"Betrachte Sudoku "<<(way.posZ+1)<<", Wert "<<way.value<<" in Spalte "<<way.posX+1<<". ";
              break;
            case FurtherWays::Reason::onlyPositionInDiagonal: //diagonal
              solvingHints<<"Betrachte Sudoku "<<(way.posZ+1)<<", Wert "<<way.value<<" in Diagonale. ";
              break;
            case FurtherWays::Reason::onlyPositionInBox: //box

              solvingHints<<"Betrachte Sudoku "<<(way.posZ+1)<<", Wert "<<way.value<<" im "<<size_<<"x"<<size_<<"-Quadrat";

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

          solvingHints<<"Schreibe "<<way.value<<" in Feld Zeile "<<way.posY+1<<" Spalte "<<way.posX+1<<"."<<std::endl;
        }

        testFields[way.index]->setValue(way.value);
        continue;
      }
    }

    if(debug)
      std::cout<<"No progress, set a new value fixed, minPossible="<<minPossible<<", maxPossible="<<maxPossible<<std::endl;


    //here no field has only one possible value and in no row, column or 3x3-box a value can be localized certainly
    //make a random field with a specific number of possible values fixed (in case !fixFieldChooseRandomly)
    //or choose a completely random field (in case fixFieldChooseRandomly)

    //int chooseFieldThatHasNPossibilities = maxPossible;   // creates an kind of easy sudoku
    int chooseFieldThatHasNPossibilities = minPossible;     // creates the most difficult possible sudoku

    // for some of the last fields choose minPossible for the ones before use maxPossible
    if (nFieldsNotSet > 7)
      chooseFieldThatHasNPossibilities = maxPossible;
    else
      chooseFieldThatHasNPossibilities = minPossible;

    //std::cout<<minPossible<<"-"<<maxPossible<<" ";

    // can be used with arbitrary shaped sudokus
    chooseOpenFieldInTestFieldsAndFixToSolution(testFields, chooseFieldThatHasNPossibilities, fixFieldChooseRandomly, debug);

    fixOccuredInLastIteration = true;
  }

  //copy the "fixed" state from testFields sudoku to this sudoku
  std::vector<std::shared_ptr<Field>>::iterator fieldIter = field_begin_;
  std::vector<std::shared_ptr<Field>>::iterator testFieldIter = testFields.begin();

  for(; fieldIter != field_end_; fieldIter++, testFieldIter++)
  {
    (*fieldIter)->setFixed((*testFieldIter)->fixed());
  }

  if(!quiet)
    std::cout<<"  done, "<<nFixed()<<" fields are fixed. ";
  if(recordFullFurtherWays)
  {
    if(debug)
      std::cout<<"Number of matches in solution path: ";

    double average_matches = 0.0;

    std::stringstream s;
    s<<"results/sudoku_"<<squared_size_<<"x"<<squared_size_<<"_path_";

    if(nPossibilities_.empty())
    {
      nPossibilities_.resize(furtherWays_.size(), 0);
      nRuns_ = 0;
    }

    // count possibilities
    int i=0;
    for(auto &furtherWay : furtherWays_)
    {
      average_matches += furtherWay.nMatches();
      if(nPossibilities_.size() <= i)
      {
      std::cout<<"size: "<<nPossibilities_.size()<<", i="<<i<<", furtherWays_.size(): "<<furtherWays_.size()<<std::endl;
        nPossibilities_.resize(i+1);
      }
      nPossibilities_[i] += furtherWay.nMatches();

      if(debug)
        std::cout<<furtherWay.nMatches()<<" ";

      if(i < 25)
        s<<furtherWay.nMatches()<<"_";
      i++;
    }
    nRuns_++;
    average_matches /= furtherWays_.size();
    if(!quiet)
      std::cout<<std::endl<<"  average number of possibilities: "<<average_matches<<std::endl;

    s<<"fixed_"<<nFixed()<<".txt";


    filename_ = s.str();
    if(writeToFile)
      print(PrintTarget::text_file, filename_);
  }

  std::chrono::time_point<std::chrono::system_clock> tEnd = std::chrono::system_clock::now();
  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);

  if(!quiet)
    std::cout<<"  duration: "<<duration.count()<<" ms, ";

  if(createSolutionHints)
  {
    std::ofstream file("Lösungsweg.txt");
    if(!file.is_open())
      std::cout<<"Could not write to file \"Lösungsweg.txt\"."<<std::endl;
    file<<solvingHints.str();
    file.close();
    solutionHints_ = solvingHints.str();

    if(filename != "")
    {
      std::ofstream file(filename.c_str());
      if(!file.is_open())
        std::cout<<"Could not write to file \""<<filename<<"\"."<<std::endl;
      file<<solvingHints.str();
      file.close();
    }

    if(!quiet)
      std::cout<<"Solution Hints are written to files \"Lösungsweg.txt\" and \""<<filename<<"\".";
  }

  if(!quiet)
    std::cout<<std::endl;
}

void Sudoku::eraseNonFixed()
{
  for(auto &field : fields_)
  {
    if(!field->fixed())
      field->resetTriedValues();
  }
}

void Sudoku::collectSolutionHints(std::string filename)
{
  //std::cout<<"Press Enter . . . "<<std::endl;
  //std::cin.get();
  std::cout<<"collect solution hints"<<std::endl;

  FieldVector testFields;   // testFields is created as copy of fields_ the first time fixSomeFieldsToGetASolvableSudoku is called

  createFieldCopy(testFields);

  const int numberRuns = 1;
  for(int n = 0; n < numberRuns; n++)
  {
    // clear tried values
    for(int i = 0; i < testFields.size(); i++)
    {
      if(!testFields[i]->fixed())
        testFields[i]->resetTriedValues();
    }

    bool createSolutionHints = (n == numberRuns-1);
    fixSomeFieldsToGetASolvableSudoku(createSolutionHints, testFields, true, filename);
  }

  int i=0;
  std::cout<<"average number of possibilities from "<<numberRuns<<" runs: "<<std::endl;
  double average = 0.0;
  for(long long value : nPossibilities_)
  {
    std::cout<<round((double)value/nRuns_)<<" ";
    average += (double)value/nRuns_;
    i++;
  }
  average /= nPossibilities_.size();

  std::cout<<std::endl<<" total average: "<<average<<std::endl
    <<" nfixed: "<<nFixed()<<std::endl;
}

void Sudoku::solveExisting()
{
  std::cout<<"Warning! solveExisting is deprecated, use collectSolutionHints instead!"<<std::endl;


  const bool debug = false;

  std::stringstream solvingHints;
  solvingHints<<"Möglicher Lösungsweg:"<<std::endl;

  if(debug)
  {
    std::cout<<"Sudoku::solveExisting()"<<std::endl;
  }

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();

  furtherWays_.clear();

  const bool recordFullFurtherWays = true;    //if at each stage all possible further ways should be counted and set in furtherWays_ (if not, it is faster)
  const bool fixFieldChooseRandomly = true;     //if new fixed fields should be chosen randomly

  bool lastLoopHadFix = false;
  for(;;)
  {
    if(debug)
      printFields(fields_);

    if(!lastLoopHadFix)
    {
      furtherWays_.push_back(FurtherWays());
    }

    lastLoopHadFix = false;


    int minPossible = 9;
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
              std::cout<<"Field check: field ("<<field->pos()<<") must be "<<field->onePossibleValue()<<""<<std::endl;

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
    restart = rowCheck(furtherWays_, fields_, recordFullFurtherWays, debug);

    if(restart)
      continue;

    restart = columnCheck(furtherWays_, fields_, recordFullFurtherWays, debug);

    if(restart)
      continue;

    restart = boxCheck(furtherWays_, fields_, recordFullFurtherWays, debug);

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
          std::cout<<"set field ("<<way.posX<<","<<way.posY<<","<<way.posZ<<") to value "<<way.value<<std::endl;

        switch(way.reason)
        {
          case 0: //field
            solvingHints<<"Betrachte "<<(way.posZ? "Rauten" : "Kreise")<<", Feld ("<<way.posY+1<<","<<way.posX+1<<"). Hier kann nur "<<way.value<<" sein. ";
            break;
          case 1: //row
            solvingHints<<"Betrachte "<<(way.posZ? "Rauten" : "Kreise")<<", Wert "<<way.value<<" in Zeile "<<way.posY+1<<". ";
            break;
          case 2: //column
            solvingHints<<"Betrachte "<<(way.posZ? "Rauten" : "Kreise")<<", Wert "<<way.value<<" in Spalte "<<way.posX+1<<". ";
            break;
          case 3: //box

            solvingHints<<"Betrachte "<<(way.posZ? "Rauten" : "Kreise")<<", Wert "<<way.value<<" im "<<size_<<"x"<<size_<<"-Quadrat";

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

        solvingHints<<"Schreibe "<<way.value<<" in Feld ("<<way.posY+1<<","<<way.posX+1<<")."<<std::endl;


        current_z_ = way.posZ;

        fields_[index(way.posX,way.posY)]->setValue(way.value);
        continue;
      }
    }

    //here no field has only one possible value and in no row,column or 3x3-box a value can be localized certainly

    if(debug)
      std::cout<<"No progress, set a new value fixed (should not happen here!), maxPossible="<<maxPossible<<std::endl;

    std::cout<<"Sudoku ist either not solvable or needs backtracking to solve (=\"trying\"). This method cannot handle it. Aborting."<<std::endl;
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

      s<<"fixed_"<<nFixed()<<".txt";

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

void Sudoku::createFieldCopy(std::vector<std::shared_ptr<Field>> &copiedFields)
{
  // create new copy of fields
  for(FieldVector::iterator iter = field_begin_; iter != field_end_; iter++)
  {
    Field &field = **iter;
    // create new field with same properties as field
    std::shared_ptr<Field> new_field = std::make_shared<Field>(field.id(), field.pos_x(), field.pos_y(), field.pos_z(),
      field.max_value());

    // copy fixed state
    if(field.fixed())
    {
      new_field->setValue(field.value());
      new_field->setFixed(true);
    }

    copiedFields.push_back(new_field);
  }

  // create copy of dependencies
  FieldVector::iterator iter_new = copiedFields.begin();
  for(FieldVector::iterator iter = field_begin_; iter != field_end_; iter++, iter_new++)
  {
    Field &field = **iter;
    Field &new_field = **iter_new;
    // loop over all dependencies
    for(auto &dependency: field.dependencies())
    {
      int id = dependency->id();

      // find field with that id in copiedFields
      for(FieldVector::iterator iter_copy = copiedFields.begin(); iter_copy != copiedFields.end(); iter_copy++)
      {
        if((*iter_copy)->id() == id)
        {
          new_field.addDependency((*iter_copy));
          break;
        }
      }
    }
  }
}

void Sudoku::print(PrintTarget target, std::string filename, PrintMode mode, int x_offset, int y_offset)
{
  if(filename.empty())
    filename = filename_;

  std::stringstream s;

  s<<"Anzahl vorgegebene Zahlen: "<<nFixed()<<std::endl;
  if(!furtherWays_.empty())
  {
    s<<"Anzahl Möglichkeiten:"<<std::endl;
    for(auto &furtherWay : furtherWays_)
    {
      s<<furtherWay.nMatches()<<"_";
    }
    s<<std::endl;
  }

  if(target == PrintTarget::cout)   //stdio
  {
    std::cout<<s.str();
  }
  else if(target == PrintTarget::text_file)   // file
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<s.str();
    file.close();
  }
  else if(target == PrintTarget::svg_file)    //svg
  {

    // begin svg file
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
    double width = 30*squared_size_;
    double height = 30*squared_size_;
    file<<"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<svg width=\""<<width<<"\" height=\""<<height<<"\">\n<g>\n";
    file.close();
  }

  // print all fields to the requested PrintTarget
  printFields(fields_, target, filename, mode, x_offset, y_offset);

  if(target == PrintTarget::text_file)
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<solutionHints_;
    file.close();
  }
  else if(target == PrintTarget::svg_file)    //svg
  {
    // end svg file
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<"\n</g>\n</svg>\n";
    file.close();
  }
}

void Sudoku::printFields(std::vector<std::shared_ptr<Field>> &fields, PrintTarget target, std::string filename, PrintMode mode, int x_offset, int y_offset)
{
  if(target == PrintTarget::cout || target == PrintTarget::text_file)
  {
    // create string representation for cout and text file
    std::stringstream s;

    // create top row with numbers
    s<<" "<<current_z_;
    for(int x = 0; x < squared_size_; x++)
    {
      if(x % size_ == 0)
        s<<" | ";
      s<<" "<<std::setw(3)<<x;
    }
    s<<"|"<<std::endl;

    // create following rows
    for (int y = 0; y < squared_size_; y++)
    {
      // horizontal bars
      if(y % size_ == 0)
      {
        s<<"--";
        for(int x = 0; x < squared_size_; x++)
        {
          if(x % size_ == 0)
            s<<"-+-";
          s<<"----";
        }
        s<<"+"<<std::endl;
      }

      // left most column with numbers
      s<<std::setw(2)<<y;

      // all fields in that row
      for(int x = 0; x < squared_size_; x++)
      {
        if(x % size_ == 0)
          s<<" | ";

        // if number is fixed
        if(fields[index(x,y)]->fixed())
        {
          if(mode == PrintMode::all_fixed || mode == PrintMode::solved)
          {
            s<<std::setw(3)<<fields[index(x,y)]->value()<<"*";
          }
          else if(mode == PrintMode::max_value)
          {
            s<<std::setw(3)<<fields[index(x,y)]->max_value()<<"*";
          }
          else if(mode == PrintMode::index)
          {
            s<<std::setw(4)<<index(x,y);
          }
          else if(mode == PrintMode::id)
          {
            s<<std::setw(4)<<fields[index(x,y)]->id();
          }
        }
        else    // number is not fixed
        {
          if(mode == PrintMode::all_fixed || mode == PrintMode::solved)
          {
            s<<" "<<std::setw(3)<<fields[index(x,y)]->value();
          }
          else if(mode == PrintMode::max_value)
          {
            s<<" "<<std::setw(3)<<fields[index(x,y)]->max_value();
          }
          else if(mode == PrintMode::index)
          {
            s<<std::setw(4)<<index(x,y);
          }
          else if(mode == PrintMode::id)
          {
            s<<std::setw(4)<<fields[index(x,y)]->id();
          }
        }
        //s<<"["<<fields[index(x,y)]->n_possible()<<"]";

      }
      s<<"|"<<std::endl;
    }

    // bottom line
    s<<"--";
    for(int x = 0; x < squared_size_; x++)
    {
      if(x % size_ == 0)
        s<<"-+-";
      s<<"----";
    }
    s<<"+"<<std::endl<<std::endl;

    // output string representation to target
    if(target == PrintTarget::cout)
    {
      std::cout<<s.str();
    }
    else if(target == PrintTarget::text_file)
    {
      if(filename.empty())
      {
        filename = filename_;
      }

      std::cout<<"Write file \""<<filename<<"\""<<std::endl;
      std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);

      if(!file.is_open())
        std::cout<<"Error writing file \""<<filename<<"\"."<<std::endl;

      file<<s.str();
      file.close();
    }
  }
  else if (target == PrintTarget::svg_file || target == PrintTarget::svg_file_body)
  {
    // create svg output
    if(filename.empty())
    {
      filename = filename_;
    }
    std::cout<<"Write file \""<<filename<<"\""<<std::endl;
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    if(!file.is_open())
      std::cout<<"Error writing file \""<<filename<<"\"."<<std::endl;
    file << "<g>\n\t<g>\n";

    int size = 30;    // size of one square
    //int x_offset = 0;
    //int y_offset = 0;

    x_offset *= size;
    y_offset *= size;
    int xpos_text = x_offset + squared_size_*size*0.4;
    int ypos_text = y_offset - 5;

    // diagonals
    if(type_ == SudokuType::diagonal)
    {
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:#aaaaaa;stroke-width:1;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:1,3;stroke-opacity:1\"\n"
         <<"\td=\"m "<<x_offset<<","<<y_offset<<" "<<size*squared_size_<<","<<size*squared_size_<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:#aaaaaa;stroke-width:1;stroke-linecap:round;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:1,3;stroke-opacity:1\"\n"
         <<"\td=\"m "<<x_offset<<","<<y_offset+size*squared_size_<<" "<<size*squared_size_<<","<<-size*squared_size_<<"\" />\n";
    }


    file << "\t</g>\n\t<g>\n";
    // numbers and squares
    for (int y = 0; y < squared_size_; y++)
    {
      for(int x = 0; x < squared_size_; x++)
      {
        file << " <rect\n"
           <<"\ty=\""<<y_offset+y*size<<"\"\n"
           <<"\tx=\""<<x_offset+x*size<<"\"\n"
           <<"\theight=\""<<size<<"\"\n"
           <<"\twidth=\""<<size<<"\"\n"
           <<"\tstyle=\"opacity:1;fill:none;fill-opacity:1;stroke:#808080;stroke-width:0.5;stroke-linecap:square;stroke-linejoin:miter;stroke-miterlimit:4;stroke-dasharray:none;stroke-dashoffset:0;stroke-opacity:1\" />\n";

        if(!(mode == PrintMode::all_fixed && !fields[index(x,y)]->fixed()))
        {
          double xpos = (fields[index(x,y)]->value() >= 10? x_offset+2 : x_offset+8) + (x*size);

         file << "<text\n"
           <<"\txml:space=\"preserve\"\n"
           <<"\tstyle=\"font-style:normal;font-weight:normal;font-size:20px;line-height:125%;font-family:sans-serif;text-align:start;letter-spacing:-2px;word-spacing:0px;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1\"\n"
           <<"\tx=\""<<xpos<<"\"\n"
           <<"\ty=\""<<y_offset-8+(y+1)*size<<"\"\n"
           <<"\tlinespacing=\"100%\"><tspan\n"
             <<"\t\trole=\"line\"\n"
             <<"\t\tx=\""<<xpos<<"\"\n"
             <<"\t\ty=\""<<y_offset-8+(y+1)*size<<"\">";

          if(mode == PrintMode::all_fixed || mode == PrintMode::solved)
          {
            file<<fields[index(x,y)]->value();
          }
          else if(mode == PrintMode::max_value)
          {
            file<<fields[index(x,y)]->max_value();
          }
          else if(mode == PrintMode::index)
          {
            file<<index(x,y);
          }
          else if(mode == PrintMode::id)
          {
            file<<fields[index(x,y)]->id();
          }

          file<<"</tspan></text>\n\n";
        }
      }
    }

    file << "\t</g>\n\t<g>\n";

    //thicker lines
    std::string stroke = "#000000";
    if(current_z_%2 == 1)
      stroke = "#003399";

    if(type_ == SudokuType::freeform)
    {
      // border horizontal
      for (int y = 0; y <= squared_size_; y+=squared_size_)
      {
        file << "<path\n"
           <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
           <<"\td=\"m "<<x_offset<<","<<y_offset+y*size<<" "<<size*squared_size_<<",0\" />\n";
      }

      // border vertical
      for (int x = 0; x <= squared_size_; x+=squared_size_)
      {
        file << "<path\n"
           <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
           <<"\td=\"m "<<x_offset+x*size<<","<<y_offset<<" 0,"<<size*squared_size_<<"\" />\n";
      }

      // center box
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+size_*size<<","<<y_offset+size_*size<<" "<<size_*size<<",0\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+size_*size<<","<<y_offset+2*size_*size<<" "<<size_*size<<",0\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+size_*size<<","<<y_offset+size_*size<<" 0,"<<size_*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size_*size<<","<<y_offset+size_*size<<" 0,"<<size_*size<<"\" />\n";

      // other lines
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+0*size<<","<<y_offset+2*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+1*size<<","<<y_offset+2*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+1*size<<","<<y_offset+3*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+3*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+4*size<<" "<<1*size<<","<<0*size<<"\" />\n";

      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+0*size<<","<<y_offset+7*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+1*size<<","<<y_offset+7*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+1*size<<","<<y_offset+6*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+6*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+5*size<<" "<<1*size<<","<<0*size<<"\" />\n";

      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+0*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+1*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+3*size<<","<<y_offset+1*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+3*size<<","<<y_offset+2*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+4*size<<","<<y_offset+2*size<<" "<<0*size<<","<<1*size<<"\" />\n";

      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+5*size<<","<<y_offset+3*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+5*size<<","<<y_offset+2*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+6*size<<","<<y_offset+2*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+6*size<<","<<y_offset+1*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+7*size<<","<<y_offset+1*size<<" "<<0*size<<","<<-1*size<<"\" />\n";


      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+9*size<<","<<y_offset+2*size<<" "<<-1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+8*size<<","<<y_offset+2*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+8*size<<","<<y_offset+3*size<<" "<<-1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+7*size<<","<<y_offset+3*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+7*size<<","<<y_offset+4*size<<" "<<-1*size<<","<<0*size<<"\" />\n";

      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+9*size<<","<<y_offset+7*size<<" "<<-1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+8*size<<","<<y_offset+7*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+8*size<<","<<y_offset+6*size<<" "<<-1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+7*size<<","<<y_offset+6*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+7*size<<","<<y_offset+5*size<<" "<<-1*size<<","<<0*size<<"\" />\n";


      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+9*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+2*size<<","<<y_offset+8*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+3*size<<","<<y_offset+8*size<<" "<<0*size<<","<<-1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+3*size<<","<<y_offset+7*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+4*size<<","<<y_offset+7*size<<" "<<0*size<<","<<-1*size<<"\" />\n";

      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+5*size<<","<<y_offset+6*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+5*size<<","<<y_offset+7*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+6*size<<","<<y_offset+7*size<<" "<<0*size<<","<<1*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+6*size<<","<<y_offset+8*size<<" "<<1*size<<","<<0*size<<"\" />\n";
      file << "<path\n"
         <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
         <<"\td=\"m "<<x_offset+7*size<<","<<y_offset+8*size<<" "<<0*size<<","<<1*size<<"\" />\n";

    }
    else
    {
      //horizontal
      for (int y = 0; y <= squared_size_; y+=size_)
      {
        file << "<path\n"
           <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
           <<"\td=\"m "<<x_offset<<","<<y_offset+y*size<<" "<<size*squared_size_<<",0\" />\n";
      }

      // vertical
      for (int x = 0; x <= squared_size_; x+=size_)
      {
        file << "<path\n"
           <<"\tstyle=\"fill:none;fill-rule:evenodd;stroke:"<<stroke<<";stroke-width:2;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1;stroke-miterlimit:4;stroke-dasharray:none\"\n"
           <<"\td=\"m "<<x_offset+x*size<<","<<y_offset<<" 0,"<<size*squared_size_<<"\" />\n";
      }
    }

    // number of sudoku
   file << "<text\n"
     <<"\txml:space=\"preserve\"\n"
     <<"\tstyle=\"font-style:normal;font-weight:normal;font-size:10px;line-height:125%;font-family:sans-serif;text-align:start;letter-spacing:0px;word-spacing:0px;writing-mode:lr-tb;text-anchor:start;fill:#000000;fill-opacity:1;stroke:none;stroke-width:1px;stroke-linecap:round;stroke-linejoin:miter;stroke-opacity:1\"\n"
     <<"\tx=\""<<xpos_text<<"\"\n"
     <<"\ty=\""<<ypos_text<<"\"\n"
     <<"\tlinespacing=\"100%\"><tspan\n"
       <<"\t\trole=\"line\"\n"
       <<"\t\tx=\""<<xpos_text<<"\"\n"
       <<"\t\ty=\""<<ypos_text<<"\">"
       <<"Sudoku "<<(current_z_+1)
       <<"</tspan></text>\n\n";

    file << "\t</g>\n</g>\n";

    file.close();
  }
}
