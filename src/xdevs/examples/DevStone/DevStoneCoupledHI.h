#ifndef _DevStonecoupledHI_h_
#define _DevStonecoupledHI_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../../core/modeling/Port.h"
#include "../../core/modeling/Coupled.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledHI : public Coupled {
public:
	/// Model input and output ports
	Port iIn;
	Port oOut;

	DevStoneCoupledHI(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime);
	virtual ~DevStoneCoupledHI();
};

#endif
