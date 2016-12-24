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

Sudoku::Sudoku(int size) : size_(size), squared_size_(size*size), current_z_(0), filename_("out.txt")
{
  fields_.resize(squared_size_*squared_size_);

  srand(std::time(NULL));

  //create fields
  for(int x = 0; x < squared_size_; x++)
  {
    for (int y = 0; y < squared_size_; y++)
    {
      fields_[index(x,y)] = std::move(std::unique_ptr<Field>(new Field(x,y,current_z_,squared_size_)));
    }
  }
  /*
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(fields_.begin(), fields_.end(), std::default_random_engine(seed));
  */
  addDependencies();
}

void Sudoku::addDependencies()
{
  addDependenciesFields(fields_);
}

void Sudoku::addDependenciesFields(std::vector<std::shared_ptr<Field>> &fields)
{
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

          //add dependencies to all of 3x3 field
          int dependency_x = block3x3_no_x*size_ + field3x3_x;
          int dependency_y = block3x3_no_y*size_ + field3x3_y;

          if(dependency_x != x || dependency_y != y)
            fields[index(x,y)]->addDependency(fields_[index(dependency_x,dependency_y)]);

          //add dependencies to all of row
          dependency_x = field3x3_x*size_ + field3x3_y;   // this is actually an own loop over x=0..squared_size
          dependency_y = y;

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
}

void Sudoku::permuteNumbers()
{
  //generate random permutation
  srand(std::time(NULL));
  std::vector<int> permutation(squared_size_);
  for(int i=0; i<squared_size_; i++)
  {
    permutation[i] = i+1;
  }

  for(int i=0; i<squared_size_; i++)
  {
    int index = rand() % (squared_size_-i) + i;
    std::swap(permutation[i], permutation[index]);
  }

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

void Sudoku::incrementFieldIterator(std::vector<std::shared_ptr<Field>>::iterator &nextField)
{
  do
  {
    nextField++;
    if(nextField == fields_.end())
      break;
  }
  while((*nextField)->fixed());

}

void Sudoku::decrementFieldIterator(std::vector<std::shared_ptr<Field>>::iterator &nextField)
{
  do
  {
    nextField--;
    if(nextField == fields_.begin())
      break;
  }
  while((*nextField)->fixed());
}

int Sudoku::n_fixed()
{
  int nFixed = 0;
  for(auto &field : fields_)
  {
    nFixed += field->fixed();
  }
  return nFixed;
}

void Sudoku::createSolution()
{
  const bool debug = false;

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();


  //std::random_shuffle(fields_.begin(), fields_.end());

  std::vector<std::shared_ptr<Field>>::iterator nextField = fields_.begin();

  std::stack<std::shared_ptr<Field>> trace;
  trace.push(*nextField);
  incrementFieldIterator(nextField);

  if(debug)
  {
    print();
    std::cout<<"current top field: "<<trace.top()->pos()<<" (next Field: "<<(*nextField)->pos()<<")"<<std::endl;
  }

  do
  {
    if(trace.top()->setNewValue())
    {
      if(debug)std::cout<<"success"<<std::endl;

      if(nextField == fields_.end())
        break;
      else
        trace.push(*nextField);
      incrementFieldIterator(nextField);
    }
    else
    {
      if(debug)std::cout<<"no success, tracking back"<<std::endl;
      trace.top()->resetTriedValues();
      trace.pop();
      decrementFieldIterator(nextField);
    }

    if(debug)
    {
      current_z_ = trace.top()->pos_z();		//only for output
      print();
      std::cout<<"current top field: "<<trace.top()->pos();
      if(nextField != fields_.end())
        std::cout<<" (next Field: "<<(*nextField)->pos()<<")"<<std::endl;
      else
        std::cout<<std::endl;
      std::cout<<std::string(80,'-')<<std::endl;
    }
  }
  while(!trace.empty());
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
  std::cout<<"Duration: "<<duration.count()<<" ms"<<std::endl;

  if(trace.empty())
    std::cout<<"Sudoku has no solution!"<<std::endl;
  else
    std::cout<<"Sudoku createSolutiond, trace size: "<<trace.size()<<std::endl;
}

bool Sudoku::rowCheck(std::list<FurtherWays> &furtherWays_, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug)
{
  bool restart = false;

    //check rows, if there are values that can only be at one position
    restart = false;
    for (int y = 0; y < squared_size_; y++)
    {

      for (int value = 1; value <= squared_size_; value++)
      {
	bool valueIsAlreadySet = false;
	int nPossiblePositionsForValue = 0;
	int positionX = 0;
	for (int x = 0; x < squared_size_; x++)
	{
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

	  furtherWays_.back().nRowCheckMatches++;
	  furtherWays_.back().addWay(positionX, y, current_z_, value, 1);

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
	furtherWays_.back().addWay(x, positionY, current_z_, value, 2);

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
	  furtherWays_.back().addWay(positionX, positionY, current_z_, value, 3);

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

void Sudoku::fixField(std::vector<std::shared_ptr<Field>> &testFields, int maxPossible, bool fixFieldChooseRandomly, bool debug)
{
  int posXtoFix = 0;
  int posYtoFix = 0;

  if(fixFieldChooseRandomly)
  {
    for(;;)
    {
      int index = rand() % (squared_size_*squared_size_);
      if(testFields[index]->value() == 0)
      {
	posXtoFix = testFields[index]->pos_x();
	posYtoFix = testFields[index]->pos_y();
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
	break;
      }
    }
  }

  int correctValue = fields_[index(posXtoFix, posYtoFix)]->value();

  testFields[index(posXtoFix, posYtoFix)]->setValue(correctValue);
  testFields[index(posXtoFix, posYtoFix)]->setFixed(true);

  if(debug)
    std::cout<<"Fix field ("<<posXtoFix<<","<<posYtoFix<<") to value "<<correctValue<<std::endl;
}

void Sudoku::FurtherWays::addWay(int posX, int posY, int posZ, int value, int reason)
{
  Way way;
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
  return posX < rhs.posX || (posX == rhs.posX && (posY < rhs.posY || (posY == rhs.posY && posZ < rhs.posZ)));
}

void Sudoku::setFixed()
{
  const bool debug = false;
  const bool writeToFile = false;

  if(debug)
  {
    std::cout<<"Sudoku::setFixed()"<<std::endl;
    std::cout<<"reference solution: "<<std::endl;
    print();
  }

  std::chrono::time_point<std::chrono::system_clock> tBegin = std::chrono::system_clock::now();

  //create second sudoku with same dependencies but without values
  Sudoku test(size_);

  std::vector<std::shared_ptr<Field>> &testFields = test.fields_;


  furtherWays_.clear();

  const bool recordFullFurtherWays = true;    //if at each stage all possible further ways should be counted and set in furtherWays_ (if not, it is faster)
  const bool fixFieldChooseRandomly = true;     //if new fixed fields should be chosen randomly

  bool lastLoopHadFix = false;
  for(;;)
  {
    if(debug)
      printFields(testFields);

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
	      std::cout<<"Field check: field ("<<field->pos()<<") must be "<<field->onePossibleValue()<<""<<std::endl;

	    furtherWays_.back().nFieldCheckMatches++;
	    furtherWays_.back().addWay(field->pos_x(), field->pos_y(), field->pos_z(), field->onePossibleValue());

	    if(!recordFullFurtherWays)
	      field->setValue(field->onePossibleValue());
	  }
	}
      }
      if(!recordFullFurtherWays)
	continue;
    }
    restart = rowCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

    if(restart)
      continue;

    restart = columnCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

    if(restart)
      continue;

    restart = boxCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

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
	current_z_ = way.posZ;

	testFields[index(way.posX,way.posY)]->setValue(way.value);
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
      print(1, filename_);
  }

  std::chrono::time_point<std::chrono::system_clock> tEnd = std::chrono::system_clock::now();
  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);
  std::cout<<"Duration: "<<duration.count()<<" ms"<<std::endl;
}


void Sudoku::eraseNonFixed()
{
  for(auto &field : fields_)
  {
    if(!field->fixed())
      field->resetTriedValues();
  }
}

void Sudoku::solveExisting()
{
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
	    furtherWays_.back().addWay(field->pos_x(), field->pos_y(), field->pos_z(), field->onePossibleValue());

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

      s<<"fixed_"<<n_fixed()<<".txt";

      std::string filename = s.str();
      print(1, filename);
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

void Sudoku::print(int target, std::string filename)
{
  if(filename.empty())
    filename = filename_;

  std::stringstream s;

  s<<"Anzahl vorgegebene Zahlen: "<<n_fixed()<<std::endl;
  if(!furtherWays_.empty())
  {
    s<<"Anzahl Möglichkeiten:"<<std::endl;
    for(auto &furtherWay : furtherWays_)
    {
      s<<furtherWay.nMatches()<<"_";
    }
    s<<std::endl;
  }

  if(target == 0)
    std::cout<<s.str();
  else
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<s.str();
    file.close();
  }

  printFields(fields_, target, filename);

  if(target != 0)
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<solutionHints_;
    file.close();
  }
}

void Sudoku::printFields(std::vector<std::shared_ptr<Field>> &fields, int target, std::string filename)
{
  std::stringstream s;

  s<<" "<<current_z_;
  for(int x = 0; x < squared_size_; x++)
  {
    if(x % size_ == 0)
      s<<" | ";
    s<<" "<<std::setw(2)<<x;
  }
  s<<"|"<<std::endl;
  for (int y = 0; y < squared_size_; y++)
  {
    if(y % size_ == 0)
    {
      s<<"--";
      for(int x = 0; x < squared_size_; x++)
      {
        if(x % size_ == 0)
          s<<"-+-";
        s<<"---";
      }
      s<<"+"<<std::endl;
    }

    s<<std::setw(2)<<y;
    for(int x = 0; x < squared_size_; x++)
    {
      if(x % size_ == 0)
        s<<" | ";
      if(fields[index(x,y)]->fixed())
        s<<std::setw(2)<<fields[index(x,y)]->value()<<"*";
      else
        s<<" "<<std::setw(2)<<fields[index(x,y)]->value();
      //s<<"["<<fields[index(x,y)]->n_possible()<<"]";

    }
    s<<"|"<<std::endl;
  }

    s<<"--";
  for(int x = 0; x < squared_size_; x++)
  {
    if(x % size_ == 0)
      s<<"-+-";
    s<<"---";
  }
  s<<"+"<<std::endl<<std::endl;

  if(target == 0)
    std::cout<<s.str();
  else
  {
    if(filename.empty())
      filename = filename_;
    std::cout<<"Write file \""<<filename<<"\""<<std::endl;
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    if(!file.is_open())
      std::cout<<"Error writing file \""<<filename<<"\"."<<std::endl;
    file<<s.str();
    file.close();
  }
}
