#pragma once

#include <vector>
#include <memory>
#include <list>

#include "field.h"

class Sudoku
{
public:
  Sudoku(int size);

  virtual void permuteNumbers();

  //target=0: cout, 1: text-file, 2: svg-file, mode=0: all/fixed (for target=2 only fixed), 1: max_value, 2: index, 3: solved
  virtual void print(int target=0, std::string filename="", int mode=0);
  virtual void setFixed();
  virtual void solveExisting();
  virtual void eraseNonFixed();
  virtual void createSolution();
  int n_fixed();
  void setOneFixed(int pos_x, int pos_y, bool fixed);

  void set(int pos_x, int pos_y, int value, bool fixed=false);


protected:

  struct FurtherWays
  {
    int nRowCheckMatches = 0;
    int nColumnCheckMatches = 0;
    int nBoxCheckMatches = 0;
    int nFieldCheckMatches = 0;

    // one possible thing to set next
    struct Way
    {
      int id;
      int posX;
      int posY;
      int posZ;
      int value;
      int reason;

      bool operator<(const Way &rhs) const;
    };
    std::set<Way> ways;		// all possible things to do at this stage

    void addWay(int id, int posX, int posY, int posZ, int value, int reason=-1);  //reason: 0: field, 1: row, 2:column, 3:box
    int nMatches();
  };

  virtual int index(int x, int y);
  virtual void addDependencies();
  virtual void addDependenciesFields(std::vector<std::shared_ptr<Field>> &fields);
  virtual void printFields(std::vector< std::shared_ptr< Field > >& fields, int target=0, std::string filename="", int mode=0);

  virtual bool rowCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  virtual bool columnCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  virtual bool boxCheck(std::list<FurtherWays> &furtherWays, std::vector<std::shared_ptr<Field>> &testFields, bool recordFullFurtherWays, bool debug);
  virtual void fixField(std::vector<std::shared_ptr<Field>> &testFields, int maxPossible, bool fixFieldChooseRandomly, bool debug);

  void incrementFieldIterator(std::vector<std::shared_ptr<Field>>::iterator &nextField);
  void decrementFieldIterator(std::vector<std::shared_ptr<Field>>::iterator &nextField);

  std::vector<std::shared_ptr<Field>> fields_;
  std::list<FurtherWays> furtherWays_;

  int size_;  //3
  int squared_size_; //9
  int current_z_;
  std::string filename_;
  std::string solutionHints_;
};
