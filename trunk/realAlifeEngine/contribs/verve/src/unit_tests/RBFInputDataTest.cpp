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
#include "../RBFInputData.h"
#include "../Agent.h"
#include "../AgentDescriptor.h"
#include "../Observation.h"

QT_TEST(testRBFInputDataComputeNumUniqueStates)
{
	verve::RBFInputData inputData;

	// Test 0 discrete sensors, 0 continuous sensors, default continuous 
	// resolution.
	verve::AgentDescriptor desc1;
	verve::Agent agent1(desc1);
	verve::Observation obs1;
	obs1.init(agent1);
	inputData.init(obs1);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(
		inputData.contResolution), 1);

	// Test 0 discrete sensors, 0 continuous sensors, continuous resolution 
	// of 1.
	verve::AgentDescriptor desc2;
	desc2.setContinuousSensorResolution(1);
	verve::Agent agent2(desc2);
	verve::Observation obs2;
	obs2.init(agent2);
	inputData.init(obs2);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(1), 1);

	// Test 2 discrete sensors, 0 continuous sensors, continuous resolution 
	// of 1.
	verve::AgentDescriptor desc3;
	desc3.setContinuousSensorResolution(1);
	desc3.addDiscreteSensor(4);
	desc3.addDiscreteSensor(4);
	verve::Agent agent3(desc3);
	verve::Observation obs3;
	obs3.init(agent3);
	inputData.init(obs3);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(1), 4 * 4);

	// Test 0 discrete sensors, 1 continuous sensors, continuous resolution 
	// of 1.
	verve::AgentDescriptor desc4;
	desc4.setContinuousSensorResolution(1);
	desc4.addContinuousSensor();
	verve::Agent agent4(desc4);
	verve::Observation obs4;
	obs4.init(agent4);
	inputData.init(obs4);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(1), 1);

	// Test 0 discrete sensors, 2 continuous sensors, continuous resolution 
	// of 1.
	verve::AgentDescriptor desc5;
	desc5.setContinuousSensorResolution(1);
	desc5.addContinuousSensor();
	desc5.addContinuousSensor();
	verve::Agent agent5(desc5);
	verve::Observation obs5;
	obs5.init(agent5);
	inputData.init(obs5);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(1), 1);

	// Test 0 discrete sensors, 1 continuous sensors, continuous resolution 
	// of 5.
	verve::AgentDescriptor desc6;
	desc6.setContinuousSensorResolution(5);
	desc6.addContinuousSensor();
	verve::Agent agent6(desc6);
	verve::Observation obs6;
	obs6.init(agent6);
	inputData.init(obs6);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(5), 5);

	// Test 0 discrete sensors, 2 continuous sensors, continuous resolution 
	// of 5.
	verve::AgentDescriptor desc7;
	desc7.setContinuousSensorResolution(5);
	desc7.addContinuousSensor();
	desc7.addContinuousSensor();
	verve::Agent agent7(desc7);
	verve::Observation obs7;
	obs7.init(agent7);
	inputData.init(obs7);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(5), 5 * 5);

	// Test 2 discrete sensors, 2 continuous sensors, continuous resolution 
	// of 5.
	verve::AgentDescriptor desc8;
	desc8.setContinuousSensorResolution(5);
	desc8.addDiscreteSensor(4);
	desc8.addDiscreteSensor(4);
	desc8.addContinuousSensor();
	desc8.addContinuousSensor();
	verve::Agent agent8(desc8);
	verve::Observation obs8;
	obs8.init(agent8);
	inputData.init(obs8);
	QT_CHECK_EQUAL(inputData.computeNumUniqueStates(5), 4 * 4 * 5 * 5);
}

class MockRBFInputData : public verve::RBFInputData
{
public:
	MockRBFInputData()
	: RBFInputData()
	{
	}

	/// Checks if the data from the given Obseration is equal to this 
	/// one.
	bool isEqual(const MockRBFInputData& obs)
	{
		bool equal = true;

		for (unsigned int i = 0; i < numDiscInputs; ++i)
		{
			if (discInputData[i] != obs.discInputData[i])
			{
				equal = false;
			}
		}

		for (unsigned int i = 0; i < numContInputs; ++i)
		{
			if (contInputData[i] != obs.contInputData[i])
			{
				equal = false;
			}
		}

		return equal;
	}
};

QT_TEST(testRBFInputDataSetToUniqueState)
{
	unsigned int continuousResolution = 5;
	verve::AgentDescriptor desc;
	desc.setContinuousSensorResolution(continuousResolution);
	desc.addDiscreteSensor(3);
	desc.addDiscreteSensor(3);
	desc.addContinuousSensor();
	desc.addContinuousSensor();
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	MockRBFInputData inputData;
	inputData.init(obs);

	// Make sure each successive state is different.
	bool neighboringActivationsAreDifferent = true;
	unsigned int numStates = inputData.computeNumUniqueStates(
		continuousResolution);
	MockRBFInputData prevInputData;
	prevInputData.init(obs);
	for (unsigned int i = 0; i < numStates; ++i)
	{
		if (i > 0)
		{
			prevInputData.setToUniqueState(i - 1, numStates, 
				continuousResolution);
			inputData.setToUniqueState(i, numStates, continuousResolution);
			if (inputData.isEqual(prevInputData))
			{
				neighboringActivationsAreDifferent = false;
			}
		}
	}

	QT_CHECK_EQUAL(neighboringActivationsAreDifferent, true);
}
