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

#include "NArmedBanditTest.h"

NArmedBanditTest::NArmedBanditTest(unsigned int numRuns, 
	unsigned int numTrialsPerRun)
: LearningTest(numRuns, numTrialsPerRun)
{
	setTestName("10-Armed Bandit");
	mNumLevers = 10;
	mLeverRewards = new verve::real[mNumLevers];
}

NArmedBanditTest::~NArmedBanditTest()
{
	delete [] mLeverRewards;
}

void NArmedBanditTest::randomizeLeverRewards()
{
	for (unsigned int i = 0; i < mNumLevers; ++i)
	{
		mLeverRewards[i] = verve::globals::randomRealGauss(1);
	}
}

verve::real NArmedBanditTest::pullLever(unsigned int i)
{
	verve::real reward = mLeverRewards[i] + 
		verve::globals::randomRealGauss(1);

	return reward;
}

unsigned int NArmedBanditTest::getNumLevers()const
{
	return mNumLevers;
}

void NArmedBanditTest::startTest()
{
	DataFile dataFile(mNumTrialsPerRun);

	for (unsigned int run = 0; run < mNumRuns; ++run)
	{
		randomizeLeverRewards();
		verve::AgentDescriptor desc;
		desc.setNumOutputs(getNumLevers());
		verve::Agent a(desc);
		verve::Observation obs;
		obs.init(a);
		a.setTDLearningRate((verve::real)0.1, 100);
		a.setTDDiscountTimeConstant((verve::real)0.3);
		verve::real reward = 0;

		for (unsigned int trial = 0; trial < mNumTrialsPerRun; ++trial)
		{
			// We do not reset the Agent's short term memory here because 
			// its eligibility traces must last across trials.
			unsigned int action = a.update(reward, obs, (verve::real)0.1);
			reward = pullLever(action);
			dataFile.storeData("trial", trial, (float)trial);
			dataFile.storeData("reward", trial, reward);
		}

		if (run % 50 == 0)
		{
			printRunStatus(run);
		}
	}

	dataFile.printToFile("results/10-ArmedBandit-performance.dat");
}
