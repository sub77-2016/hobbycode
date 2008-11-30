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

#include "UltraSparseCodePopulation.h"
#include "Neuron.h"

namespace verve
{
	UltraSparseCodePopulation::UltraSparseCodePopulation()
	: Population()
	{
		mActiveIndex = -1;
	}

	UltraSparseCodePopulation::~UltraSparseCodePopulation()
	{
	}

	void UltraSparseCodePopulation::init(unsigned int numNeurons)
	{
		Population::init(numNeurons);
		mActiveIndex = -1;
	}

	void UltraSparseCodePopulation::resetShortTermMemory()
	{
		// This should zero all firing rates.
		Population::resetShortTermMemory();
		mActiveIndex = -1;
		// Be sure not to activate any Neurons here.
	}

	void UltraSparseCodePopulation::setActiveOutput(unsigned int i)
	{
		if (mNeurons.empty() || i == mActiveIndex)
		{
			return;
		}

		// If another Neuron is currently active, zero its firing rate.
		if (-1 != mActiveIndex)
		{
			mNeurons[mActiveIndex]->setFiringRate(0);
		}

		mActiveIndex = i;
		mNeurons[mActiveIndex]->setFiringRate(1);
	}

	int UltraSparseCodePopulation::getActiveOutput()const
	{
		return mActiveIndex;
	}
}
