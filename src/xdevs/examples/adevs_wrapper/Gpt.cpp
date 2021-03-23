/*
 * Gpt.cpp
 *
 *  Created on: 8 de may. de 2016
 *      Author: jlrisco
 */

#include "Gpt.h"

Gpt::Gpt(const std::string& name, const double& period, const double& observationTime) : Coupled(name){
	generator = new Generator("generator", period);
	Processor* processorADEVS = new Processor(3*period);
	std::list<int> adevs_inports = {0};
	std::list<int> adevs_outports = {1};
	processor = new AtomicADEVS("processor_adevs", processorADEVS, adevs_inports, adevs_outports);
	transducer = new Transducer("transducer", observationTime);
	Coupled::addComponent(generator);
	Coupled::addComponent(processor);
	Coupled::addComponent(transducer);
	Coupled::addCoupling(generator, &(generator->oOut), processor, processor->getInPorts().front());
	Coupled::addCoupling(generator, &(generator->oOut), transducer, &(transducer->iArrived));
	Coupled::addCoupling(processor, processor->getOutPorts().front(), transducer, &(transducer->iSolved));
	Coupled::addCoupling(transducer, &(transducer->oOut), generator, &(generator->iStop));
}

Gpt::~Gpt() { 
	delete generator;
	delete processor;
	delete transducer;
}

