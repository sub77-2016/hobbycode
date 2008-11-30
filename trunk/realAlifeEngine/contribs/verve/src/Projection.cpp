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

#include "Projection.h"
#include "Population.h"
#include "Neuron.h"
#include "Connection.h"

namespace verve
{
	Projection::Projection()
	{
		mInitialWeightMethod = IDEAL_NOISE;

	}

	Projection::~Projection()
	{
		clear();
	}

	void Projection::init(Population* pop1, Population* pop2, 
		InitialWeightMethod initWeightMethod, real maxInputPopActivationSum)
	{
		mInitialWeightMethod = initWeightMethod;
		mMaxInputPopActivationSum = maxInputPopActivationSum;
		clear();

		// Allocate room for the new Connections.  This should run a lot 
		// faster than allocating them one at a time.
		unsigned int numConnections = (unsigned int)pop1->getNumNeurons() 
			* (unsigned int)pop2->getNumNeurons();
		mConnections.reserve(numConnections);

		unsigned int size = (unsigned int)pop1->getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			createConnections(pop1->getNeuron(i), pop2);
		}

		setInitialConnectionWeights();
		storeTargetNeuronReferences(pop2);
	}

	void Projection::init(Population* pop, Neuron* neuron, 
		InitialWeightMethod initWeightMethod, real maxInputPopActivationSum)
	{
		mInitialWeightMethod = initWeightMethod;
		mMaxInputPopActivationSum = maxInputPopActivationSum;
		clear();

		// Allocate room for the new Connections.  This should run a lot 
		// faster than allocating them one at a time.
		mConnections.reserve(pop->getNumNeurons());

		unsigned int size = (unsigned int)pop->getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			connectNeurons(pop->getNeuron(i), neuron);
		}

		setInitialConnectionWeights();
		storeTargetNeuronReferences(neuron);
	}

	void Projection::init(Neuron* neuron, Population* pop, 
		InitialWeightMethod initWeightMethod, real maxInputPopActivationSum)
	{
		mInitialWeightMethod = initWeightMethod;
		mMaxInputPopActivationSum = maxInputPopActivationSum;
		clear();

		// Allocate room for the new Connections.  This should run a lot 
		// faster than allocating them one at a time.
		mConnections.reserve(pop->getNumNeurons());

		createConnections(neuron, pop);

		setInitialConnectionWeights();
		storeTargetNeuronReferences(pop);
	}

	void Projection::clear()
	{
		while (!mConnections.empty())
		{
			delete mConnections.back();
			mConnections.pop_back();
		}

		// Do this to change the allocated size to zero.
		mConnections.clear();

		// Empty the list of Neuron pointers.  The actual Neurons will get 
		// deleted elsewhere.
		mTargetNeurons.clear();
	}

	void Projection::resetShortTermMemory()
	{
		unsigned int size = (unsigned int)mConnections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mConnections[i]->resetShortTermMemory();
		}
	}

	void Projection::addPreNeuron(Neuron* neuron)
	{
		unsigned int numTargetNeurons = (unsigned int)mTargetNeurons.size();

		// Allocate room for the new Connections.  This should run a lot 
		// faster than allocating them one at a time.
		mConnections.reserve(mConnections.size() + numTargetNeurons);

		for (unsigned int i = 0; i < numTargetNeurons; ++i)
		{
			connectNeurons(neuron, mTargetNeurons[i]);
			setInitialConnectionWeight(mConnections.back());
		}
	}

	unsigned int Projection::getNumConnections()const
	{
		return (unsigned int)mConnections.size();
	}

	Connection* Projection::getConnection(unsigned int i)
	{
		return mConnections.at(i);
	}

	//void Projection::applyUniformNoise(real min, real max)
	//{
	//	unsigned int size = (unsigned int)mConnections.size();
	//	for (unsigned int i = 0; i < size; ++i)
	//	{
	//		mConnections[i]->addToWeight(globals::randomRealUniform(min, 
	//			max));
	//	}
	//}

	//void Projection::applyGaussianNoise(real variance)
	//{
	//	unsigned int size = (unsigned int)mConnections.size();
	//	for (unsigned int i = 0; i < size; ++i)
	//	{
	//		mConnections[i]->addToWeight(globals::randomRealGauss(variance));
	//	}
	//}

	//void Projection::applyIdealInitialNoise()
	//{
	//	unsigned int size = (unsigned int)mConnections.size();
	//	for (unsigned int i = 0; i < size; ++i)
	//	{
	//		mConnections[i]->applyIdealInitialNoise();
	//	}
	//}

	void Projection::setInitialConnectionWeights()
	{
		unsigned int size = (unsigned int)mConnections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			setInitialConnectionWeight(mConnections[i]);
		}
	}

	void Projection::setInitialConnectionWeight(Connection* c)
	{
		switch(mInitialWeightMethod)
		{
			case IDEAL_NOISE:
			{
				// Apply noise with variance inversely proportional to 
				// the Connection's postsynaptic Neuron's maximum amount 
				// of presynaptic input.  (This is similar to the method 
				// that applies Gaussian noise to the weight with variance 
				// equal to the inverse of the number of dendrites 
				// connected to the postsynaptic Neuron; but this method 
				// assumes that all presynaptic input activations could 
				// be 1 at the same time.)
				real variance = 1 / mMaxInputPopActivationSum;
				real stdDev = globals::sqrt(variance);
				c->setWeight(globals::randomRealNormal(stdDev));
				break;
			}
			case WEIGHTS_NEAR_0:
			{
				// Use variance proportional to the inverse of the max 
				// input activation, but make it a very small amount.  We 
				// want a little random noise, but not much.
				real variance = (real)0.001 / mMaxInputPopActivationSum;
				real stdDev = globals::sqrt(variance);
				c->setWeight(globals::randomRealNormal(stdDev));
				break;
			}
			case WEIGHTS_NEAR_1:
			{
				// Use variance proportional to the inverse of the max 
				// input activation, but make it a very small amount.  We 
				// want a little random noise, but not much.
				real variance = (real)0.001 / mMaxInputPopActivationSum;
				real stdDev = globals::sqrt(variance);
				c->setWeight(globals::randomRealNormal(stdDev));
				break;
			}
			default:
				assert(false);
				break;
		}
	}

	void Projection::createConnections(Neuron* neuron, Population* pop)
	{
		unsigned int size = (unsigned int)pop->getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			connectNeurons(neuron, pop->getNeuron(i));
		}
	}

	void Projection::storeTargetNeuronReferences(Population* pop)
	{
		unsigned int size = pop->getNumNeurons();
		for (unsigned int i = 0; i < size; ++i)
		{
			storeTargetNeuronReferences(pop->getNeuron(i));
		}
	}

	void Projection::storeTargetNeuronReferences(Neuron* neuron)
	{
		mTargetNeurons.push_back(neuron);
	}

	void Projection::connectNeurons(Neuron* neuron1, Neuron* neuron2)
	{
		Connection* c = new Connection(neuron1, neuron2);

		// Tell the pre-synaptic Neuron that this connection is one of its 
		// axons.
		neuron1->addAxon(c);

		// Tell the post-synaptic Neuron that this connection is one of its 
		// dendrites.
		neuron2->addDendrite(c);

		mConnections.push_back(c);
	}
}
