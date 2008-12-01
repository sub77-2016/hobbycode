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

#include "CuriosityTest.h"

CuriosityTest::CuriosityTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun)
: LearningTest(numRuns, numTrialsPerRun), 
mWorld(10, 10)
{
	setTestName("Curiosity");
	mRobotStartPosition[0] = 1;
	mRobotStartPosition[1] = 1;
	mSmallReward1Position[0] = 4;
	mSmallReward1Position[1] = 4;
	mSmallReward2Position[0] = 4;
	mSmallReward2Position[1] = 1;
	mSmallReward3Position[0] = 1;
	mSmallReward3Position[1] = 4;
	mLargeRewardPosition[0] = 8;
	mLargeRewardPosition[1] = 8;
	mNumStepsPerTrial = 100;
}

CuriosityTest::~CuriosityTest()
{
}

void CuriosityTest::startTest()
{
	runTest(false, "results/curiosity");
}

void CuriosityTest::runTest(bool useContinuousInputs, 
	const std::string& baseFilename)
{
	DataFile dataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		verve::AgentDescriptor desc;
		desc.setArchitecture(verve::CURIOUS_MODEL_RL);
		desc.setMaxNumPlanningSteps(50);

		if (useContinuousInputs)
		{
			desc.addContinuousSensor(); // Robot x position.
			desc.addContinuousSensor(); // Robot y position.

			// If we're using dynamic RBFs, we need to keep the resolution 
			// smaller than the actual discrete grid world's resolution.  
			// Otherwise, we'll get a sparse set of RBFs with tiny receptive 
			// fields.  (This is sort of a special case; normally we would 
			// not use continuous sensors in a discrete world.)
			desc.setContinuousSensorResolution(15);
		}
		else
		{
			desc.addDiscreteSensor(mWorld.getGridXSize()); // Robot x position.
			desc.addDiscreteSensor(mWorld.getGridYSize()); // Robot y position.
		}

		desc.setNumOutputs(5);
		verve::Agent a(desc);
		verve::Observation obs;
		obs.init(a);
		a.setModelLearningRate((verve::real)0.0);
		a.setTDLearningRate((verve::real)0.1, (verve::real)2.0);

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			a.resetShortTermMemory();
			mWorld.setRobotPosition(mRobotStartPosition[0], 
				mRobotStartPosition[1]);
			unsigned int stepCount = 0;
			verve::real rewardSum = 0;

			while (stepCount < mNumStepsPerTrial)
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
				rewardSum += computeReward();
			}

			// If the Agent has actually reached the goal (and did not 
			// simply run out of time), this will reward it.
			updateObservation(obs, useContinuousInputs);
			a.update(computeReward(), obs, (verve::real)0.1);

			dataFile.storeData("trial", trial, (float)trial);
			dataFile.storeData("reward sum", trial, rewardSum);

			// Print value function data.
			if (0 == run && 
				(trial == 1 || trial == 9 || trial == 49 || trial == 79))
			{
				char fileStr[1024];
				sprintf(fileStr, "%s-trial%d-value.dat", 
					baseFilename.c_str(), trial);
				a.saveValueData(400, fileStr);
			}

			if (trial % 5 == 0)
			{
				printTrialAndRunStatus(run, trial, rewardSum);
			}
		}
	}

	dataFile.printToFile(baseFilename + "-performance.dat");
}

verve::real CuriosityTest::computeReward()
{
	if (mWorld.getRobotXPosition() == mSmallReward1Position[0] 
		&& mWorld.getRobotYPosition() == mSmallReward1Position[1]
		|| mWorld.getRobotXPosition() == mSmallReward2Position[0] 
		&& mWorld.getRobotYPosition() == mSmallReward2Position[1]
		|| mWorld.getRobotXPosition() == mSmallReward3Position[0] 
		&& mWorld.getRobotYPosition() == mSmallReward3Position[1])
	{
		return (verve::real)0.1;
	}
	else if (mWorld.getRobotXPosition() == mLargeRewardPosition[0] 
		&& mWorld.getRobotYPosition() == mLargeRewardPosition[1])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CuriosityTest::updateObservation(verve::Observation& obs, 
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
