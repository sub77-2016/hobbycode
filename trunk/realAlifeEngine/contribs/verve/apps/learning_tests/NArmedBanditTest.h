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

#ifndef N_ARMED_BANDIT_TEST_H
#define N_ARMED_BANDIT_TEST_H

#include "../src/LearningTest.h"

// The n-armed bandit task is a fairly standard reinforcement learning 
// problem.  In the basic version, there are no input signals: the agent 
// must learn to choose the best action from among several options.

class NArmedBanditTest : public LearningTest
{
public:
	NArmedBanditTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	~NArmedBanditTest();

protected:
	virtual void startTest();

private:
	/// Randomizes the rewards for all levers.
	void randomizeLeverRewards();

	/// Returns the (probabilistic) reward from selecting the given lever.
	verve::real pullLever(unsigned int i);

	/// Returns the number of levers.
	unsigned int getNumLevers()const;

	/// The number of levers on the n-armed bandit (i.e. slot machine).
	unsigned int mNumLevers;

	/// The mean rewards for all levers.
	verve::real* mLeverRewards;
};

#endif
