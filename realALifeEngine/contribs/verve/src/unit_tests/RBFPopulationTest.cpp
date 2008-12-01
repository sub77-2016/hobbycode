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
#include "../RBFPopulation.h"
#include "../Observation.h"
#include "../Agent.h"
#include "../ActiveTDConnectionList.h"
#include "../WinnerTakeAllPopulation.h"
#include "../TDProjection.h"
#include "../RBFInputData.h"

QT_TEST(testRBFPopulationNonDynamicDoesNotCreateRBFs)
{
	verve::AgentDescriptor desc;
	desc.setDynamicRBFEnabled(false);
	desc.setContinuousSensorResolution(5);
	desc.addDiscreteSensor(4);
	desc.addDiscreteSensor(4);
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	verve::RBFInputData inputData;
	inputData.init(obs);
	verve::RBFPopulation pop;
	pop.init(inputData, false);

	unsigned int initialNumRBFs = pop.getNumNeurons();

	for (int i = 0; i < 100; ++i)
	{
		obs.setDiscreteValue(0, verve::globals::randomIntUniform(0, 3));
		obs.setDiscreteValue(1, verve::globals::randomIntUniform(0, 3));
		obs.setContinuousValue(0, verve::globals::randomRealUniform(-1, 1));
		obs.setContinuousValue(1, verve::globals::randomRealUniform(-1, 1));

		inputData.copyInputData(obs.getDiscreteInputData(), 
			obs.getContinuousInputData());
		pop.updateFiringRatesRBF(inputData, false);
	}

	QT_CHECK_EQUAL(pop.getNumNeurons(), initialNumRBFs);
}

QT_TEST(testRBFPopulationDynamicCreatesNewRBFsWithLearningEnabled)
{
	verve::ActiveTDConnectionList dummyList;
	verve::AgentDescriptor desc;
	desc.setDynamicRBFEnabled(true);
	desc.setContinuousSensorResolution(5);
	desc.addDiscreteSensor(4);
	desc.addDiscreteSensor(4);
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	verve::RBFInputData inputData;
	inputData.init(obs);
	verve::RBFPopulation pop;
	pop.init(inputData, desc.isDynamicRBFEnabled());

	QT_CHECK_EQUAL(pop.getNumNeurons(), 0);

	obs.setDiscreteValue(0, 0);
	obs.setDiscreteValue(1, 0);
	obs.setContinuousValue(0, -1);
	obs.setContinuousValue(1, -1);
	inputData.copyInputData(obs.getDiscreteInputData(), 
		obs.getContinuousInputData());
	pop.updateFiringRatesRBF(inputData, true);
	QT_CHECK_EQUAL(pop.getNumNeurons(), 1);

	// This should create a new RBF.
	obs.setDiscreteValue(0, 1);
	inputData.copyInputData(obs.getDiscreteInputData(), 
		obs.getContinuousInputData());
	pop.updateFiringRatesRBF(inputData, true);
	QT_CHECK_EQUAL(pop.getNumNeurons(), 2);

	// This should not create any new RBFs since we're using the same 
	// Observation every time.
	pop.updateFiringRatesRBF(inputData, true);
	pop.updateFiringRatesRBF(inputData, true);
	pop.updateFiringRatesRBF(inputData, true);
	QT_CHECK_EQUAL(pop.getNumNeurons(), 2);

	// This should not create a new RBF since learning is disabled.
	obs.setDiscreteValue(0, 2);
	inputData.copyInputData(obs.getDiscreteInputData(), 
		obs.getContinuousInputData());
	pop.updateFiringRatesRBF(inputData, false);
	QT_CHECK_EQUAL(pop.getNumNeurons(), 2);

	// This should create a new RBF.
	obs.setContinuousValue(0, (verve::real)0.5);
	inputData.copyInputData(obs.getDiscreteInputData(), 
		obs.getContinuousInputData());
	pop.updateFiringRatesRBF(inputData, true);
	QT_CHECK_EQUAL(pop.getNumNeurons(), 3);

	// This should not create a new RBF since its close enough to the 
	// previous one.
	obs.setContinuousValue(0, (verve::real)0.501);
	inputData.copyInputData(obs.getDiscreteInputData(), 
		obs.getContinuousInputData());
	pop.updateFiringRatesRBF(inputData, true);
	QT_CHECK_EQUAL(pop.getNumNeurons(), 3);
}
