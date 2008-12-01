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

#ifndef MAZE_TEST_H
#define MAZE_TEST_H

#include "../src/LearningTest.h"
#include "common/GridWorld.h"

// The maze task uses a simple discrete 2D grid world with wall barriers 
// that define the maze.  Maze data is loaded from a file.  There is a 
// single start state and goal state determined by the maze data.  The 
// Agent gets -1 reward everywhere except the goal state which gives a 
// reward of 0.

class GridWorld;

class MazeTest : public LearningTest
{
public:
	MazeTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	~MazeTest();

protected:
	virtual void startTest();

	void updateObservation(verve::Observation& obs, 
		bool useContinuousInputs);

	void runTest(bool useContinuousInputs, const std::string& baseFilename);

	verve::real computeReward();

	/// Checks if the robot has reached the goal.
	bool isRobotAtGoal();

	GridWorld mWorld;

	///// The position of the left goal.
	//verve::real mLeftGoal;

	///// The position of the right goal.
	//verve::real mRightGoal;

	/// The maximum number of step allowed per trial.
	unsigned int mMaxStepsPerTrial;

private:
};

#endif
