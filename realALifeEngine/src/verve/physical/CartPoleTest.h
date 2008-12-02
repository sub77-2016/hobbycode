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

#ifndef CART_POLE_TEST_H
#define CART_POLE_TEST_H

#include "../base/LearningTest.h"
#include "../base/SimulationEngine.h"

// The cart-pole task is that of learning to balance a pole attached 
// to a cart by applying forces to the cart alone.  The Agent has four 
// continuous input sensors: 1) the cart's position, 2) the cart's 
// velocity, 3) the pole's angle, and 4) the pole's angular velocity.  It 
// has three actions: 1) apply force left, 2) apply force right, and 3) do 
// nothing.

class CartPole;
class AgentVisualDebugger;

class CartPoleTest : public LearningTest, public opal::PostStepEventHandler
{
public:
	CartPoleTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	virtual ~CartPoleTest();

protected:
	virtual void startTest();

	virtual void OPAL_CALL handlePostStepEvent();

	void handleInput(opal::real elapsedRealTime, 
		const SimulationEngine& engine);

	bool isOutOfBounds();

	void updateOverlayData(unsigned int trialNum);

	/// The length of each physics step, in seconds.
	verve::real mPhysicsStepSize;

	/// The current amount of elapsed time within a trial.
	opal::real mCurrentTrialTime;

	bool mTrialFinished;

	bool mTaskSolved;

	/// Pointer to the CartPole.
	CartPole* mCartPole;

	///// Pointer to an Ogre overlay object.
	//Ogre::Overlay* mTrialOverlay;

	/// Pointer to the visual Agent debugger.
	AgentVisualDebugger* mAgentDebugger;

private:
};

#endif
