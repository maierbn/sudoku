#pragma once
#include <memory>

class Field;

class CompareFields
{
  public:
    bool operator()(std::shared_ptr< Field > a, std::shared_ptr< Field > b);
};
