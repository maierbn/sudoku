#include "sum_box.h"

void SumBox::addSummand(std::shared_ptr<Field> summand)
{
  summands_.insert(summand);
}

std::set<std::shared_ptr<Field>, CompareFields>  SumBox::summands()
{
  return summands_;
}
