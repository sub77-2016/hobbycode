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

#include "HotPlate2DTest.h"

HotPlate2DTest::HotPlate2DTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun, bool continuousRewards)
: LearningTest(numRuns, numTrialsPerRun), 
mWorld(20, 20)
{
	setTestName("2D Hot Plate");
	mMaxStepsPerTrial = 200;
	mUseContinuousRewards = continuousRewards;
}

HotPlate2DTest::~HotPlate2DTest()
{
}

void HotPlate2DTest::startTest()
{
	runTest(true, "results/2DHotPlateCont");
	runTest(false, "results/2DHotPlateDisc");
}

void HotPlate2DTest::runTest(bool useContinuousInputs, 
	const std::string& baseFilename)
{
	DataFile dataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		verve::AgentDescriptor desc;

		if (useContinuousInputs)
		{
			desc.addContinuousSensor(); // Robot X position.
			desc.addContinuousSensor(); // Robot Y position.

			// If we're using dynamic RBFs, we need to keep the resolution 
			// smaller than the actual discrete grid world's resolution.  
			// Otherwise, we'll get a sparse set of RBFs with tiny receptive 
			// fields.  (This is sort of a special case; normally we would 
			// not use continuous sensors in a discrete world.)
			desc.setContinuousSensorResolution(10);
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

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			a.resetShortTermMemory();
			mWorld.randomizeRobotPosition();
			unsigned int stepCount = 0;

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

			//// Print value function data.
			//if (0 == run && 
			//	(trial == 1 || trial == 9 || trial == 49 || trial == 299))
			//{
			//	char fileStr[1024];
			//	sprintf(fileStr, "%s-trial%d-value.dat", 
			//		baseFilename.c_str(), trial);
			//	a.saveValueData(400, fileStr);
			//}

			//// Print RBF position data.
			//if (0 == run && 
			//	(trial == 4 || trial == 9 || trial == 49 || trial == 299))
			//{
			//	char fileStr[1024];
			//	sprintf(fileStr, "%s-trial%d-RBFData.dat", 
			//		baseFilename.c_str(), trial);
			//	a.saveStateRBFData(fileStr);
			//}
		}

		if (run % 50 == 0)
		{
			printRunStatus(run);
		}
	}

	dataFile.printToFile(baseFilename + "-performance.dat");
}

verve::real HotPlate2DTest::computeReward()
{
	if (mUseContinuousRewards)
	{
		// Compute distance to goal.
		verve::real robotPos[2] = {mWorld.getRobotContinuousXPosition(), 
			mWorld.getRobotContinuousYPosition()};

		verve::real distance = 0;

		// Check left side.
		distance = robotPos[0] + 1;

		// Check right side.
		if (1 - robotPos[0] < distance)
		{
			distance = 1 - robotPos[0];
		}

		// Check lower side.
		if (robotPos[1] + 1 < distance)
		{
			distance = robotPos[1] + 1;
		}

		// Check upper side.
		if (1 - robotPos[1] < distance)
		{
			distance = 1 - robotPos[1];
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

void HotPlate2DTest::updateObservation(verve::Observation& obs, 
	bool useContinuousInputs)
{
	if (useContinuousInputs)
	{
		obs.setContinuousValue(0, mWorld.getRobotContinuousXPosition());
		obs.setContinuousValue(1, mWorld.getRobotContinuousYPosition());
	}
	else
	{
		obs.setDiscreteValue(0, mWorld.getRobotXPosition());
		obs.setDiscreteValue(1, mWorld.getRobotYPosition());
	}
}

bool HotPlate2DTest::isRobotAtGoal()
{
	unsigned int posX = mWorld.getRobotXPosition();
	unsigned int posY = mWorld.getRobotYPosition();

	if (0 == posX || 19 == posX 
		|| 0 == posY || 19 == posY)
	{
		return true;
	}
	else
	{
		return false;
	}
}
