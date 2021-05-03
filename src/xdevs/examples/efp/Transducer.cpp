/*
 * Transducer.cpp
 *
 *  Created on: 8 de may. de 2016
 *      Author: jlrisco
 */

#include "Transducer.h"

Transducer::Transducer(const std::string& name, double observationTime) : Atomic(name), jobsArrived(0), jobsSolved(0), observationTime(observationTime), totalTa(0), clock(0), iArrived("arrived"), iSolved("solved"), oOut("out") {
  Component::addInPort(&iArrived);
  Component::addInPort(&iSolved);
  Component::addOutPort(&oOut);
}

Transducer::~Transducer() {
}

void Transducer::initialize() {
  Atomic::holdIn("active", observationTime);
}

void Transducer::exit() {
}

void Transducer::deltint() {
  clock += Atomic::getSigma();
  double throughput;
  double avgTaTime;
  if (Atomic::phaseIs("active")) {
    if (jobsSolved >0) {
      avgTaTime = totalTa / jobsSolved;
      if (clock > 0.0) {
	throughput = jobsSolved / clock;
      } else {
	throughput = 0.0;
      }
    } else {
      avgTaTime = 0.0;
      throughput = 0.0;
    }
    std::cout << "End time: " << clock << std::endl;
    std::cout << "Jobs arrived : " << jobsArrived << std::endl;
    std::cout << "Jobs solved : " << jobsSolved << std::endl;
    std::cout << "Average TA = " << avgTaTime << std::endl;
    std::cout << "Throughput = " << throughput << std::endl;
    Atomic::holdIn("done", 0);
  } else {
    Atomic::passivate();
  }
}

void Transducer::deltext(double e) {
  clock += e;
  if (Atomic::phaseIs("active")) {
    if (!iArrived.isEmpty()) {
      std::shared_ptr<Event> event = iArrived.getSingleValue();
      Job job(*(Job*)event.get());
      std::cout << "Start job " << job.getId() << " @ t = " << clock << std::endl;
      job.setTime(clock);
      jobsArrived++;
    }
    if (!iSolved.isEmpty()) {
      std::shared_ptr<Event> event = iSolved.getSingleValue();
      Job job(*(Job*)event.get());
      totalTa += (clock - job.getTime());
      std::cout << "Finish jobAux " << job.getId() << " @ t = " << clock << std::endl;
      job.setTime(clock);
      jobsSolved++;
    }
  }
  //logger.info("###Deltext: "+showState());
}

void Transducer::lambda() {
  if (Atomic::phaseIs("done")) {
    std::shared_ptr<Event> event(nullptr);
    oOut.addValue(event);
  }
}
