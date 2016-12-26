#include "field.h"

#include <sstream>
#include <iostream>
#include <algorithm>

Field::Field(int id, int pos_x, int pos_y, int pos_z, int max_value) :
  value_(0), valuesPossible_(max_value), n_possible_(0), pos_x_(pos_x), pos_y_(pos_y), pos_z_(pos_z), id_(id),
  fixed_(false), max_value_(max_value)
{

}

Field::Field(int id, int pos_x, int pos_y, int pos_z, int max_value, std::vector<std::shared_ptr<Field>> dependencies) :
  Field(id, pos_x, pos_y, pos_z, max_value)
{
  dependencies_.insert(dependencies.begin(), dependencies.end());
}

void Field::setValue(int value)
{
  value_ = value;
}

void Field::setFixed(bool fixed)
{
  fixed_ = fixed;
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

bool Field::alreadyTried(int i)
{
  return triedValues_.find(i) != triedValues_.end();
}

bool Field::isPossibleFromDependencies(int i)
{
  for(auto &dependency : dependencies_)
  {
    if(dependency->value() == i)
      return false;
  }

  return true;
}

bool Field::setNewValue()
{
  const bool debug = false;

  //choose next value to be tried
  std::vector<int> valuesToTry(max_value_);
  for(int i=1; i<=max_value_; i++)
    valuesToTry[i-1] = i;

  std::random_shuffle(valuesToTry.begin(), valuesToTry.end());

  int i = value_+1;

  if(debug)
    std::cout<<"start with "<<i<<std::endl;

  bool success = false;
  for(int i = 0; i < max_value_; i++)
  {
    int currentValue = valuesToTry[i];

    if(debug)std::cout<<" "<<i;

    if(!alreadyTried(currentValue))
    {
      triedValues_.insert(currentValue);
      if(isPossibleFromDependencies(currentValue))
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
  if(value_ != 0)		//if value is already known
  {
    for(int i=1; i<=max_value_; i++)
    {
      valuesPossible_[i-1] = false;
    }
    valuesPossible_[value_-1] = true;
    n_possible_ = 0;
  }
  else
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
  s<<pos_x_<<","<<pos_y_<<","<<pos_z_;
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
