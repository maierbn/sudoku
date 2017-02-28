#include "interleaved_sudoku.h"

#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <set>
#include <vector>
#include <chrono>


InterleavedSudoku::InterleavedSudoku(int size) : Sudoku(size, Sudoku::SudokuType::classical)
{
  fields_.resize(squared_size_*squared_size_*2);

  //create fields
  for(int x = 0; x < squared_size_; x++)
  {
    for (int y = 0; y < squared_size_; y++)
    {
      fields_[index(x,y,0)] = std::move(std::unique_ptr<Field>(new Field(index(x,y,0),x,y,0,squared_size_)));
      fields_[index(x,y,1)] = std::move(std::unique_ptr<Field>(new Field(index(x,y,0),x,y,1,squared_size_)));
    }
  }

  addDependencies();
}


void InterleavedSudoku::addDependencies()
{
  current_z_ = 0;
  Sudoku::addDependencies();
  current_z_ = 1;
  Sudoku::addDependencies();

  //interleaved dependencies
  for(int x = 0; x < squared_size_; x++)
  {
    for (int y = 0; y < squared_size_; y++)
    {
      // 0 to 1
      fields_[index(x,y,0)]->addDependency(fields_[index(x,y,1)]);    //bottom right

      if(x > 0)
      {
        fields_[index(x,y,0)]->addDependency(fields_[index(x-1,y,1)]);  //bottom left
      }
      if(y > 0)
      {
        fields_[index(x,y,0)]->addDependency(fields_[index(x,y-1,1)]);  //top right
      }
      if(y > 0 && x > 0)
      {
        fields_[index(x,y,0)]->addDependency(fields_[index(x-1,y-1,1)]);  //top left
      }

      // 1 to 0
      fields_[index(x,y,1)]->addDependency(fields_[index(x,y,0)]);          //top left

      if(x < squared_size_-1)
      {
        fields_[index(x,y,1)]->addDependency(fields_[index(x+1,y,0)]);    //top right
      }

      if(y < squared_size_-1)
      {
        fields_[index(x,y,1)]->addDependency(fields_[index(x,y+1,0)]);          //bottom left
      }

      if(x < squared_size_-1 && y < squared_size_-1)
      {
        fields_[index(x,y,1)]->addDependency(fields_[index(x+1,y+1,0)]);          //bottom right
      }
    }
  }
}

void InterleavedSudoku::permuteNumbers()
{
  //generate random permutation
  //srand(std::time(NULL));
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
      if(fields_[index(x,y,0)]->value() > 0)
        fields_[index(x,y,0)]->setValue(permutation[fields_[index(x,y,0)]->value()-1]);
      if(fields_[index(x,y,1)]->value() > 0)
        fields_[index(x,y,1)]->setValue(permutation[fields_[index(x,y,1)]->value()-1]);
    }
  }
}

int InterleavedSudoku::index(int x, int y, int z)
{
  return z*squared_size_*squared_size_ + y*squared_size_ + x;
}

int InterleavedSudoku::index(int x, int y)
{
  return current_z_*squared_size_*squared_size_ + y*squared_size_ + x;
}

void InterleavedSudoku::createSolution()
{
  current_z_ = 0;
  Sudoku::createSolution();
}

void InterleavedSudoku::solveExisting()
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
	    furtherWays_.back().addWay(field->pos_x(), field->pos_y(), field->pos_z(), field->onePossibleValue(), 0);

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

	fields_[index(way.posX,way.posY,way.posZ)]->setValue(way.value);
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
      s<<"results/sudoku_interleaved_"<<squared_size_<<"x"<<squared_size_<<"_path_";

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

void InterleavedSudoku::fixSomeFieldsToGetASolvableSudoku()
{
  //Sudoku::setFixed();
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
  InterleavedSudoku test(size_);

  std::vector<std::shared_ptr<Field>> &testFields = test.fields_;

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
            furtherWays_.back().addWay(field->id(), field->pos_x(), field->pos_y(), field->pos_z(), field->onePossibleValue());

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

	testFields[index(way.posX,way.posY,way.posZ)]->setValue(way.value);
	continue;
      }
    }

    if(debug)
      std::cout<<"No progress, set a new value fixed, maxPossible="<<maxPossible<<std::endl;


    //here no field has only one possible value and in no row,column or 3x3-box a value can be localized certainly
    //make a value with the most possible values available

    chooseOpenFieldInTestFieldsAndFixToSolution(testFields, maxPossible, fixFieldChooseRandomly, debug);
    lastLoopHadFix = true;
  }

  //set fixed in current Sudoku
  std::vector<std::shared_ptr<Field>>::iterator fieldIter = fields_.begin();
  std::vector<std::shared_ptr<Field>>::iterator testFieldIter = testFields.begin();

  for(; fieldIter != fields_.end(); fieldIter++, testFieldIter++)
  {
    (*fieldIter)->setFixed((*testFieldIter)->fixed());
  }

  std::cout<<nFixed()<<" Fields are fixed"<<std::endl;
  if(recordFullFurtherWays)
  {
    if(debug)
      std::cout<<"Number of matches in solution path: ";
    std::stringstream s;
    s<<"results/sudoku_interleaved_"<<squared_size_<<"x"<<squared_size_<<"_path_";

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

    s<<"fixed_"<<nFixed()<<".txt";

    filename_ = s.str();
    if(writeToFile)
      print(PrintTarget::text_file, filename_);
  }

  std::chrono::time_point<std::chrono::system_clock> tEnd = std::chrono::system_clock::now();
  std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(tEnd - tBegin);
  std::cout<<"Duration: "<<duration.count()<<" ms"<<std::endl;



  if(debug)
  {
    std::cout<<"InterleavedSudoku::setFixed() done, result:"<<std::endl;
    print();
  }
}

void InterleavedSudoku::eraseNonFixed()
{
  for(auto &field : fields_)
  {
    if(!field->fixed())
      field->resetTriedValues();
  }
}


void InterleavedSudoku::chooseOpenFieldInTestFieldsAndFixToSolution(std::vector< std::shared_ptr< Field > >& testFields, int maxPossible, bool fixFieldChooseRandomly, bool debug)
{
  int posXtoFix = 0;
  int posYtoFix = 0;
  int posZtoFix = 0;

  if(fixFieldChooseRandomly)
  {
    for(;;)
    {
      int x = rand() % (squared_size_);
      int y = rand() % (squared_size_);
      int z = rand() % 2;
      if(testFields[index(x,y,z)]->value() == 0)
      {
	posXtoFix = x;
	posYtoFix = y;
	posZtoFix = z;
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
	break;
      }
    }
  }

  int correctValue = fields_[index(posXtoFix, posYtoFix, posZtoFix)]->value();

  testFields[index(posXtoFix, posYtoFix, posZtoFix)]->setValue(correctValue);
  testFields[index(posXtoFix, posYtoFix, posZtoFix)]->setFixed(true);

  if(debug)
    std::cout<<"Fix field ("<<posXtoFix<<","<<posYtoFix<<","<<posZtoFix<<") to value "<<correctValue<<std::endl;
}

bool InterleavedSudoku::rowCheck(std::list< Sudoku::FurtherWays >& furtherWays_, std::vector< std::shared_ptr< Field > >& testFields, bool recordFullFurtherWays, bool debug)
{
  current_z_ = 0;
  bool restart0 = Sudoku::rowCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

  current_z_ = 1;
  bool restart1 = Sudoku::rowCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

  return restart0 || restart1;
}

bool InterleavedSudoku::columnCheck(std::list< Sudoku::FurtherWays >& furtherWays_, std::vector< std::shared_ptr< Field > >& testFields, bool recordFullFurtherWays, bool debug)
{
  current_z_ = 0;
  bool restart0 = Sudoku::columnCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

  current_z_ = 1;
  bool restart1 = Sudoku::columnCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

  return restart0 || restart1;
}

bool InterleavedSudoku::boxCheck(std::list< Sudoku::FurtherWays >& furtherWays_, std::vector< std::shared_ptr< Field > >& testFields, bool recordFullFurtherWays, bool debug)
{
  current_z_ = 0;
  bool restart0 = Sudoku::boxCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

  current_z_ = 1;
  bool restart1 = Sudoku::boxCheck(furtherWays_, testFields, recordFullFurtherWays, debug);

  return restart0 || restart1;
}


void InterleavedSudoku::print(PrintTarget target, std::string filename)
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

  if(target == PrintTarget::cout)
  {
    std::cout<<s.str();
  }
  else
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<s.str();
    file.close();
  }

  printFields(fields_, target, filename);

  if(target != PrintTarget::cout)
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<solutionHints_;
    file.close();
  }
}

void InterleavedSudoku::printFields(std::vector< std::shared_ptr< Field > >& fields, PrintTarget target, std::string filename)
{
  if(filename.empty())
    filename = filename_;

  if(target == PrintTarget::cout)
    std::cout<<"A"<<std::endl;
  else
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::trunc);
    file<<"A"<<std::endl;
    file.close();
  }

  current_z_ = 0;
  Sudoku::printFields(fields, target, filename);

  if(target == PrintTarget::cout)
    std::cout<<"B"<<std::endl;
  else
  {
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
    file<<"B"<<std::endl;
    file.close();
  }
  current_z_ = 1;
  Sudoku::printFields(fields, target, filename);

}

void InterleavedSudoku::set(int posX, int posY, int posZ, int value)
{
  fields_[index(posX, posY, posZ)]->setValue(value);
  fields_[index(posX, posY, posZ)]->setFixed(true);
}
