#include "DevStoneCoupledHOmod.h"

DevStoneCoupledHOmod::DevStoneCoupledHOmod(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime)
  : Coupled(prefix + std::to_string(depth-1)),
    iIn("in"),
    iInAux("inAux"),
    oOut("out") {
  Component::addInPort(&iIn);
  Component::addInPort(&iInAux);
  Component::addOutPort(&oOut);
  if (depth == 1) {
    DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
    Coupled::addComponent(atomic);
    Coupled::addCoupling(this, &iIn, atomic, &atomic->iIn);
    Coupled::addCoupling(atomic, &atomic->oOut, this, &oOut);
  } else {
    DevStoneCoupledHOmod* coupled = new DevStoneCoupledHOmod(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
    Coupled::addComponent(coupled);
    Coupled::addCoupling(this, &iIn, coupled, &coupled->iIn);
    Coupled::addCoupling(coupled, &coupled->oOut, this, &oOut);

    // First layer of atomic models:
    std::vector<DevStoneAtomic*> prevLayer;
    for (int i = 0; i < (width - 1); ++i) {
      DevStoneAtomic* atomic = new DevStoneAtomic(std::string("AL1_") + std::to_string(i + 1) + "_" + name, preparationTime, intDelayTime, extDelayTime);
      Coupled::addComponent(atomic);
      Coupled::addCoupling(this, &iInAux, atomic, &atomic->iIn);
      Coupled::addCoupling(atomic, &atomic->oOut, coupled, &coupled->iInAux);
      prevLayer.push_back(atomic);
    }
    // Second layer of atomic models:
    std::vector<DevStoneAtomic*> currentLayer;
    for (int i = 0; i < (width - 1); ++i) {
      DevStoneAtomic* atomic = new DevStoneAtomic(std::string("AL2_") + std::to_string(i + 1) + "_" + name, preparationTime, intDelayTime, extDelayTime);
      this->addComponent(atomic);
      if (i == 0) {
	Coupled::addCoupling(this, &iInAux, atomic, &atomic->iIn);
      }
      currentLayer.push_back(atomic);
    }
    for (std::vector<DevStoneAtomic*>::size_type i = 0; i < currentLayer.size(); ++i) {
      DevStoneAtomic* fromAtomic = currentLayer[i];
      for (std::vector<DevStoneAtomic*>::size_type j = 0; j < prevLayer.size(); ++j) {
	DevStoneAtomic* toAtomic = prevLayer[j];
	Coupled::addCoupling(fromAtomic, &fromAtomic->oOut, toAtomic, &toAtomic->iIn);
      }
    }

    // Rest of the tree
    prevLayer = currentLayer;
    currentLayer = std::vector<DevStoneAtomic*>();
    int level = 3;
    while (prevLayer.size() > 1) {
      for (std::vector<DevStoneAtomic*>::size_type i = 0; i < prevLayer.size() - 1; ++i) {
	DevStoneAtomic* atomic = new DevStoneAtomic(std::string("AL") + std::to_string(level) + "_" + std::to_string(i + 1) + "_" + name, preparationTime, intDelayTime, extDelayTime);
	Coupled::addComponent(atomic);
	if (i == 0) {
	  Coupled::addCoupling(this, &iInAux, atomic, &atomic->iIn);
	}
	currentLayer.push_back(atomic);
      }
      for (std::vector<DevStoneAtomic*>::size_type i = 0; i < currentLayer.size(); ++i) {
	DevStoneAtomic* fromAtomic = currentLayer[i];
	DevStoneAtomic* toAtomic = prevLayer[i + 1];
	Coupled::addCoupling(fromAtomic, &fromAtomic->oOut, toAtomic, &toAtomic->iIn);
      }
      prevLayer = currentLayer;
      currentLayer = std::vector<DevStoneAtomic*>();
      level++;
    }  
  }
}


DevStoneCoupledHOmod::~DevStoneCoupledHOmod() {
  auto components = Coupled::getComponents();
  for(auto component : components) {
    delete component;
  }
}

