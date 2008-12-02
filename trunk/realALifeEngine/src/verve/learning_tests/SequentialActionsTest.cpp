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

#include "SequentialActionsTest.h"

SequentialActionsTest::SequentialActionsTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun)
: LearningTest(numRuns, numTrialsPerRun)
{
	setTestName("Sequential Actions");
	mSequenceLength = 7; // Suri & Schultz used sequences of 7 steps.
	mNumTrialsPerBlock = numTrialsPerRun / mSequenceLength;
}

SequentialActionsTest::~SequentialActionsTest()
{
}

void SequentialActionsTest::startTest()
{
	DataFile dataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		verve::AgentDescriptor desc;

		// We use 1 input value for each step of the sequence plus 1 for 
		// the goal state and 1 for the failure state.
		desc.addDiscreteSensor(mSequenceLength + 2);
		desc.setNumOutputs(mSequenceLength);
		verve::Agent a(desc);
		verve::Observation obs;
		obs.init(a);
		a.setTDLearningRate((verve::real)0.1, 1000);

		// Similar to Suri & Schultz's trace decay value (8% remaining after 
		// 500 ms).
		a.setETraceTimeConstant((verve::real)0.2);

		// Similar to Suri & Schultz's discount value (scaled by 0.98 every 
		// 100 ms).
		a.setTDDiscountTimeConstant((verve::real)5.0);

		// We start from the end of the sequence and train backwards, 
		// adding another step each time.
		for (int currentStep = mSequenceLength - 1; currentStep >= 0; 
			--currentStep)
		{
			// Each action/block gets trained for the same number of trials.
			for (unsigned int trial = 0; trial < mNumTrialsPerBlock; ++trial)
			{
				a.resetShortTermMemory();
				bool trialFinished = false;
				unsigned int step = currentStep;
				unsigned int numCorrect = 0;

				while (!trialFinished)
				{
					obs.setDiscreteValue(0, step);
					unsigned int action = a.update(0, obs, (verve::real)0.1);

					if (action == step)
					{
						++numCorrect;

						if (step == mSequenceLength - 1)
						{
							// Success: we have reached the goal state.
							obs.setDiscreteValue(0, mSequenceLength);
							a.update(1, obs, (verve::real)0.1);
							trialFinished = true;
						}
						else
						{
							// Advance to the next step.
							++step;
						}
					}
					else
					{
						// Failure: an incorrect action was chosen.  
						// Note: we actually get better results if we use 
						// a reward of -1 here, but we will use 0 just to 
						// be consistent with Suri & Schultz.
						obs.setDiscreteValue(0, mSequenceLength + 1);
						a.update(0, obs, (verve::real)0.1);
						trialFinished = true;
					}
				}

				unsigned int actualTrialNum = (mSequenceLength - 1 - 
					currentStep) * mNumTrialsPerBlock + trial;
				dataFile.storeData("trial", actualTrialNum, 
					(float)actualTrialNum);
				dataFile.storeData("percent correct", actualTrialNum, 
					100 * (float)numCorrect / (float)(mSequenceLength - 
					currentStep));
			}
		}

		if (run % 50 == 0)
		{
			printRunStatus(run);
		}

		if (0 == run)
		{
			a.saveValueData(0, "results/sequentialActions-valueData.dat");
		}
	}

	dataFile.printToFile("results/sequentialActions-performance.dat");
}
