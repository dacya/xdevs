/*
 * Gpt.cpp
 *
 *  Created on: 8 de may. de 2016
 *      Author: jlrisco
 */

#include "Gpt.h"

Gpt::Gpt(const std::string& name, const double& period, const double& observationTime) : Coupled(name), generator("generator", period),	transducer("transducer", observationTime) {
	Coupled::addComponent(&generator);
	Processor processorADEVS(period);
	AtomicADEVS* processor = new AtomicADEVS(processorADEVS);
	Coupled::addComponent(processor);
	Coupled::addComponent(&transducer);
	Coupled::addCoupling(&generator, &generator.oOut, processor, &(processor->iIn[0]));
	Coupled::addCoupling(&generator, &generator.oOut, &transducer, &transducer.iArrived);
	Coupled::addCoupling(processor, &(processor->oOut[0]), &transducer, &transducer.iSolved);
	Coupled::addCoupling(&transducer, &transducer.oOut, &generator, &generator.iStop);
}

Gpt::~Gpt() { }

