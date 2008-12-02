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

#ifndef CURIOSITY_TEST_H
#define CURIOSITY_TEST_H

#include "../src/LearningTest.h"
#include "common/GridWorld.h"

// This test measures the effectiveness of curiosity with planning.  It 
// uses a 2D grid with two rewards on it, one being larger than the other.  
// The smaller reward is closer to the agent's initial position.  The 
// intended result is that planning agents with curiosity should be 
// more likely to find the larger reward.

class GridWorld;

class CuriosityTest : public LearningTest
{
public:
	CuriosityTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	~CuriosityTest();

protected:
	virtual void startTest();

	void updateObservation(verve::Observation& obs, 
		bool useContinuousInputs);

	void runTest(bool useContinuousInputs, const std::string& baseFilename);

	verve::real computeReward();

	GridWorld mWorld;

	/// The number of steps per trial.
	unsigned int mNumStepsPerTrial;

	unsigned int mRobotStartPosition[2];

	unsigned int mSmallReward1Position[2];
	unsigned int mSmallReward2Position[2];
	unsigned int mSmallReward3Position[2];

	unsigned int mLargeRewardPosition[2];

private:
};

#endif
