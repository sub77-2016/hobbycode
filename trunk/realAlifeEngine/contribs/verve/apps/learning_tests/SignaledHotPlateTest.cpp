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

#include "SignaledHotPlateTest.h"

SignaledHotPlateTest::SignaledHotPlateTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun, bool continuousRewards)
: LearningTest(numRuns, numTrialsPerRun), 
mWorld(20, 1)
{
	setTestName("Signaled Hot Plate");
	mMaxStepsPerTrial = 200;
	mUseContinuousRewards = continuousRewards;
	mGoalOnLeftSide = true;
}

SignaledHotPlateTest::~SignaledHotPlateTest()
{
}

void SignaledHotPlateTest::startTest()
{
	runTest(false, "results/signaledHotPlateDisc");
	runTest(true, "results/signaledHotPlateCont");
}

void SignaledHotPlateTest::runTest(bool useContinuousInputs, 
	const std::string& baseFilename)
{
	DataFile dataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		verve::AgentDescriptor desc;
		desc.addDiscreteSensor(2); // Goal position sensor.

		if (useContinuousInputs)
		{
			desc.addContinuousSensor(); // Robot position.

			// This takes more RBFs to approximate the value function 
			// (reward only at one end) than having rewards at both 
			// ends.
			desc.setContinuousSensorResolution(25);
		}
		else
		{
			desc.addDiscreteSensor(20); // Robot Position.
		}

		desc.setNumOutputs(3);
		verve::Agent a(desc);
		verve::Observation obs;
		obs.init(a);
		a.setTDLearningRate((verve::real)0.1, 10);

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			a.resetShortTermMemory();
			mWorld.randomizeRobotPosition();
			randomizeGoalSide();
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
			//	(trial == 1 || trial == 4 || trial == 9 || trial == 79))
			//{
			//	char fileStr[1024];
			//	sprintf(fileStr, "%s-trial%d-value.dat", 
			//		baseFilename.c_str(), trial);
			//	a.saveValueData(400, fileStr);
			//}
		}

		if (run % 50 == 0)
		{
			printRunStatus(run);
		}

		//if (0 == run)
		//{
		//	a.saveValueData(400, baseFilename + "-valueData.dat");
		//	a.saveStateRBFData(baseFilename + "-stateRBFData.dat");
		//}
	}

	dataFile.printToFile(baseFilename + "-performance.dat");
}

verve::real SignaledHotPlateTest::computeReward()
{
	if (mUseContinuousRewards)
	{
		// Compute distance to goal.
		verve::real robotPos = mWorld.getRobotContinuousXPosition();
		verve::real distance = 0;

		if (mGoalOnLeftSide)
		{
			distance = robotPos + 1;
		}
		else
		{
			distance = 1 - robotPos;
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

void SignaledHotPlateTest::updateObservation(verve::Observation& obs, 
	bool useContinuousInputs)
{
	if (mGoalOnLeftSide)
	{
		obs.setDiscreteValue(0, 0);
	}
	else
	{
		obs.setDiscreteValue(0, 1);
	}

	if (useContinuousInputs)
	{
		obs.setContinuousValue(0, mWorld.getRobotContinuousXPosition());
	}
	else
	{
		obs.setDiscreteValue(1, mWorld.getRobotXPosition());
	}
}

bool SignaledHotPlateTest::isRobotAtGoal()
{
	unsigned int pos = mWorld.getRobotXPosition();
	if ((mGoalOnLeftSide && 0 == pos) 
		|| (!mGoalOnLeftSide && 19 == pos))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void SignaledHotPlateTest::randomizeGoalSide()
{
	mGoalOnLeftSide = verve::globals::randomBool();
}
