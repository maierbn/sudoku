#include "field.h"

#include <sstream>
#include <iostream>
#include <algorithm>

Field::Field(int id, int pos_x, int pos_y, int pos_z, int max_value) :
  value_(0), valuesPossible_(max_value, true), n_possible_(max_value), pos_x_(pos_x), pos_y_(pos_y), pos_z_(pos_z), id_(id),
  fixed_(false), max_value_(max_value)
{

}

Field::Field(int id, int pos_x, int pos_y, int pos_z, int max_value, std::vector<std::shared_ptr<Field>> dependencies) :
  Field(id, pos_x, pos_y, pos_z, max_value)
{
  dependencies_.insert(dependencies.begin(), dependencies.end());
}

Field::Field(int id, int max_value) :
  Field(id, -1, -1, -1, max_value)
{
}

void Field::setValue(int value)
{
  value_ = value;
}

void Field::setFixed(bool fixed)
{
  fixed_ = fixed;
}

void Field::setMaxValue(int maxValue)
{
  max_value_ = maxValue;
}

bool Field::fixed()
{
  return fixed_;
}

int Field::value()
{
  return value_;
}

void Field::addDependency(std::shared_ptr<Field> dependency)
{
  dependencies_.insert(dependency);
}

void Field::addGroupDependency(std::set<std::shared_ptr<Field>, CompareFields> groupDependencies)
{
  groupDependencies_.push_back(groupDependencies);
}

void Field::addSumBox(std::shared_ptr<SumBox> sumBox)
{
  sumBox_ = sumBox;
}

bool Field::alreadyTried(int i)
{
  return triedValues_.find(i) != triedValues_.end();
}

void Field::getPossibleValuesToTry(std::set<int> &valuesPossible)
{
  for (int j = 1; j <= max_value_; j++)
  {
    if (valuePossible(j))
    {
      valuesPossible.insert(j);
    }
  }
}

bool Field::isPossibleFromDependencies(int i, bool includeGroupDependencies=true)
{
  const bool debug = false;
  //std::cout << pos_x_ << "," << pos_y_ << " isPossibleFromDependencies(" << i << "), ";

  // check dependencies
  for(auto &dependency : dependencies_)
  {
    if(dependency->value() == i)
      return false;
  }

  
  if (!includeGroupDependencies)
    return true;

  // check group dependencies
  for(auto &groupDependency : groupDependencies_)
  {
    // collect possible values in group
    std::set<int> valuesPossible;
    for(auto &groupField : groupDependency)
    {
      groupField->getPossibleValuesToTry(valuesPossible);
    }    

    if (debug)
    {
      std::cout << pos_x_ << "," << pos_y_ << " isPossibleFromDependencies(" << i << "), ";

      std::cout << "valuesPossible: { ";
      for (const auto& value : valuesPossible) {
          std::cout << value << " ";
      }
      std::cout << "}: possible=" << std::boolalpha << !(valuesPossible.find(i) == valuesPossible.end()) << std::endl;
    }

    // If `i` is not among the possible values for this group
    if (valuesPossible.find(i) == valuesPossible.end())
    {
      return false;
    }
  }

  return true;
}

bool Field::setNewValue()
{
  const bool debug = false;

  std::set<int> valuesPossible;
  getPossibleValuesToTry(valuesPossible);
  std::vector<int> valuesToTry2(valuesPossible.begin(), valuesPossible.end());   // valuesToTry is e.g. [1,2,3,4,5,6,7,8,9] for max_value_ = 9

  //choose next value to be tried
  std::vector<int> valuesToTry(max_value_);   // valuesToTry is e.g. [1,2,3,4,5,6,7,8,9] for max_value_ = 9
  for(int i=1; i<=max_value_; i++)
    valuesToTry[i-1] = i;

  std::random_shuffle(valuesToTry.begin(), valuesToTry.end());
  std::random_shuffle(valuesToTry2.begin(), valuesToTry2.end());

  if (debug)
  {
    std::cout << "valuesToTry: [";
    for (auto value : valuesToTry)
    {
      std::cout << value << ",";
    }
    std::cout << "]" << std::endl;

    std::cout << "valuesToTry2: [";
    for (auto value : valuesToTry2)
    {
      std::cout << value << ",";
    }
    std::cout << "]" << std::endl;
  }

  // check each value if it is possible
  bool success = false;
  //for(int i = 0; i < max_value_; i++)
  for(auto currentValue : valuesToTry)
  {
    //int currentValue = valuesToTry[i];

    //if(debug)
    //  std::cout<<" "<<i;

    if(!alreadyTried(currentValue))
    {
      triedValues_.insert(currentValue);
      if(isPossibleFromDependencies(currentValue, true))
      {
        value_ = currentValue;
        if(debug)std::cout<<"set "<<pos()<<" to "<<value_<<std::endl;

        success = true;
        break;
      }
      else if(debug)
        std::cout<<" not possible from dependencies"<<std::endl;
    }
    else if(debug)
      std::cout<<" already tried"<<std::endl;
  }

  // no value was possible
  if(!success)
    return false;

  return true;
}

void Field::storePossible()
{
  if(value_ != 0)		//if value is already known, set the value to possible, all others to not possilble
  {
    for(int i=1; i<=max_value_; i++)
    {
      valuesPossible_[i-1] = false;
    }
    valuesPossible_[value_-1] = true;
    n_possible_ = 0;
  }
  else    // if value is not yet known, set possible due to dependencies
  {
    n_possible_ = 0;
    for(int i=1; i<=max_value_; i++)
    {
      valuesPossible_[i-1] = isPossibleFromDependencies(i);

      if(valuesPossible_[i-1])
        n_possible_++;
    }
  }
}

int Field::n_possible()
{
  return n_possible_;
}

bool Field::valuePossible(int i)
{
  if(fixed_)
  {
    return value_ == i;
  }
  int l = valuesPossible_.size();
  if(l < i)
    return false;
  return valuesPossible_.at(i-1);
}

int Field::onePossibleValue()
{
  for(int i=0; i<max_value_; i++)
  {
    if(valuesPossible_[i])
      return i+1;
  }
  return 0;
}

std::string Field::pos()
{
  std::stringstream s;
  s<<"id="<<id_<<", (x:"<<pos_x_<<", y:"<<pos_y_<<", z:"<<pos_z_<<")";
  return s.str();
}

int Field::pos_x()
{
  return pos_x_;
}

int Field::pos_y()
{
  return pos_y_;
}

int Field::pos_z()
{
  return pos_z_;
}

int Field::id()
{
  return id_;
}

int Field::max_value()
{
  return max_value_;
}

void Field::setPosZ(int pos_z)
{
  pos_z_ = pos_z;
}

void Field::setId(int id)
{
  id_ = id;
}

std::set< std::shared_ptr< Field >, CompareFields > Field::dependencies()
{
  return dependencies_;
}


void Field::addDependencies(std::set<std::shared_ptr<Field>, CompareFields> dependencies)
{
  dependencies_.insert(dependencies.begin(), dependencies.end());
}

void Field::resetTriedValues()
{
  value_ = 0;
  triedValues_.clear();
}

void Field::setAllTriedExceptValue()
{
  triedValues_.clear();
  for(int i=0; i<max_value_; i++)
  {
    if(i != value_)
      triedValues_.insert(i);
  }
}
