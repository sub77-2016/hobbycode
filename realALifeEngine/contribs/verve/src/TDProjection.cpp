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

#include "TDProjection.h"
#include "Neuron.h"

namespace verve
{
	TDProjection::TDProjection(TDConnectionType type)
	: Projection()
	{
		mTDConnectionType = type;
		mETraceDecayFactor = 0;
		mTDDiscountFactor = 0;
	}

	TDProjection::~TDProjection()
	{
	}

	void TDProjection::setETraceDecayFactor(real value)
	{
		// Store this value for new Connections later.
		mETraceDecayFactor = value;

		// Update all existing Connections.
		unsigned int size = (unsigned int)mConnections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			static_cast<TDConnection*>(mConnections[i])->
				setETraceDecayFactor(value);
		}
	}

	void TDProjection::setTDDiscountFactor(real value)
	{
		// Store this value for new Connections later.
		mTDDiscountFactor = value;

		// Update all existing Connections.
		unsigned int size = (unsigned int)mConnections.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			static_cast<TDConnection*>(mConnections[i])->
				setTDDiscountFactor(value);
		}
	}

	void TDProjection::connectNeurons(Neuron* neuron1, Neuron* neuron2)
	{
		TDConnection* c = new TDConnection(neuron1, neuron2, 
			mTDConnectionType);

		c->setETraceDecayFactor(mETraceDecayFactor);
		c->setTDDiscountFactor(mTDDiscountFactor);

		// Tell the pre-synaptic Neuron that this connection is one of its 
		// axons.
		neuron1->addAxon(c);

		// Tell the post-synaptic Neuron that this connection is one of its 
		// dendrites.
		neuron2->addDendrite(c);

		mConnections.push_back(c);
	}
}
