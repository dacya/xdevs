#ifndef _DevStoneCoupledLI_h_
#define _DevStoneCoupledLI_h_

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "../../core/modeling/Port.h"
#include "../../core/modeling/Coupled.h"
#include "DevStoneAtomic.h"

class DevStoneCoupledLI : public Coupled {
public:
	/// Model input and output ports
	Port iIn;
	Port oOut;

	DevStoneCoupledLI(const std::string& prefix, int width, int depth, double preparationTime, double intDelayTime, double extDelayTime);
	virtual ~DevStoneCoupledLI();
};

#endif
