#pragma once

#include <vector>
#include <memory>
#include <set>

#include "compare_fields.h"

class Field
{
public:
  Field(int id, int pos_x, int pos_y, int pos_z, int max_value);
  Field(int id, int pos_x, int pos_y, int pos_z, int max_value, std::vector<std::shared_ptr<Field>> dependencies);

  void setValue(int value);
  void addDependency(std::shared_ptr<Field> dependency);
  void addDependencies(std::set<std::shared_ptr<Field>, CompareFields> dependencies);
  void setFixed(bool fixed);
  void storePossible();
  void setPosZ(int pos_z);
  void setId(int id);

  ///try new value, return false if no more values are possible
  bool setNewValue();

  void resetTriedValues();

  //getters
  std::string pos();
  int pos_x();
  int pos_y();
  int pos_z();
  bool fixed();
  int value();
  int max_value();
  int id();
  std::set<std::shared_ptr<Field>, CompareFields> dependencies();
  bool valuePossible(int i);

  //returns the value if there is only one possibility left
  int onePossibleValue();

  //if value is set (!=0), n_possible is 0
  int n_possible();

private:

  bool alreadyTried(int i);
  bool isPossibleFromDependencies(int i);

  int value_;			//0 means not set
  std::set<int> triedValues_;
  std::set<std::shared_ptr<Field>, CompareFields> dependencies_;
  std::vector<bool> valuesPossible_;
  int n_possible_;

  int pos_x_;
  int pos_y_;
  int pos_z_;
  int id_;
  bool fixed_;
  int max_value_; //9
};
