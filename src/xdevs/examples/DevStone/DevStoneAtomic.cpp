#include "DevStoneAtomic.h"
#include <iostream>

long DevStoneAtomic::NUM_DELT_INTS(0);
long DevStoneAtomic::NUM_DELT_EXTS(0);
long DevStoneAtomic::NUM_EVENT_EXTS(0);

DevStoneAtomic::DevStoneAtomic(const std::string& name, double preparationTime, double intDelayTime, double extDelayTime)
	: Atomic(name),
	  preparationTime(preparationTime),
	  intDelayTime(intDelayTime),
	  extDelayTime(extDelayTime),
	  outValues(),
	  dhrystone(),
	  iIn("in"),
	  oOut("out") {
	Component::addInPort(&iIn);
	Component::addOutPort(&oOut);
}

DevStoneAtomic::~DevStoneAtomic() {}

void DevStoneAtomic::initialize() {
	Atomic::passivate();
}

void DevStoneAtomic::exit() {
}
		
/// Internal transition function
void DevStoneAtomic::deltint() {
	NUM_DELT_INTS++;
	outValues.clear();
	if(intDelayTime>0) dhrystone.execute(intDelayTime);
	Atomic::passivate();
	//std::cout << this->name << " (di)" << std::endl;
}
	
/// External transition function
void DevStoneAtomic::deltext(double e) {
	NUM_DELT_EXTS++;
    if(extDelayTime>0) dhrystone.execute(extDelayTime);
	if(!iIn.isEmpty()) {
		std::list<Event> events = iIn.getValues();
		for(auto event : events) {
			outValues.push_back((long)(long*)event.getPtr());
			NUM_EVENT_EXTS++;
		}
	}
	//cout << name << " Size: " << x.size() << endl;
	Atomic::holdIn("active", preparationTime);
    //std::cout << this->name << " (de)" << std::endl;
}
	

/// Output function.
void DevStoneAtomic::lambda() {
	//for(auto value : outValues) {
	Event event = Event::makeEvent(new long(0));
	oOut.addValue(event);
	//}
}
