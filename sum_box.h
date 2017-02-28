#pragma once

#include <memory>
#include <set>

#include "field.h"

class SumBox
{
public:
  SumBox();

  void addSummand(std::shared_ptr<Field> summand);

  std::set<std::shared_ptr<Field>, CompareFields>  summands();
private:
  int sum_;

  std::set<std::shared_ptr<Field>, CompareFields> summands_;
};
