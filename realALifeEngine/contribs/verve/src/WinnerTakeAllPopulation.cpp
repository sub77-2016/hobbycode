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

#include "WinnerTakeAllPopulation.h"
#include "Neuron.h"

namespace verve
{
	WinnerTakeAllPopulation::WinnerTakeAllPopulation()
	: UltraSparseCodePopulation()
	{
	}

	WinnerTakeAllPopulation::~WinnerTakeAllPopulation()
	{
	}

	void WinnerTakeAllPopulation::updateFiringRatesWTA()
	{
		// IDEA: This could be a little more efficient if we could avoid 
		// updating all of the Neurons' targets (post-synaptic Neurons) 
		// until the end.  Right now, we are updating all firing rates 
		// twice.

		// Compute new firing rates from the synaptic input sums.  Use 
		// a sigmoidal activation.
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->updateFiringRateSigmoid();
		}

		// Now pick the "winner" Neuron; set its firing rate to 1 and the 
		// rest to 0.  Save the index of the winner.
		mActiveIndex = computeWinnerIndex();

		// Set the winner's firing rate to 1 and the rest to 0.
		for (unsigned int i = 0; i < size; ++i)
		{
			if (i == mActiveIndex)
			{
				mNeurons[i]->setFiringRate(1);
			}
			else
			{
				mNeurons[i]->setFiringRate(0);
			}
		}
	}

	unsigned int WinnerTakeAllPopulation::computeWinnerIndex()
	{
		unsigned int winnerIndex = 0;
		unsigned int size = (unsigned int)mNeurons.size();

		// We will use a roulette selection method to choose the winner.  
		// First we find the total firing rate and count the number of 
		// inactive Neurons.
		real totalFiringRate = 0;
		unsigned int numInactiveNeurons = 0;
		for (unsigned int i = 0; i < size; ++i)
		{
			real firingRate = mNeurons[i]->getFiringRate();
			if (0 == firingRate)
			{
				++numInactiveNeurons;
			}
			else
			{
				totalFiringRate += firingRate;
			}
		}

		// We need to add a little to the total for each inactive Neuron.  
		// This ensures that the 'defaults::minActionSelectionProb' exactly 
		// specifies the minimum action selection probability.
		real extra = 0;
		if (defaults::minActionSelectionProb > 0)
		{
			// The purpose of using this extra variable is to avoid 
			// compiler (e.g. gcc 3.4.4) warnings about division by 
			// zero.  For example, when defaults::minActionSelectionProb 
			// is zero, the calculation below looks like a divide by 
			// zero problem, but this will never happen because of 
			// the 'if' check above.
			real minProb = defaults::minActionSelectionProb;

			// The restriction on 'defaults::minActionSelectionProb' ensure 
			// that the following quantity is a valid positive number.
			extra = totalFiringRate / (1 / minProb - numInactiveNeurons);
		}
		totalFiringRate += numInactiveNeurons * extra;

		// Generate a random number uniformly between 0 and 
		// 'totalFiringRate.'
		real randomValue = globals::randomRealUniform(0, 
			totalFiringRate);

		// Pick a "winner" probabilistically.  A higher firing 
		// rate means a higher chance of being chosen as the 
		// winner.
		real currentTotal = 0;
		for (unsigned int i = 0; i < size; ++i)
		{
			// We need to add some activation to the inactive Neurons to 
			// make the 'defaults::minActionSelectionProb' valid.
			real firingRate = mNeurons[i]->getFiringRate();
			if (0 == firingRate)
			{
				firingRate = extra;
			}

			if (randomValue <= firingRate + currentTotal)
			{
				winnerIndex = i;
				break;
			}
			else
			{
				currentTotal += firingRate;
			}
		}

		return winnerIndex;
	}
}
