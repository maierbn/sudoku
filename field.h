#pragma once

#include <vector>
#include <memory>
#include <set>

class Field
{
public:
  Field(int pos_x, int pos_y, int pos_z, int max_value);
  Field(int pos_x, int pos_y, int pos_z, int max_value, std::vector<std::shared_ptr<Field>> dependencies);
    
  void setValue(int value);
  void addDependency(std::shared_ptr<Field> dependency);
  void addDependencies(std::vector<std::shared_ptr<Field>> dependencies);
  void setFixed(bool fixed);  
  void storePossible();
  void setPosZ(int pos_z);
  
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
  std::vector<std::shared_ptr<Field>> dependencies();
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
  std::vector<std::shared_ptr<Field>> dependencies_;
  std::vector<bool> valuesPossible_;
  int n_possible_;
    
  int pos_x_;
  int pos_y_;
  int pos_z_;
  bool fixed_;
  int max_value_; //9
};