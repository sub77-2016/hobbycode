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

#ifndef SEQUENTIAL_ACTIONS_TEST_H
#define SEQUENTIAL_ACTIONS_TEST_H

#include "../src/LearningTest.h"

// This test is adapted from Suri & Schultz's 1998 paper, "Learning of 
// Sequential Movements by Neural Network Model with Dopamine-Like 
// Reinforcement Signal."  (The major difference here is that we do not 
// use a temporal stimulus representation.)  The agent is presented with 
// an input signal and is expected to output a corresponding action 
// signal.  If the correct action is selected, the next input signal is 
// presented.   Reinforcement is 0 for every step except when the agent 
// correctly chooses the final action in the sequence, in which case the 
// reinforcement = 1.  Training proceeds in reverse order, starting from 
// the last input/output pair in the sequence and working backwards.  
// This allows the agent to assign value to the earliest 
// reward-predicting stimulus in the sequence.

class SequentialActionsTest : public LearningTest
{
public:
	SequentialActionsTest(unsigned int numRuns, unsigned int numTrialsPerRun);

	~SequentialActionsTest();

protected:
	virtual void startTest();

	verve::real computeReward();

	void updateObservation(verve::Observation& obs);

	/// The number of state-action pairs in the sequence.
	unsigned int mSequenceLength;

	/// The number of trials per training "block."
	unsigned int mNumTrialsPerBlock;

private:
};

#endif
