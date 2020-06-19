#include "DevStoneCoupledLI.h"

DevStoneCoupledLI::DevStoneCoupledLI(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime)
	: Coupled(prefix),
	  iIn("in"),
	  oOut("out") {
	Component::addInPort(&iIn);
	Component::addOutPort(&oOut);
	char buffer[40];
	snprintf(buffer, sizeof(buffer), "%d", depth-1);
	this->name = prefix;
	this->name.append(buffer);
	if (depth == 1) {
		DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A1_") + name, preparationTime, intDelayTime, extDelayTime);
		Coupled::addComponent(atomic);
		Coupled::addCoupling(this, &iIn, atomic, &atomic->iIn);
		Coupled::addCoupling(atomic, &atomic->oOut, this, &oOut);
	} else {
		DevStoneCoupledLI* coupled = new DevStoneCoupledLI(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
		Coupled::addComponent(coupled);
		Coupled::addCoupling(this, &iIn, coupled, &coupled->iIn);
		Coupled::addCoupling(coupled, &coupled->oOut, this, &oOut);
		for (int i = 0; i < (width - 1); ++i) {
			snprintf(buffer, sizeof(buffer), "%d", i+1);
			DevStoneAtomic* atomic = new DevStoneAtomic(std::string("A") + buffer + "_" + name, preparationTime, intDelayTime, extDelayTime);
			Coupled::addComponent(atomic);
			Coupled::addCoupling(this, &iIn, atomic, &atomic->iIn);
		}
	}
}


DevStoneCoupledLI::~DevStoneCoupledLI() {
	auto components = Coupled::getComponents();
	for(auto component : components) {
		delete component;
	}
}

