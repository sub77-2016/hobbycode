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
#include "../TDConnection.h"
#include "../Neuron.h"
//#include "../InputNeuron.h"
//#include "../RBFNeuron.h"

struct TDConnectionFixture
{
	TDConnectionFixture(verve::TDConnectionType type)
	: preNeuron(0), 
	postNeuron(0), 
	c(&preNeuron, &postNeuron, type)
	{
		c.setWeight((verve::real)0.5);
	}

	verve::Neuron preNeuron;
	verve::Neuron postNeuron;
	verve::TDConnection c;
};

//QT_TEST(testConnectionPreETraceIncrease)
//{
//	// This test is only relevant for gradual eligibility trace 
//	// increases.
//	if (verve::defaults::gradualETraceIncrease)
//	{
//		TDConnectionFixture f;
//		f.c.setETraceUpdateMethod(verve::PRE_ONLY_ELIGIBILITY);
//
//		// The time constant parameter is the amount of time it takes to 
//		// decay the value to 37% of its initial value.
//		verve::real decayFactor = verve::globals::calcDecayConstant(
//			(verve::real)1.0, (verve::real)1.0);
//		f.c.setETraceDecayFactor(decayFactor);
//		f.preNeuron.setFiringRate(1);
//		f.c.updateETrace();
//		QT_CHECK_CLOSE(f.c.getETrace(), (verve::real)0.63, 
//			(verve::real)0.01);
//	}
//}

//QT_TEST(testConnectionPrePostETraceIncrease)
//{
//	// This test is only relevant for gradual eligibility trace 
//	// increases.
//	if (verve::defaults::gradualETraceIncrease)
//	{
//		TDConnectionFixture f;
//		f.c.setETraceUpdateMethod(verve::PRE_POST_ELIGIBILITY);
//
//		// The time constant parameter is the amount of time it takes to 
//		// decay the value to 37% of its initial value.
//		verve::real decayFactor = verve::globals::calcDecayConstant(
//			(verve::real)1.0, (verve::real)1.0);
//		f.c.setETraceDecayFactor(decayFactor);
//
//		// This will make the postNeuron's firing rate 0.5;
//		f.postNeuron.updateFiringRate();
//
//		f.preNeuron.setFiringRate((verve::real)0.5);
//		f.c.updateETrace();
//		QT_CHECK_CLOSE(f.c.getETrace(), (verve::real)0.5 * 
//			(verve::real)0.5 * (verve::real)0.63, (verve::real)0.01);
//	}
//}

QT_TEST(testConnectionETraceDecrease)
{
	TDConnectionFixture f(verve::VALUE_FUNCTION_TDCONNECTION);

	// The time constant parameter is the amount of time it takes to decay 
	// the value to 37% of its initial value.
	verve::real decayFactor = verve::globals::calcDecayConstant(
		(verve::real)1.0, (verve::real)1.0);
	verve::real discountFactor = verve::globals::calcDecayConstant(
		(verve::real)1.0, (verve::real)1.0);
	f.c.setETraceDecayFactor(decayFactor);
	f.c.setTDDiscountFactor(discountFactor);
	f.preNeuron.setFiringRate(1);
	f.c.increaseETrace();
	f.preNeuron.setFiringRate(0);
	f.c.decayETrace();

	//if (verve::defaults::gradualETraceIncrease)
	//{
	//	QT_CHECK_CLOSE(f.c.getETrace(), (verve::real)0.63 * 
	//		(verve::real)0.37, (verve::real)0.01);
	//}
	//else
	//{
		QT_CHECK_CLOSE(f.c.getETrace(), (verve::real)0.37 * 
			(verve::real)0.37, (verve::real)0.01);
	//}
}

QT_TEST(testConnectionResetShortTermMemory)
{
	// Ensure that STM params are reset but LTM params are not.
	TDConnectionFixture f(verve::VALUE_FUNCTION_TDCONNECTION);

	// Increase the eligibility trace so we can test whether it gets reset.  
	f.c.setETraceDecayFactor((verve::real)0.5);
	f.preNeuron.setFiringRate(1);
	f.c.increaseETrace();
	f.c.setWeight(1);
	f.c.resetShortTermMemory();
	QT_CHECK_CLOSE(f.c.getETrace(), 0, (verve::real)0.0001);
	QT_CHECK_CLOSE(f.c.getWeight(), 1, (verve::real)0.0001);
}
