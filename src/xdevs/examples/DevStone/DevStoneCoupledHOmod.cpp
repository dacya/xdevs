#include "DevStoneCoupledHOmod.h"

DevStoneCoupledHOmod::DevStoneCoupledHOmod(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime)
	: Coupled(prefix),
	  iIn("in"),
	  iInAux("inAux"),
	  oOut("out") {
	Component::addInPort(&iIn);
	Component::addInPort(&iInAux);
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
		DevStoneCoupledHOmod* coupled = new DevStoneCoupledHOmod(prefix, width, depth - 1, preparationTime, intDelayTime, extDelayTime);
		Coupled::addComponent(coupled);
		Coupled::addCoupling(this, &iIn, coupled, &coupled->iIn);
		Coupled::addCoupling(coupled, &coupled->oOut, this, &oOut);

		if(width > 1) {
            std::map<int, std::list<DevStoneAtomic *>> atomics;
            for (int i = 0; i < width; i++) {
                int minRowIdx = (i < 2) ? 0 : (i - 1);
                for (int j = minRowIdx; j < width - 1; j++) {
                    std::string atomicName = "A" + std::to_string(i + 1) + "_" + std::to_string(j + 1) + "_" + name;
                    //std::cout << "Creating " << name << "..." << std::endl;
                    DevStoneAtomic *atomic = new DevStoneAtomic(atomicName, preparationTime, intDelayTime, extDelayTime);
                    Coupled::addComponent(atomic);

                    atomics.insert(std::pair<int, std::list < DevStoneAtomic * >>
                    (i, std::list<DevStoneAtomic *>()));
                    atomics.find(i)->second.push_back(atomic);
                }
            }

            // Connect EIC
            for (auto a: atomics.find(0)->second) {
                Coupled::addCoupling(this, &iInAux, a, &a->iIn);
            }
            for (int i = 1; i < width; i++) {
                DevStoneAtomic *a = atomics.find(i)->second.front();
                Coupled::addCoupling(this, &iInAux, a, &a->iIn);
            }
            // Connect IC
            for (auto a: atomics.find(0)->second) {
                Coupled::addCoupling(a, &a->oOut, coupled, &coupled->iInAux);
            }
            for (int i = 0; i < (int)atomics.find(0)->second.size(); i++) {
                DevStoneAtomic *aTop = *std::next(atomics.find(0)->second.begin(), i);
                for (int j = 0; j < (int)atomics.find(1)->second.size(); j++) {
                    DevStoneAtomic *aDown = *std::next(atomics.find(1)->second.begin(), j);
                    Coupled::addCoupling(aDown, &aDown->oOut, aTop, &aTop->iIn);
                }
            }
            for (int i = 2; i < width; i++) {
                for (int j = 0; j < (int)atomics.find(i)->second.size(); j++) {
                    DevStoneAtomic *aFrom = *std::next(atomics.find(i)->second.begin(), j);
                    DevStoneAtomic *aTo = *std::next(atomics.find(i - 1)->second.begin(), j + 1);
                    Coupled::addCoupling(aFrom, &aFrom->oOut, aTo, &aTo->iIn);
                }
            }
        }
	}
}


DevStoneCoupledHOmod::~DevStoneCoupledHOmod() {
	auto components = Coupled::getComponents();
	for(auto component : components) {
		delete component;
	}
}

