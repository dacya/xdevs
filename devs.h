#ifndef _DEVS_H_
#define _DEVS_H_

struct devs_state {
	double sigma;
	const char* phase;
	void user_state;
};

struct atomic_operations {
	double (*ta) (devs_state);
	devs_message (*lambda) (devs_state);
	devs_state (*deltint) (devs_state);
	devs_state (*deltext) (devs_state, double, devs_message);
	devs_state (*deltcon) (devs_state, double, devs_message);
};

#endif

