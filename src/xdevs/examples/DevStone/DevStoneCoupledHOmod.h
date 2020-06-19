#ifndef _DevStoneCoupledHOmod_h_
#define _DevStoneCoupledHOmod_h_

#include <string>
#include <map>
#include <list>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../../core/modeling/Port.h"
#include "../../core/modeling/Coupled.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledHOmod : public Coupled {
public:
	/// Model input and output ports
	Port iIn, iInAux;
	Port oOut;

	DevStoneCoupledHOmod(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime);
	virtual ~DevStoneCoupledHOmod();
};

#endif
