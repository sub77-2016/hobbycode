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

#include "Connection.h"
#include "Neuron.h"

namespace verve
{
	Connection::Connection(Neuron* preNeuron, Neuron* postNeuron)
	{
		assert(preNeuron && postNeuron);

		// Note: We don't have to update the post-synaptic Neuron's input 
		// sum here because the weight is intially zero, so it doesn't 
		// affect anything.

		mPreNeuron = preNeuron;
		mPostNeuron = postNeuron;
		mWeight = 0;
	}

	Connection::~Connection()
	{
	}

	void Connection::resetShortTermMemory()
	{
	}

	real Connection::getWeight()const
	{
		return mWeight;
	}

	void Connection::setWeight(real value)
	{
		// If nothing has changed, quit early to save time.
		if (value == mWeight)
		{
			return;
		}

		real deltaWeight = value - mWeight;
		mWeight = value;

		// If the pre-synaptic Neuron is active, then when need to update 
		// the post-synaptic Neuron's input sum.
		real preFiringRate = mPreNeuron->getFiringRate();
		if (0 != preFiringRate)
		{
			mPostNeuron->addToSynapticInputSum(
				deltaWeight * preFiringRate);
		}
	}

	void Connection::addToWeight(real delta)
	{
		// If nothing has changed, quit early to save time.
		if (0 == delta)
		{
			return;
		}

		mWeight += delta;

		// If the pre-synaptic Neuron is active, then when need to update 
		// the post-synaptic Neuron's input sum.
		real preFiringRate = mPreNeuron->getFiringRate();
		if (0 != preFiringRate)
		{
			mPostNeuron->addToSynapticInputSum(
				delta * preFiringRate);
		}
	}

	Neuron* Connection::getPreNeuron()const
	{
		return mPreNeuron;
	}

	Neuron* Connection::getPostNeuron()const
	{
		return mPostNeuron;
	}

	//void Connection::applyIdealInitialNoise()
	//{
	//	// Applies Gaussian noise to the Connection weights with standard 
	//	// deviation equal to the inverse of the square root of the number 
	//	// of dendrites connected to the post-synaptic Neuron.  See Remi 
	//	// Coulom's PhD thesis for the reasoning.

	//	// We calculate the variance instead of standard deviation 
	//	// since that's what the random function takes as an argument.  
	//	// The standard deviation here should be 
	//	// 1 / sqrt(# of post-synaptic Neuron dendrites).  Since the 
	//	// variance equals the standard deviation squared, we can 
	//	// simply use 1 / # of post-synaptic Neuron dendrites.

	//	// A Connection's post-synaptic Neuron will always have at least 
	//	// one dendrite (i.e. the Connection in question), so we're safe 
	//	// from divide by zero errors here.
	//	real variance = 1 / (real)(mPostNeuron->getNumDendrites());
	//	addToWeight(globals::randomRealGauss(variance));
	//}
}
