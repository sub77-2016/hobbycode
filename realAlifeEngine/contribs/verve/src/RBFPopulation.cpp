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

#include "RBFPopulation.h"
#include "RBFNeuron.h"
#include "Projection.h"
#include "RBFInputData.h"

namespace verve
{
	RBFPopulation::RBFPopulation()
	: Population()
	{
		mContinuousResolution = 0;
		mNumDiscreteDimensions = 0;
		mNumContinuousDimensions = 0;
		mIsDynamic = true;
		mSingleState = false;
		mStdDevWidth = 0;
	}

	RBFPopulation::~RBFPopulation()
	{
	}

	void RBFPopulation::init(const RBFInputData& inputData, bool isDynamic)
	{
		assert(inputData.contResolution > 0);

		clear();

		mContinuousResolution = inputData.contResolution;
		mNumDiscreteDimensions = inputData.numDiscInputs;
		mNumContinuousDimensions = inputData.numContInputs;
		mIsDynamic = isDynamic;

		// The RBF std. dev. width equals the distance of one standard 
		// deviation from the RBF center.  This value affects the width 
		// of each RBF's Gaussian curve (and, thus, how much the RBFs 
		// will overlap).  

		// RBF WIDTH METHOD 1: Make the RBF width depend on the resolution 
		// (and distance between neighboring RBFs).  We'll set it equal 
		// to the distance between neighboring RBFs.
		mStdDevWidth = 2 / (real)mContinuousResolution;

		// RBF WIDTH METHOD 2: Use a constant width that is independent 
		// of the resolution.  We'll set it to a fraction of the input 
		// range (which is 2).
		//mStdDevWidth = (real)0.5;

		// Check if this is a "null" Population with a single, constantly-
		// active Neuron.
		if (0 == mNumDiscreteDimensions && 0 == mNumContinuousDimensions)
		{
			mSingleState = true;

			// Just create a single RBF to represent the one possible 
			// "state."
			Population::init(1);

			// Keep the single RBFNeuron active constantly.
			mNeurons[0]->setFiringRate(1);

			return;
		}

		// Check if this is a non-dynamic RBFPopulation.  If so, create 
		// an RBF for every position in the input space.
		if (!isDynamic)
		{
			RBFInputData tempInputData;
			tempInputData.init(inputData.numDiscInputs, 
				inputData.discNumOptionsData, inputData.discInputData, 
				inputData.numContInputs, inputData.contResolution, 
				inputData.contCircularData, inputData.contInputData);
			unsigned numStates = tempInputData.computeNumUniqueStates(
				inputData.contResolution);
			for (unsigned int i = 0; i < numStates; ++i)
			{
				tempInputData.setToUniqueState(i, numStates, 
					inputData.contResolution);
				makeNewRBF(tempInputData);
			}
		}

		// If we're using a dynamic RBFPopulation, we don't need to create 
		// any RBFNeurons at this point.  They'll get created dynamically 
		// later.
	}

	void RBFPopulation::resetShortTermMemory()
	{
		Population::resetShortTermMemory();

		// Empty the lists of active Neurons.
		mActiveNeurons.clear();

		if (mSingleState)
		{
			// Keep the single RBFNeuron active constantly.
			mNeurons[0]->setFiringRate(1);
		}
	}

	void RBFPopulation::clear()
	{
		Population::clear();
	}

	void RBFPopulation::updateFiringRatesRBF(const RBFInputData& inputData, 
		bool allowDynamicRBFCreation)
	{
		mActiveNeurons.clear();

		if (mSingleState)
		{
			// This is basically a null Population with a single, 
			// constantly-active Neuron.

			// Track the active Neuron.
			mActiveNeurons.push_back(mNeurons.back());
			return;
		}

		// Assume we need to make a new RBF until we find an existing 
		// one with high activation.
		bool needNewRBF = true;

		unsigned int numRBFs = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < numRBFs; ++i)
		{
			RBFNeuron* neuron = static_cast<RBFNeuron*>(mNeurons[i]);
			RBFActivationCode code = neuron->updateFiringRate(inputData);

			if (HIGH_ACTIVATION == code)
			{
				needNewRBF = false;

				// Track the active Neuron.
				mActiveNeurons.push_back(neuron);
			}
			else if (LOW_ACTIVATION == code)
			{
				// Track the active Neuron.
				mActiveNeurons.push_back(neuron);
			}
		}

		//// We might need to limit the number of RBFs for dynamic RBF allocation 
		//// combined with self-organizing centers; however, this is pointless 
		//// for large state spaces (for res = 16 and 8 dimensions, the max 
		//// number of RBFs is ~4.3E10).
		//unsigned int maxRBFs = (unsigned int)globals::pow(
		//	(verve::real)mContinuousResolution, 
		//	(verve::real)(mNumDiscreteDimensions + mNumContinuousDimensions));
		//if (allowDynamicRBFCreation && mIsDynamic && needNewRBF && 
		//	mNeurons.size() < maxRBFs)
		if (allowDynamicRBFCreation && mIsDynamic && needNewRBF)
		{
			makeNewRBF(inputData);
			Neuron* newRBFNeuron = mNeurons.back();
			connectNewRBFToTargets(newRBFNeuron);

			// Set the new RBFNeuron's activation high because it is located 
			// right on the current data point.
			newRBFNeuron->setFiringRate(1);

			// Save the new RBFNeuron in the active Neuron list.
			mActiveNeurons.push_back(newRBFNeuron);
		}
	}

	void RBFPopulation::createNeuron(unsigned int id)
	{
		RBFNeuron* neuron = new RBFNeuron(id);
		mNeurons.push_back(neuron);
	}

	void RBFPopulation::makeNewRBF(const RBFInputData& inputData)
	{
		// Create the new RBFNeuron.
		createNeuron((unsigned int)mNeurons.size());
		RBFNeuron* newRBFNeuron = static_cast<RBFNeuron*>(mNeurons.back());

		// The RBF's "new RBF threshold" is the RBF's "personal space."  
		// If any points fall within this distance from its center, no 
		// new RBFs will be created.  Since the approximate distance 
		// between each RBF along a given dimension is 2 / resolution, 
		// we'll use it as the threshold value.  This value must be 
		// related to the resolution because this value will affect 
		// the total number of RBFs created in the dynamic case (which 
		// is the purpose of the resolution parameter).
		real newRBFThreshold = 2 / (real)mContinuousResolution;

		// mStdDevWidth must be >= 0.25 * newRBFThreshold to ensure 
		// that the RBFs creation threshold distance isn't beyond 
		// the activation threshold.  The 0.25 comes from the fact 
		// that we set the activation threshold to 4 * stdDevWidth 
		// in RBFNeuron::init.
		assert(mStdDevWidth >= (real)0.25 * newRBFThreshold);

		// Initialize the RBF.  
		newRBFNeuron->init(mStdDevWidth, newRBFThreshold, inputData);
	}

	void RBFPopulation::connectNewRBFToTargets(Neuron* n)
	{
		// Connect the new RBFNeuron to the target Populations by 
		// notifying the output Projections of the new Neuron.
		unsigned int numProjections = (unsigned int)mOutputProjections.size();
		for (unsigned int i = 0; i < numProjections; ++i)
		{
			mOutputProjections[i]->addPreNeuron(n);
		}
	}

	unsigned int RBFPopulation::getNumActiveNeurons()const
	{
		return (unsigned int)mActiveNeurons.size();
	}

	Neuron* RBFPopulation::getActiveNeuron(unsigned int i)
	{
		return mActiveNeurons.at(i);
	}

	real RBFPopulation::computeMaxActivationSum()const
	{
		// The following computes the maximum possible activation sum of all 
		// RBFs.  We can then use this value to adjust the learning rate to 
		// ensure stable learning.  The method we use here depends on 
		// whether the RBF std. dev. width (mStdDevWidth) is constant or 
		// if it depends on the resolution.
		real maxActivationSum = 0;

		if (0 == mNumContinuousDimensions)
		{
			// We only have discrete inputs (or no inputs at all), so there 
			// will only be 1 RBF active at once.
			maxActivationSum = 1;
		}
		else
		{
			// RBF ACTIVATION SUM METHOD 1: Assuming std. dev. width depends 
			// on resolution.  In this case, we will use the experimentally-
			// determined relationship: sum = 2.5^n, where n is the number 
			// of continuous dimensions.
			maxActivationSum = globals::pow((real)2.5, 
				(real)mNumContinuousDimensions);

			//// RBF ACTIVATION SUM METHOD 2: Assuming std. dev. width is 
			//// constant.  In this case, we will use the experimentally-
			//// determined relationship for std. dev. width = 0.5: 
			//// sum = 0.7477 * 0.42^(n-1) * resolution^n, where n is the 
			//// number of continuous dimensions.
			//assert((real)0.5 == mStdDevWidth);
			//maxActivationSum = (real)0.7477 * globals::pow((real)0.42, 
			//	(real)mNumContinuousDimensions - 1) * globals::pow(
			//	(real)mContinuousResolution, (real)mNumContinuousDimensions);
		}

		return maxActivationSum;
	}
}
