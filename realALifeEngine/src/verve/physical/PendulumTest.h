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

#ifndef PENDULUM_TEST_H
#define PENDULUM_TEST_H

#include "../base/LearningTest.h"
#include "../base/SimulationEngine.h"

// The pendulum task is that of getting a freely-swinging pendulum to 
// hold itself upright and stay balanced.  The Agent has two continuous 
// input sensors: 1) the pendulum's angle, and 2) the pendulum's angular 
// velocity.  It has three actions: 1) apply torque in one direction, 
// 2) apply torque in the other direction, and 3) do nothing.  The 
// pendulum is underactuated, so it must learn to swing back and forth 
// to build momentum.

class Pendulum;
class AgentVisualDebugger;

class PendulumTest : public LearningTest, public opal::PostStepEventHandler
{
public:
	PendulumTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	virtual ~PendulumTest();

protected:
	virtual void startTest();

	virtual void OPAL_CALL handlePostStepEvent();

	void handleInput(opal::real elapsedRealTime, 
		const SimulationEngine& engine);

	verve::real computeReward();

	void updateOverlayData(unsigned int trialNum);

	/// The length of each physics step, in seconds.
	verve::real mPhysicsStepSize;

	/// The current amount of elapsed time within a trial.
	opal::real mCurrentTrialTime;

	/// The length of each trial in seconds.
	opal::real mTrialLength;

	/// The rewards received during a single trial, in rewards per step.
	verve::real mAvgRewardPerStep;

	/// Pointer to the Pendulum.
	Pendulum* mPendulum;

	///// Pointer to an Ogre overlay object.
	//Ogre::Overlay* mTrialOverlay;

	/// Pointer to the visual Agent debugger.
	AgentVisualDebugger* mAgentDebugger;

private:
};

#endif
