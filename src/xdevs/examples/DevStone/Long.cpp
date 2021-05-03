#include "Long.h"

Long::Long(const long& value) : value(value) { }

Long::Long(const Long& src) : value(src.value) {}

const Long& Long::operator=(const Long& right) {
  this->value = right.value;
  return *(this);
}


Long::~Long() { }

void Long::setValue(const long& value) {
  this->value = value;
}

const long& Long::getValue() const { return value; }
