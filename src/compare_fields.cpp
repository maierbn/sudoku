#include "compare_fields.h"

#include "field.h"

bool CompareFields::operator()(std::shared_ptr< Field > a, std::shared_ptr< Field > b)
{
  return a->id() < b->id();
}
