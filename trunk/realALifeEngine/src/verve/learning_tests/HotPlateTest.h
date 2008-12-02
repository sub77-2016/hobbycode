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

#ifndef HOT_PLATE_TEST_H
#define HOT_PLATE_TEST_H

#include "../src/LearningTest.h"
#include "common/GridWorld.h"

// The "hot plate" task involves a robot in a one-dimensional world.  
// The center of the world is painful for the robot (i.e. the hot 
// plate); the edges are not painful.  The robot must learn to move 
// to one the edges as quickly as possible.  Note the informative 
// diagram below...
// 
//                         _________
//                        |         |
//                        |   Ow.   |
//                        |_  ______|
//                          |/
//                       /\
//                   u  |oo|
//    robot ---->     \-|__|--c
// ______________________||__________________________
// No Punishment   |   Punishment   |   No Punishment
// 

class World1D;

class HotPlateTest : public LearningTest
{
public:
	HotPlateTest(unsigned int numRuns, unsigned int numTrialsPerRun, 
		bool useContinuousRewards);

	~HotPlateTest();

protected:
	virtual void startTest();

	void runTest(bool useContinuousInputs, const std::string& baseFilename);

	verve::real computeReward();

	void updateObservation(verve::Observation& obs, 
		bool useContinuousInputs);

	/// Checks if the robot has reached the goal.
	bool isRobotAtGoal();

	/// Returns the distance of the robot from the goal.
	verve::real computeDistanceFromGoal();

	GridWorld mWorld;

	///// The position of the left goal.
	//verve::real mLeftGoal;

	///// The position of the right goal.
	//verve::real mRightGoal;

	/// The maximum number of step allowed per trial.
	unsigned int mMaxStepsPerTrial;

	/// Determines whether the Agent uses discrete or continuous reward 
	/// signals.
	bool mUseContinuousRewards;

private:
};

#endif
