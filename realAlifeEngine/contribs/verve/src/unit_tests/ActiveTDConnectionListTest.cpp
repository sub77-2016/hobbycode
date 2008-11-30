/************************************************************************
* Verve                                                                 *
* http://verve-agents.sourceforge.net                                   *
* Copyright (C) 2004-2008                                               *
* Tyler Streeter (http://www.tylerstreeter.net)                         *
*                                                                       *
* This library is free software; you can redistribute it and/or         *
* modify it under the terms of EITHER:                                  *
*   (1) The GNU Lesser General Public License as published by the Free  *
*       Software Foundation; either version 2.1 of the License, or (at  *
*       your option) any later version. The text of the GNU Lesser      *
*       General Public License is included with this library in the     *
*       file license-LGPL.txt.                                          *
*   (2) The BSD-style license that is included with this library in     *
*       the file license-BSD.txt.                                       *
*                                                                       *
* This library is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
* license-LGPL.txt and license-BSD.txt for more details.                *
************************************************************************/

#include "../external/quicktest/quicktest.h"
#include "../ActiveTDConnectionList.h"
#include "../TDConnection.h"
#include "../Neuron.h"

class MockActiveTDConnectionList : public verve::ActiveTDConnectionList
{
public:
	MockActiveTDConnectionList()
	: ActiveTDConnectionList()
	{
	}

	unsigned int getNumConnections()const
	{
		return (unsigned int)mConnections.size();
	}

	//verve::TDConnection* getConnection(unsigned int i)
	//{
	//	if (mConnections.empty())
	//	{
	//		return NULL;
	//	}

	//	std::list<verve::TDConnection*>::iterator iter = 
	//		mConnections.begin();

	//	while (i > 0)
	//	{
	//		++iter;
	//		--i;
	//	}

	//	return *iter;
	//}
};

struct ActiveTDConnectionListFixture
{
	ActiveTDConnectionListFixture(verve::TDConnectionType type)
	: preNeuron1(0), 
	preNeuron2(0), 
	postNeuron1(0), 
	postNeuron2(0), 
	connection1(&preNeuron1, &postNeuron1, type), 
	connection2(&preNeuron1, &postNeuron2, type), 
	connection3(&preNeuron2, &postNeuron1, type), 
	connection4(&preNeuron2, &postNeuron2, type)
	{
		preNeuron1.addAxon(&connection1);
		preNeuron1.addAxon(&connection2);
		preNeuron2.addAxon(&connection3);
		preNeuron2.addAxon(&connection4);
		postNeuron1.addDendrite(&connection1);
		postNeuron1.addDendrite(&connection3);
		postNeuron2.addDendrite(&connection2);
		postNeuron2.addDendrite(&connection4);
	}

	verve::Neuron preNeuron1;
	verve::Neuron preNeuron2;
	verve::Neuron postNeuron1;
	verve::Neuron postNeuron2;
	verve::TDConnection connection1;
	verve::TDConnection connection2;
	verve::TDConnection connection3;
	verve::TDConnection connection4;
	MockActiveTDConnectionList list;
};

QT_TEST(testActiveTDConnectionListInitialNumConnections)
{
	ActiveTDConnectionListFixture f(verve::VALUE_FUNCTION_TDCONNECTION);
	QT_CHECK_EQUAL(f.list.getNumConnections(), 0);
}

QT_TEST(testActiveTDConnectionListClearList)
{
	ActiveTDConnectionListFixture f(verve::VALUE_FUNCTION_TDCONNECTION);
	f.list.addNewActiveConnection(&f.connection1);
	f.list.addNewActiveConnection(&f.connection2);
	f.list.addNewActiveConnection(&f.connection3);
	f.list.addNewActiveConnection(&f.connection4);
	QT_CHECK_EQUAL(f.list.getNumConnections(), 4);
	f.list.clearList();
	QT_CHECK_EQUAL(f.list.getNumConnections(), 0);
}

QT_TEST(testActiveTDConnectionListAddRedundantConnection)
{
	ActiveTDConnectionListFixture f(verve::VALUE_FUNCTION_TDCONNECTION);
	f.list.addNewActiveConnection(&f.connection1);
	f.list.addNewActiveConnection(&f.connection1);
	f.list.addNewActiveConnection(&f.connection1);
	QT_CHECK_EQUAL(f.list.getNumConnections(), 1);
}

QT_TEST(testActiveTDConnectionListDecayRemovesConnections)
{
	// Here we will test the ActiveTDConnectionList::decayETraces function 
	// which should remove Connections that become inactive.
	ActiveTDConnectionListFixture f(verve::VALUE_FUNCTION_TDCONNECTION);
	f.connection1.setETraceDecayFactor((verve::real)0.5);
	f.connection2.setETraceDecayFactor((verve::real)0.5);
	f.connection3.setETraceDecayFactor((verve::real)0.5);
	f.connection4.setETraceDecayFactor((verve::real)0.5);
	f.connection1.setTDDiscountFactor((verve::real)0.5);
	f.connection2.setTDDiscountFactor((verve::real)0.5);
	f.connection3.setTDDiscountFactor((verve::real)0.5);
	f.connection4.setTDDiscountFactor((verve::real)0.5);
	f.preNeuron1.setFiringRate(1);
	f.preNeuron2.setFiringRate(1);
	f.connection1.increaseETrace();
	f.connection2.increaseETrace();
	f.connection3.increaseETrace();
	f.connection4.increaseETrace();
	f.list.addNewActiveConnection(&f.connection1);
	f.list.addNewActiveConnection(&f.connection2);
	f.list.addNewActiveConnection(&f.connection3);
	f.list.addNewActiveConnection(&f.connection4);

	// With the numbers we're using here, 4 decays should remove a 
	// TDConnection.

	QT_CHECK_EQUAL(f.list.getNumConnections(), 4);
	f.list.decayETraces();
	f.list.decayETraces();
	f.list.decayETraces();
	QT_CHECK_EQUAL(f.list.getNumConnections(), 4);
	f.list.decayETraces();
	QT_CHECK_EQUAL(f.list.getNumConnections(), 0);
}
