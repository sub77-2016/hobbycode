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

#include "Population.h"
#include "Connection.h"
#include "Neuron.h"
#include "TDProjection.h"

namespace verve
{
	Population::Population()
	{
	}

	Population::~Population()
	{
		clear();
	}

	void Population::init(unsigned int numNeurons)
	{
		clear();

		// Allocate room for the new Neurons.  This should run a lot 
		// faster than allocating them one at a time.
		mNeurons.reserve(numNeurons);

		for (unsigned int i = 0; i < numNeurons; ++i)
		{
			createNeuron(i);
		}
	}

	void Population::clear()
	{
		while (!mNeurons.empty())
		{
			delete mNeurons.back();
			mNeurons.pop_back();
		}

		// Destroy output Projections.  This will also destroy the 
		// Connections contained within them.
		while (!mOutputProjections.empty())
		{
			delete mOutputProjections.back();
			mOutputProjections.pop_back();
		}

		// Do this to change the allocated size to zero.
		mNeurons.clear();
	}

	void Population::resetShortTermMemory()
	{
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->resetShortTermMemory();
		}

		size = (unsigned int)mOutputProjections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mOutputProjections[i]->resetShortTermMemory();
		}
	}

	void Population::project(Population* pop, 
		InitialWeightMethod initWeightMethod, real maxInputPopActivationSum)
	{
		Projection* proj = new Projection();
		proj->init(this, pop, initWeightMethod, maxInputPopActivationSum);
		mOutputProjections.push_back(proj);
	}

	void Population::projectTD(Population* pop, TDConnectionType type, 
		InitialWeightMethod initWeightMethod, real maxInputPopActivationSum)
	{
		TDProjection* proj = new TDProjection(type);
		proj->init(this, pop, initWeightMethod, maxInputPopActivationSum);
		mOutputProjections.push_back(proj);
	}

	void Population::updateFiringRatesLinear()
	{
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->updateFiringRateLinear();
		}
	}

	void Population::updateFiringRatesLinearBoundedNegOneToOne()
	{
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->updateFiringRateLinearBoundedNegOneToOne();
		}
	}

	void Population::updateFiringRatesLinearBoundedZeroToOne()
	{
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->updateFiringRateLinearBoundedZeroToOne();
		}
	}

	void Population::updateFiringRatesSigmoid()
	{
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->updateFiringRateSigmoid();
		}
	}

	real Population::trainPreDeltaRuleLinear(const real* actualOutputs, 
		real learningFactor)
	{
		// Here we use the delta rule (for a perceptron, not a multilayer 
		// neural network) to train the presynaptic input Connection 
		// weights.  The general form of the weight update rule is: 
		// 
		// dw = n * (d - post) * g'(h) * pre
		// 
		// ...where dw is weight change, n is learning rate, d is the 
		// desired postsynaptic firing rate, post is the actual 
		// postsynaptic firing rate, g' is the first derivative of the 
		// postsynaptic neuron's activation function, h is the postsynaptic 
		// neuron's weighted input sum, and pre is the actual presynaptic 
		// firing rate (assumed to be >= zero).
		// 
		// For linear (y = x) activation functions (whose first derivative 
		// is 1), the weight update rule is: 
		// 
		// dw = n * (d - post) * pre

		real mse = 0;

		unsigned int numNeurons = getNumNeurons();
		for (unsigned int n = 0; n < numNeurons; ++n)
		{
			Neuron* neuron = mNeurons[n];
			real error = actualOutputs[n] - neuron->getFiringRate();
			mse += (error * error);

			unsigned int numInputs = neuron->getNumDendrites();
			for (unsigned int i = 0; i < numInputs; ++i)
			{
				Connection* c = neuron->getDendrite(i);
				c->addToWeight(learningFactor * error * 
					c->getPreNeuron()->getFiringRate());
			}
		}

		if (numNeurons > 1)
		{
			mse /= numNeurons;
		}

		return mse;
	}

	unsigned int Population::getNumNeurons()const
	{
		return (unsigned int)mNeurons.size();
	}

	Neuron* Population::getNeuron(unsigned int i)
	{
		return mNeurons.at(i);
	}

	void Population::normalizeInputWeights()
	{
		unsigned int size = (unsigned int)mNeurons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mNeurons[i]->normalizeInputWeights();
		}
	}

	void Population::setPostETraceDecayFactors(real value)
	{
		unsigned int size = (unsigned int)mOutputProjections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			static_cast<TDProjection*>(mOutputProjections[i])->
				setETraceDecayFactor(value);
		}
	}

	void Population::setPostTDDiscountFactors(real value)
	{
		unsigned int size = (unsigned int)mOutputProjections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			static_cast<TDProjection*>(mOutputProjections[i])->
				setTDDiscountFactor(value);
		}
	}

	void Population::createNeuron(unsigned int id)
	{
		Neuron* neuron = new Neuron(id);
		mNeurons.push_back(neuron);
	}

	//Neuron* Population::findNeuronByID(unsigned int id)
	//{
	//	std::vector<Neuron*>::iterator iter;
	//	for (iter = mAllNeurons.begin(); iter != mAllNeurons.end(); ++iter)
	//	{
	//		if ((*iter)->getID() == id)
	//		{
	//			return (*iter);
	//		}
	//	}

	//	return NULL;
	//}
}
