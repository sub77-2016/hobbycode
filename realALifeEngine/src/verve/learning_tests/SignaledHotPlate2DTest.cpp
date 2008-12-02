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

#include "SignaledHotPlate2DTest.h"

SignaledHotPlate2DTest::SignaledHotPlate2DTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun, bool continuousRewards)
: LearningTest(numRuns, numTrialsPerRun), 
mWorld(20, 20)
{
	setTestName("2D Signaled Hot Plate");
	mMaxStepsPerTrial = 200;
	mUseContinuousRewards = continuousRewards;
	mGoalSide = LEFT_SIDE;
}

SignaledHotPlate2DTest::~SignaledHotPlate2DTest()
{
}

void SignaledHotPlate2DTest::startTest()
{
	runTest(false, "results/2DSignaledHotPlateDisc");
	runTest(true, "results/2DSignaledHotPlateCont");
}

void SignaledHotPlate2DTest::runTest(bool useContinuousInputs, 
	const std::string& baseFilename)
{
	DataFile dataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		verve::AgentDescriptor desc;
		desc.addDiscreteSensor(4); // Reward position signal.

		if (useContinuousInputs)
		{
			desc.addContinuousSensor(); // Robot X position.
			desc.addContinuousSensor(); // Robot Y position.

			// This takes more RBFs to approximate the value function 
			// (reward only at one end) than having rewards at both 
			// ends.
			desc.setContinuousSensorResolution(25);
		}
		else
		{
			desc.addDiscreteSensor(20); // Robot X position.
			desc.addDiscreteSensor(20); // Robot Y position.
		}

		desc.setNumOutputs(5);
		verve::Agent a(desc);
		verve::Observation obs;
		obs.init(a);
		a.setTDLearningRate((verve::real)0.1, 10);
		//a.setETraceTimeConstant((verve::real)0.2);
		//a.setTDDiscountTimeConstant((verve::real)5.0);

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			a.resetShortTermMemory();
			mWorld.randomizeRobotPosition();
			randomizeGoalSide();
			unsigned int stepCount = 0;

			obs.setDiscreteValue(0, mGoalSide);

			while (!isRobotAtGoal() && stepCount < mMaxStepsPerTrial)
			{
				updateObservation(obs, useContinuousInputs);

				// Update the Agent.
				unsigned int action = a.update(computeReward(), obs, 
					(verve::real)0.1);

				switch(action)
				{
					case 0:
						mWorld.moveRobotLeft();
						break;
					case 1:
						mWorld.moveRobotRight();
						break;
					case 2:
						mWorld.moveRobotUp();
						break;
					case 3:
						mWorld.moveRobotDown();
						break;
					case 4:
						// Don't move.
						break;
					default:
						assert(false);
						break;
				}

				++stepCount;
			}

			// If the Agent has actually reached the goal (and did not 
			// simply run out of time), this will reward it.
			updateObservation(obs, useContinuousInputs);
			a.update(computeReward(), obs, (verve::real)0.1);

			dataFile.storeData("trial", trial, (float)trial);
			dataFile.storeData("steps to goal", trial, (float)stepCount);
		}

		if (run % 5 == 0)
		{
			printRunStatus(run);
		}

		if (0 == run)
		{
			//a.saveValueData(400, "results/signaledHotPlate2D-valueData.dat");
			a.saveStateRBFData(baseFilename + "-stateRBFData.dat");
		}
	}

	dataFile.printToFile(baseFilename + "-performance.dat");
}

verve::real SignaledHotPlate2DTest::computeReward()
{
	if (mUseContinuousRewards)
	{
		// Compute distance to goal.
		verve::real robotPos[2] = {mWorld.getRobotContinuousXPosition(), 
			mWorld.getRobotContinuousYPosition()};
		verve::real distance = 0;

		switch(mGoalSide)
		{
			case LEFT_SIDE:
				distance = robotPos[0] + 1;
				break;
			case RIGHT_SIDE:
				distance = 1 - robotPos[0];
				break;
			case LOWER_SIDE:
				distance = robotPos[1] + 1;
				break;
			case UPPER_SIDE:
				distance = 1 - robotPos[1];
				break;
			default:
				assert(false);
		}

		return -distance;
	}
	else
	{
		if (isRobotAtGoal())
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
}

void SignaledHotPlate2DTest::updateObservation(verve::Observation& obs, 
	bool useContinuousInputs)
{
	if (useContinuousInputs)
	{
		obs.setContinuousValue(0, mWorld.getRobotContinuousXPosition());
		obs.setContinuousValue(1, mWorld.getRobotContinuousYPosition());
	}
	else
	{
		obs.setDiscreteValue(1, mWorld.getRobotXPosition());
		obs.setDiscreteValue(2, mWorld.getRobotYPosition());
	}
}

bool SignaledHotPlate2DTest::isRobotAtGoal()
{
	unsigned int posX = mWorld.getRobotXPosition();
	unsigned int posY = mWorld.getRobotYPosition();

	if ((LEFT_SIDE == mGoalSide && 0 == posX) 
		|| (RIGHT_SIDE == mGoalSide && 19 == posX) 
		|| (LOWER_SIDE == mGoalSide && 0 == posY) 
		|| (UPPER_SIDE == mGoalSide && 19 == posY))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SignaledHotPlate2DTest::randomizeGoalSide()
{
	mGoalSide = (GoalSide)verve::globals::randomIntUniform(0, 3);
}
