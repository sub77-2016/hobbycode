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
#include "../RLModule.h"
#include "../Agent.h"
#include "../Observation.h"
#include "../RBFPopulation.h"
#include "../Neuron.h"
#include "../UltraSparseCodePopulation.h"

class MockRLModule : public verve::RLModule
{
public:
	MockRLModule(const verve::Observation& obs, 
		bool isDynamicRBFEnabled, unsigned int numOutputs)
	: RLModule(obs, isDynamicRBFEnabled, numOutputs)
	{
		verve::real stepSize = (verve::real)0.1;
		setETraceTimeConstant((verve::real)0.1, stepSize);
		setTDDiscountTimeConstant((verve::real)0.1, stepSize);
		setTDLearningRate((verve::real)0.1, 5, stepSize);
	}

	//verve::real getETraceDecayFactor()const
	//{
	//	return mETraceDecayFactor;
	//}

	verve::real getTDDiscountFactor()const
	{
		return mTDDiscountFactor;
	}

	verve::real getValueFunctionLearningFactor()const
	{
		return mValueFunctionLearningFactor;
	}

	verve::RBFPopulation* getStatePopulation()const
	{
		return mStateRepresentation;
	}

private:
};

// Update: The value function is now initialized to zero for all states, 
// so the implementation of this test is no longer helpful.
//QT_TEST(testRLModuleResetShortTermMemory)
//{
//	verve::AgentDescriptor desc;
//	desc.addDiscreteSensor(2);
//	desc.setNumOutputs(1);
//	verve::Agent a(desc);
//	verve::Observation obs;
//	obs.init(a);
//	MockRLModule RLModule(obs, desc.isDynamicRBFEnabled(), 
//		desc.getNumOutputs());
//
//	// Get the Agent's initial value estimation.
//	verve::real initialValue = RLModule.computeValueEstimation(obs);
//
//	// Get the value estimation for a different Observation.
//	obs.setDiscreteValue(0, 1);
//
//	// Update the Agent with this Observation in case it needs to add 
//	// a new RBF dynamically.
//	RLModule.update(obs, 0);
//	RLModule.update(obs, 0);
//
//	// The value of the new Observation/state should be different.
//	QT_CHECK_NOT_EQUAL(RLModule.computeValueEstimation(obs), initialValue);
//
//	obs.zeroInputData();
//
//	// The value estimate should now be zero again.
//	QT_CHECK_CLOSE(RLModule.computeValueEstimation(obs), initialValue, 
//		(verve::real)0.0001);
//}

QT_TEST(testRLModuleChangeStepSizeChangesDependentFactors)
{
	// The eligibility trace decay factor and the TD discount factor 
	// should change whenever the step size changes.
	verve::AgentDescriptor desc;
	desc.addContinuousSensor();
	desc.setContinuousSensorResolution(3);
	desc.setNumOutputs(1);
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	MockRLModule RLModule(obs, desc.isDynamicRBFEnabled(), 
		desc.getNumOutputs());
	
	// Start with step size 1.
	RLModule.changeStepSize(1);

	// Store the initial step size-dependent values.
	obs.setContinuousValue(0, 1);
	verve::real initialValueFunctionLearningFactor = 
		RLModule.getValueFunctionLearningFactor();
	verve::real initialTDDiscountFactor = RLModule.getTDDiscountFactor();
	RLModule.update(obs, 0);

	// Change the step size.
	RLModule.changeStepSize(2);

	// Make sure the step size-dependent values changed.
	QT_CHECK_NOT_EQUAL(initialValueFunctionLearningFactor, 
		RLModule.getValueFunctionLearningFactor());
	QT_CHECK_NOT_EQUAL(initialTDDiscountFactor, 
		RLModule.getTDDiscountFactor());
}

QT_TEST(testRLModuleSetTDDiscountTimeConstantChangesDiscountFactor)
{
	verve::AgentDescriptor desc;
	desc.addContinuousSensor();
	desc.setContinuousSensorResolution(3);
	desc.setNumOutputs(1);
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	MockRLModule RLModule(obs, desc.isDynamicRBFEnabled(), 
		desc.getNumOutputs());

	obs.setContinuousValue(0, 1);
	verve::real initialTDDiscountFactor = RLModule.getTDDiscountFactor();
	RLModule.setTDDiscountTimeConstant((verve::real)0.2, (verve::real)0.1);
	QT_CHECK_NOT_EQUAL(initialTDDiscountFactor, 
		RLModule.getTDDiscountFactor());
}

QT_TEST(testRLModuleIncreaseTDDiscountTimeConstantIncreasesTDDiscountFactor)
{
	verve::AgentDescriptor desc;
	desc.addContinuousSensor();
	desc.setContinuousSensorResolution(3);
	desc.setNumOutputs(1);
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	MockRLModule RLModule(obs, desc.isDynamicRBFEnabled(), 
		desc.getNumOutputs());

	obs.setContinuousValue(0, 1);
	RLModule.setTDDiscountTimeConstant((verve::real)0.1, (verve::real)0.1);
	verve::real initialTDDiscountFactor = RLModule.getTDDiscountFactor();
	RLModule.setTDDiscountTimeConstant((verve::real)0.2, (verve::real)0.1);
	QT_CHECK_GREATER(RLModule.getTDDiscountFactor(), 
		initialTDDiscountFactor);
}

QT_TEST(testRLModuleConstantInputYieldsConstantOutput)
{
	verve::AgentDescriptor desc;
	desc.addContinuousSensor();
	desc.setContinuousSensorResolution(3);
	desc.setNumOutputs(1);
	verve::Agent a(desc);
	verve::Observation obs;
	obs.init(a);
	MockRLModule testRLModule(obs, desc.isDynamicRBFEnabled(), 
		desc.getNumOutputs());
	obs.setContinuousValue(0, 1);

	verve::real initialValue = testRLModule.computeValueEstimation(obs);
	QT_CHECK_EQUAL(initialValue, testRLModule.computeValueEstimation(obs));
	QT_CHECK_EQUAL(initialValue, testRLModule.computeValueEstimation(obs));
	QT_CHECK_EQUAL(initialValue, testRLModule.computeValueEstimation(obs));
}

QT_TEST(testRLModuleDynamicAndNonDynamicCreateSameConnections)
{
	verve::AgentDescriptor desc;
	desc.setDynamicRBFEnabled(false);
	desc.addDiscreteSensor(2);
	desc.addDiscreteSensor(2);
	desc.setNumOutputs(1);
	verve::Agent nonDynamicAgent(desc);
	verve::Observation nonDynamicObs;
	nonDynamicObs.init(nonDynamicAgent);
	MockRLModule nonDynamicRLModule(nonDynamicObs, 
		desc.isDynamicRBFEnabled(), desc.getNumOutputs());

	verve::RBFPopulation* state = nonDynamicRLModule.getStatePopulation();
	unsigned int numStateNeurons = state->getNumNeurons();
	QT_CHECK_EQUAL(numStateNeurons, 4);

	unsigned int numNonDynamicStateAxons = 0;
	for (unsigned int i = 0; i < numStateNeurons; ++i)
	{
		numNonDynamicStateAxons += state->getNeuron(i)->getNumAxons();
	}

	// Each of the 4 state RBFNeurons should have an axon to the single 
	// action Neuron and the critic Neuron.
	QT_CHECK_EQUAL(numNonDynamicStateAxons, 8);

	// Make the dynamic RBFPopulation generate all 4 RBFs.
	desc.setDynamicRBFEnabled(true);
	verve::Agent dynamicAgent(desc);
	verve::Observation dynamicObs;
	dynamicObs.init(dynamicAgent);
	MockRLModule dynamicRLModule(dynamicObs, desc.isDynamicRBFEnabled(), 
		desc.getNumOutputs());

	dynamicRLModule.update(dynamicObs, 0); // Initial update.
	dynamicObs.setDiscreteValue(0, 0);
	dynamicObs.setDiscreteValue(1, 0);
	dynamicRLModule.update(dynamicObs, 0);
	dynamicObs.setDiscreteValue(0, 0);
	dynamicObs.setDiscreteValue(1, 1);
	dynamicRLModule.update(dynamicObs, 0);
	dynamicObs.setDiscreteValue(0, 1);
	dynamicObs.setDiscreteValue(1, 0);
	dynamicRLModule.update(dynamicObs, 0);
	dynamicObs.setDiscreteValue(0, 1);
	dynamicObs.setDiscreteValue(1, 1);
	dynamicRLModule.update(dynamicObs, 0);

	state = dynamicRLModule.getStatePopulation();
	numStateNeurons = state->getNumNeurons();
	QT_CHECK_EQUAL(numStateNeurons, 4);

	unsigned int numDynamicStateAxons = 0;
	for (unsigned int i = 0; i < numStateNeurons; ++i)
	{
		numDynamicStateAxons += state->getNeuron(i)->getNumAxons();
	}

	QT_CHECK_EQUAL(numDynamicStateAxons, 8);
}
