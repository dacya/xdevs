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

AtomicADEVS::AtomicADEVS(adevs::Atomic<PortValue> &modelArg) : Atomic("aDEVS"), model(modelArg)
{
	// Add a fixed number of ports.
	// TODO Fix this in the future
	for (int i = 0; i < 10; ++i)
	{
		iIn[i] = Port(std::to_string(i));
		Component::addInPort(&iIn[i]);
	}
	for (int i = 10; i < 20; ++i)
	{
		oOut[i] = Port(std::to_string(i));
		Component::addOutPort(&oOut[i]);
	}
}

AtomicADEVS::~AtomicADEVS() {
}

void AtomicADEVS::initialize()
{
}

void AtomicADEVS::exit()
{
}

/*double AtomicADEVS::ta()
{
	double sigmaAux = model.ta();
	if (sigmaAux >= DBL_MAX)
	{
		//sigmaAux = Constants::INFINITY;
		sigmaAux = std::numeric_limits<double>::infinity();
	}
	return sigmaAux;
}*/

void AtomicADEVS::deltint()
{
	model.delta_int();
}

void AtomicADEVS::deltext(double e)
{
	adevs::Bag<PortValue> msg = buildMessage();
	model.delta_ext(e, msg);
}

/*void AtomicADEVS::deltcon(double e)
{
	adevs::Bag<PortValue> msg = buildMessage();
	model.delta_conf(msg);
}*/

void AtomicADEVS::lambda()
{
	std::list<Port *> ports = this->getOutPorts();

	adevs::Bag<PortValue> msg;
	model.output_func(msg);
	
	adevs::Bag<PortValue>::const_iterator itr1;
	for (itr1 = msg.begin(); itr1 != msg.end(); itr1++)
	{
		PortValue port_adevs = *itr1;
		for(std::list<Port*>::iterator itr2 = ports.begin(); itr2 != ports.end(); ++itr2) 
		{
			Port* port_xdevs = *itr2;
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
	for (std::list<Port *>::iterator itr1 = ports.begin(); itr1 != ports.end(); ++itr1)
	{
		Port *port = *itr1;
		std::string port_name = port->getName();
		std::list<Event> events = port->getValues();
		for (std::list<Event>::iterator itr2 = events.begin(); itr2 != events.end(); ++itr2)
		{
			Event event = *itr2;
			PortValue pv(std::stoi(port_name), event);
			msg.insert(pv);
		}
	}
	return msg;
}
