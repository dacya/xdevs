/*
 * Gpt.cpp
 *
 *  Created on: 8 de may. de 2016
 *      Author: jlrisco
 */

#include "Gpt.h"

Gpt::Gpt(const std::string& name, const double& period, const double& observationTime) : Coupled(name) {
	generator = new Generator("generator", period);
	processor = new Processor("processor", 3*period);
	transducer = new Transducer("transducer", observationTime);
	Coupled::addComponent(generator);
	Coupled::addComponent(processor);
	Coupled::addComponent(transducer);
	Coupled::addCoupling(generator, &(generator->oOut), processor, &(processor->iIn));
	Coupled::addCoupling(generator, &(generator->oOut), transducer, &(transducer->iArrived));
	Coupled::addCoupling(processor, &(processor->oOut), transducer, &(transducer->iSolved));
	Coupled::addCoupling(transducer, &(transducer->oOut), generator, &(generator->iStop));
}

Gpt::~Gpt() { 
	delete generator;
	delete processor;
	delete transducer;
}

