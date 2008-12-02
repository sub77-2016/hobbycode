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

#include "MazeTest.h"

MazeTest::MazeTest(unsigned int numRuns, unsigned int numTrialsPerRun)
: LearningTest(numRuns, numTrialsPerRun)
{
	mWorld.loadMapData("../data/easyMaze2.dat");
	setTestName("Maze");
	mMaxStepsPerTrial = 10000;
}

MazeTest::~MazeTest()
{
}

void MazeTest::startTest()
{
	//runTest(false, "results/mazeDisc");
	runTest(true, "results/mazeCont");
}

void MazeTest::runTest(bool useContinuousInputs, 
	const std::string& baseFilename)
{
	DataFile stepsDataFile(mNumTrialsPerRun);
	DataFile MSEDataFile(mNumTrialsPerRun);
	DataFile planLengthDataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		verve::AgentDescriptor desc;
		desc.setArchitecture(verve::RL);
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

		// LOW POLICY LEARNING RATE
		// For better value function learning, use exploratory actions 
		// longer.
		a.setTDLearningRate((verve::real)0.1, (verve::real)0.1);

		//// MEDIUM POLICY LEARNING RATE
		//a.setTDLearningRate((verve::real)0.1, (verve::real)1);

		//// HIGH POLICY LEARNING RATE
		//// For better immediate learning performance, use high learning 
		//// rates to exploit greedy actions sooner.  This only works well 
		//// for simple tasks.  The harder the task, the more unreliable 
		//// the results because the action probabilities are quickly 
		//// polarized.  If it finds a good greedy solution soon, it works.  
		//// Otherwise, it might never find a good solution since it loses 
		//// exploratory actions so quickly.
		//a.setTDLearningRate((verve::real)0.1, (verve::real)5);

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			a.resetShortTermMemory();
			mWorld.resetRobotPosition();
			unsigned int stepCount = 0;
			verve::real avgMSE = 0;
			verve::real avgPlanLength = 0;

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
				avgMSE += a.getDebugData().modelPredictionError;
				avgPlanLength += a.getDebugData().planLength;
			}

			avgMSE /= stepCount;
			avgPlanLength /= stepCount;

			// If the Agent has actually reached the goal (and did not 
			// simply run out of time), this will reward it.
			updateObservation(obs, useContinuousInputs);
			a.update(computeReward(), obs, (verve::real)0.1);

			stepsDataFile.storeData("trial", trial, (float)trial);
			stepsDataFile.storeData("steps to goal", trial, (float)stepCount);

			MSEDataFile.storeData("trial", trial, (float)trial);
			MSEDataFile.storeData("avg MSE", trial, avgMSE);

			planLengthDataFile.storeData("trial", trial, (float)trial);
			planLengthDataFile.storeData("avg plan length", trial, avgPlanLength);

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
				printTrialAndRunStatus(run, trial, (float)stepCount);
			}
		}

		//if (0 == run)
		//{
		//	a.saveStateRBFData(baseFilename + "-stateRBFData.dat");
		//}
	}

	stepsDataFile.printToFile(baseFilename + "-performance.dat");
	MSEDataFile.printToFile(baseFilename + "-MSE.dat");
	planLengthDataFile.printToFile(baseFilename + "-planLength.dat");
}

verve::real MazeTest::computeReward()
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

void MazeTest::updateObservation(verve::Observation& obs, 
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

bool MazeTest::isRobotAtGoal()
{
	if (mWorld.getRobotXPosition() == mWorld.getGoalXPosition() 
		&& mWorld.getRobotYPosition() == mWorld.getGoalYPosition())
	{
		return true;
	}
	else
	{
		return false;
	}
}
