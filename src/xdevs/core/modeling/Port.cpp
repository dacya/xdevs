/*
 * Port.cpp
 *
 *  Created on: 5 de may. de 2016
 *      Author: jlrisco
 */

#include "Port.h"

Port::Port(const std::string& name) : name(name), values() {}

Port::~Port() {
  this->clear();
}

void Port::clear() {
  values.clear();
}

bool Port::isEmpty() const {
  return values.empty();
}

const std::shared_ptr<Event>& Port::getSingleValue() const {
  return values.front();
}

const std::list<std::shared_ptr<Event>>& Port::getValues() const {
  return values;
}

void Port::addValue(const std::shared_ptr<Event>& value) {
  values.push_back(value);
}

void Port::addValues(const std::list<std::shared_ptr<Event>>& values) {
  for(auto value : values) {
    addValue(value);
  }
}

const std::string& Port::getName() const {
  return name;
}
