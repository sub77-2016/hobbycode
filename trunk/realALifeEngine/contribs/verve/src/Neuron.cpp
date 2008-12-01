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

#include "Neuron.h"
#include "Connection.h"

namespace verve
{
	Neuron::Neuron(unsigned int id)
	{
		mID = id;
		mFiringRate = 0;
		mSynapticInputSum = 0;
	}

	Neuron::~Neuron()
	{
	}

	void Neuron::resetShortTermMemory()
	{
		mFiringRate = 0;
		mSynapticInputSum = 0;
	}

	void Neuron::normalizeInputWeights()
	{
		unsigned int numDendrites = (unsigned int)mDendrites.size();
		if (numDendrites < 2)
		{
			return;
		}

		//// METHOD 1: Keep all weights <= 1.  This scales all weights by a 
		//// constant amount.

		//// Find the maximum weight magnitude.
		//real maxMagnitude = 0;

		//std::vector<Connection*>::iterator iter;
		//for (iter = mDendrites.begin(); iter != mDendrites.end(); ++iter)
		//{
		//	if (globals::abs((*iter)->getWeight()) > maxMagnitude)
		//	{
		//		maxMagnitude = globals::abs((*iter)->getWeight());
		//	}
		//}

		//// If the maximum weight magnitude is larger than 1, use it to 
		//// normalize all of the weights to be <= 1.  We perform this check 
		//// to avoid situations where, for example, the set of weights: 
		//// <0.0, 0.0, 0.000001> would be scaled to: <0.0, 0.0, 1.0>.  In 
		//// other words, if the weights' magnitudes are already <= 1, 
		//// leave them alone.
		//if (maxMagnitude > 1)
		//{
		//	real inverseMagnitude = 1 / maxMagnitude;

		//	for (iter = mDendrites.begin(); iter != mDendrites.end(); ++iter)
		//	{
		//		real weight = (*iter)->getWeight();
		//		//real newWeightMagnitude = globals::scaleZeroToOne(
		//		//	globals::abs(weight), 0, maxMagnitude);
		//		real newWeightMagnitude = globals::abs(weight) * 
		//			inverseMagnitude;

		//		if (weight < 0)
		//		{
		//			(*iter)->setWeight(-newWeightMagnitude);
		//		}
		//		else
		//		{
		//			(*iter)->setWeight(newWeightMagnitude);
		//		}
		//	}
		//}

		// METHOD 2: Scale weights to keep the sum of squared weights equal 
		// to 1 (i.e. a unit circle).

		// Find the sum of squared weights.
		real sumOfSquaredWeights = 0;

		for (unsigned int i = 0; i < numDendrites; ++i)
		{
			real weight = mDendrites[i]->getWeight();
			sumOfSquaredWeights += (weight * weight);
		}

		// Don't do anything if the sum is zero (in which case the weights 
		// are still all zero).
		if (sumOfSquaredWeights > 0)
		{
			// Precompute the inverse of the square root of the sum.
			real invRootSum = 1 / globals::sqrt(sumOfSquaredWeights);

			// Scale the weights.  After this loop the sum of squared 
			// weights should be 1.
			for (unsigned int i = 0; i < numDendrites; ++i)
			{
				mDendrites[i]->setWeight(mDendrites[i]->getWeight() * 
					invRootSum);
			}
		}
	}

	void Neuron::setFiringRate(real value)
	{
		// If nothing has changed, quit early to save time.
		if (value == mFiringRate)
		{
			return;
		}

		real deltaFiringRate = value - mFiringRate;
		mFiringRate = value;

		// Update post-synaptic Neurons' synaptic input sums.
		unsigned int size = (unsigned int)mAxons.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mAxons[i]->getPostNeuron()->addToSynapticInputSum(
				deltaFiringRate * mAxons[i]->getWeight());
		}
	}

	void Neuron::addToSynapticInputSum(real delta)
	{
		mSynapticInputSum += delta;
	}

	void Neuron::updateFiringRateLinear()
	{
		// IDEA: it might speed things up to keep track of whether the 
		// input sum has changed, then only update things if it has.  
		// The Neuron::setFiringRate should at least avoid updating 
		// post-synaptic input sums when the firing rate hasn't changed.

		// Just use the linear function firing rate = input sum.
		setFiringRate(mSynapticInputSum);
	}

	void Neuron::updateFiringRateLinearBoundedNegOneToOne()
	{
		// IDEA: it might speed things up to keep track of whether the 
		// input sum has changed, then only update things if it has.  
		// The Neuron::setFiringRate should at least avoid updating 
		// post-synaptic input sums when the firing rate hasn't changed.

		// Just use the linear function firing rate = input sum.
		if (mSynapticInputSum < -1)
		{
			setFiringRate(-1);
		}
		else if (mSynapticInputSum > 1)
		{
			setFiringRate(1);
		}
		else
		{
			setFiringRate(mSynapticInputSum);
		}
	}

	void Neuron::updateFiringRateLinearBoundedZeroToOne()
	{
		// IDEA: it might speed things up to keep track of whether the 
		// input sum has changed, then only update things if it has.  
		// The Neuron::setFiringRate should at least avoid updating 
		// post-synaptic input sums when the firing rate hasn't changed.

		// Just use the linear function firing rate = input sum.
		if (mSynapticInputSum < 0)
		{
			setFiringRate(0);
		}
		else if (mSynapticInputSum > 1)
		{
			setFiringRate(1);
		}
		else
		{
			setFiringRate(mSynapticInputSum);
		}
	}

	void Neuron::updateFiringRateSigmoid()
	{
		// IDEA: it might speed things up to keep track of whether the 
		// input sum has changed, then only update things if it has.  
		// The Neuron::setFiringRate should at least avoid updating 
		// post-synaptic input sums when the firing rate hasn't changed.

		// Sigmoid function.  Avoid overflow.
		if (mSynapticInputSum < -500)
		{
			setFiringRate(0);
		}
		else if (mSynapticInputSum > 500)
		{
			setFiringRate(1);
		}
		else
		{
			setFiringRate(1 / (1 + globals::exp(-mSynapticInputSum)));
		}
	}

	real Neuron::getFiringRate()const
	{
		return mFiringRate;
	}

	void Neuron::addDendrite(Connection* c)
	{
		mDendrites.push_back(c);
	}

	unsigned int Neuron::getNumDendrites()const
	{
		return (unsigned int)mDendrites.size();
	}

	Connection* Neuron::getDendrite(unsigned int i)
	{
		return mDendrites.at(i);
	}

	void Neuron::addAxon(Connection* c)
	{
		mAxons.push_back(c);
	}

	unsigned int Neuron::getNumAxons()const
	{
		return (unsigned int)mAxons.size();
	}

	Connection* Neuron::getAxon(unsigned int i)
	{
		return mAxons.at(i);
	}

	int Neuron::getID()const
	{
		return mID;
	}
}
