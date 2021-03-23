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

#ifndef _ATOMICADEVS_H_
#define _ATOMICADEVS_H_

#include "../../core/modeling/Port.h"
#include "../../core/modeling/Atomic.h"
#include "../../core/modeling/Event.h"
#include "../../core/util/Constants.h"
#include "lib/adevs/include/adevs.h"

typedef adevs::PortValue<Event> PortValue;

class AtomicADEVS : public Atomic {
private:
	adevs::Atomic<PortValue>* model;
public:
	AtomicADEVS(const std::string& name, adevs::Atomic<PortValue>* model, const std::list<int>& in_ports, const std::list<int>& out_ports);
	virtual ~AtomicADEVS();

	virtual void initialize();
	virtual void exit();
	virtual double ta();
	virtual void deltint();
	virtual void deltext(double e);
	virtual void deltcon(double e);
	virtual void lambda();

	adevs::Bag<PortValue> buildMessage();
};

#endif /* _ATOMICADEVS_H_ */
