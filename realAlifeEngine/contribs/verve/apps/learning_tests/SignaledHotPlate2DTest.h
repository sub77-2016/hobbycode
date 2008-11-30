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

#ifndef SIGNALED_HOT_PLATE_2D_TEST_H
#define SIGNALED_HOT_PLATE_2D_TEST_H

#include "../src/LearningTest.h"
#include "common/GridWorld.h"

// The "2D signaled hot plate" task is similar to the regular signaled 
// hot plate task, except that the robot lives in a square 2D world.  The 
// "safe zone" is randomly chosen from the four sides.  The inputs are 
// the robot's x and y position and the signal (which could be one of 
// four possibilities).  There are five outputs: move left, right, up, 
// down, or don't move.

enum GoalSide
{
	LEFT_SIDE,
	RIGHT_SIDE,
	LOWER_SIDE,
	UPPER_SIDE
};

class GridWorld;

class SignaledHotPlate2DTest : public LearningTest
{
public:
	SignaledHotPlate2DTest(unsigned int numRuns, unsigned int numTrialsPerRun, 
		bool useContinuousRewards);

	~SignaledHotPlate2DTest();

protected:
	virtual void startTest();

	void runTest(bool useContinuousInputs, const std::string& baseFilename);

	verve::real computeReward();

	void updateObservation(verve::Observation& obs, 
		bool useContinuousInputs);

	/// Checks if the robot has reached the goal.
	bool isRobotAtGoal();

	///// Returns the distance of the robot from the goal.
	//verve::real computeDistanceFromGoal();

	/// Randomly chooses a new side for the goal.
	void randomizeGoalSide();

	GridWorld mWorld;

	/// Determines on which side the goal is located.
	GoalSide mGoalSide;

	///// The goal location.
	//verve::real mGoalPosition[2];

	/// The maximum number of step allowed per trial.
	unsigned int mMaxStepsPerTrial;

	/// Determines whether the Agent uses discrete or continuous reward 
	/// signals.
	bool mUseContinuousRewards;

private:
};

#endif
