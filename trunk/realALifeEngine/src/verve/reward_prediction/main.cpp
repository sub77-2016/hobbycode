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

#include <verve/verve.h>
#include <fstream>
#include <time.h>

/// A simple classical conditioning test of the critic's ability to predict 
/// rewards.
void testRewardPredictionLearning()
{
	const verve::real timeStepSize = (verve::real)0.01;
	verve::real currentTime = 0;
	const verve::real maxTime = 1000;

	/// Output data for this many seconds of the end of the simulation.
	const verve::real printAmount = (verve::real)10;

	// # of stimulus-reward sequences per sec.
	const verve::real stimulusRewardFrequency = (verve::real)1.0;
	const verve::real stepsPerSecond = 1/timeStepSize;
	//verve::real stimulusRewardChance = 0;
	bool inSequence = false;
	verve::real stimulusTime = 1/stimulusRewardFrequency;
	verve::real rewardTime = 0;
	verve::real stimulusLength = (verve::real)0.05;
	verve::real rewardLength = (verve::real)0.05;
	verve::real endStimulusTime = 0;
	verve::real endRewardTime = 0;
	const verve::real interStimulusInterval = (verve::real)0.0;

	// create an RLAgent
	verve::RLAgent* agent = verve::createRLAgent(1, 1);
	agent->setStepSize(timeStepSize);
	agent->applyUniformNoise((verve::real)0.1);

	// setup the output file
	std::ofstream file("./results/reward_prediction.dat", std::ios::out);

	verve::real stimulus = 0;
	verve::real reinforcement = 0;
	//verve::real inputs[1] = 0;
	//const verve::real* outputs = agent->getOutputs();

	while (currentTime < maxTime)
	{
		// reset the stimulus and reinforcement signals
		//if (!inSequence)
		//{
		//	stimulus = 0;
		//}
		stimulus = 0;
		reinforcement = 0;

		//// make stimulus-reward sequences more frequent at first to train the agent
		//if (currentTime < (verve::real)0.5 * maxTime)
		//{
		//	stimulusRewardChance = (stimulusRewardFrequency / stepsPerSecond) * 5;
		//}
		//else
		//{
		//	stimulusRewardChance = stimulusRewardFrequency / stepsPerSecond;
		//}

		// check if we should initiate a stimulus-reward sequence
		//if (!inSequence && verve::globals::randomr(0, 1) < stimulusRewardChance)
		if (!inSequence && currentTime > stimulusTime)
		{
			inSequence = true;
			
			// setup the time that the stimulus should be given
			stimulusTime = currentTime + 1/stimulusRewardFrequency + 
				verve::globals::randomRealUniform(0, 1);
			// setup the time that the reward should be given
			rewardTime = currentTime + stimulusLength + interStimulusInterval;
			
			// give the agent the stimulus
			endStimulusTime = currentTime + stimulusLength;
		}

		// check if we should be applying the stimulus
		if (currentTime < endStimulusTime)
		//if (currentTime < rewardTime)
		{
			//stimulus += (verve::real)0.04;
			stimulus = 1;
		}

		// check if it's time to give the reward and end the sequence
		if (inSequence && currentTime > rewardTime)
		{
			inSequence = false;
			endRewardTime = currentTime + rewardLength;
		}

		// check if we should be applying the reward
		if (currentTime < endRewardTime)
		{
			reinforcement = (verve::real)0.5;
		}

		agent->reinforce(reinforcement);
		agent->setInput(0, stimulus);
		agent->step();

		if (maxTime - currentTime < printAmount)
		{
			// print stats
			file << currentTime << " " << stimulus << " " << reinforcement << " " 
				<< ((verve::RLAgent*)agent)->getTDError() << " " 
				<<  ((verve::RLAgent*)agent)->getCurrentValueEstimation()<< std::endl;
		}

		currentTime += timeStepSize;

		//if (reinforcement > 0 && currentTime >= endRewardTime)
		//{
		//	// we just finished the reward
		//	agent->resetDynamics();
		//}
	}

	agent->destroy();
	file.close();
}

int _main(int argc, char **argv)
{
	std::cout << "===============================================================" << std::endl;
	std::cout << "Verve temporal_difference test" << std::endl;
	std::cout << "This will test the temporal difference reinforcement learning " << std::endl;
	std::cout << "algorithm.  An agent's value estimations and temporal " << std::endl;
	std::cout << "difference prediction errors will be tested in a classical " << std::endl;
	std::cout << "conditioning method using a simple stimulus/reward sequence." << std::endl;
	std::cout << "Output data will be saved to files." << std::endl;
	std::cout << "===============================================================" << std::endl;

	//srand((unsigned int)time(NULL));
	srand(1);

	testRewardPredictionLearning();

	//// testing different decay methods
	//std::ofstream file("../../../../test.dat", std::ios::out);
	//verve::real stepSize = 0.001;
	//verve::real currentTime = 0;
	//verve::real maxTime = 1;
	//verve::real a = 1;
	//verve::real b = 1;
	//while (currentTime <= maxTime)
	//{
	//	a *= verve::globals::calcDecayConstant((verve::real)0.1, 0.01, stepSize);
	//	b *= verve::globals::calcDecayConstant((verve::real)0.001, 0.0001, stepSize);
	//	file << currentTime << " " << a << " " << b << std::endl;
	//	currentTime += stepSize;
	//}
	//file.close();

	return 0;
}
