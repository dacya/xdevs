#include "devs.h"

double ta(devs_state s) {
	return s.sigma;
}

struct atomic_operations processor = {
	.ta = ta
};

