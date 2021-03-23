/*
 * Copyright (C) 2016-2021 José Luis Risco Martín <jlrisco@ucm.es>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see
 * http://www.gnu.org/licenses/
 *
 * Contributors:
 *  - José Luis Risco Martín
 */

#include "AtomicADEVS.h"

AtomicADEVS::AtomicADEVS(const std::string& name, adevs::Atomic<PortValue>* model, const std::list<int>& in_ports, const std::list<int>& out_ports) : Atomic(name)
{
	this->model = model;
	for(int port : in_ports) 
	{
		Component::addInPort(new Port(std::to_string(port)));
	}
	for(int port: out_ports)
	{
		Component::addOutPort(new Port(std::to_string(port)));
	}
}

AtomicADEVS::~AtomicADEVS() {
	delete model;
}

void AtomicADEVS::initialize()
{
}

void AtomicADEVS::exit()
{
}

double AtomicADEVS::ta()
{
	double sigmaAux = model->ta();
	if (sigmaAux >= DBL_MAX)
	{
		sigmaAux = std::numeric_limits<double>::infinity();
	}
	return sigmaAux;
}

void AtomicADEVS::deltint()
{
	model->delta_int();
}

void AtomicADEVS::deltext(double e)
{
	adevs::Bag<PortValue> msg = buildMessage();
	model->delta_ext(e, msg);
}

void AtomicADEVS::deltcon(double e)
{
	adevs::Bag<PortValue> msg = buildMessage();
	model->delta_conf(msg);
}

void AtomicADEVS::lambda()
{
	adevs::Bag<PortValue> msg;
	model->output_func(msg);

	std::list<Port *> ports = this->getOutPorts();

	for(auto port_adevs : msg) {
		for(auto port_xdevs : ports) {
			if(port_adevs.port==std::stoi(port_xdevs->getName())) {
				port_xdevs->addValue(port_adevs.value);
			}
		}
	}	
}

adevs::Bag<PortValue> AtomicADEVS::buildMessage()
{
	adevs::Bag<PortValue> msg;
	std::list<Port *> ports = getInPorts();

	for(auto port : ports) {
		const std::string& port_name = port->getName();
		const std::list<Event>& events = port->getValues();
		for(auto event : events) {
			PortValue pv(std::stoi(port_name), event);
			msg.insert(pv);
		}
	}
	return msg;
}
