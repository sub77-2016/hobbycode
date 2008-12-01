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

#ifndef VERVE_NEURON_H
#define VERVE_NEURON_H

#include "Globals.h"

namespace verve
{
	class Connection;

	/// The basic Neuron class.  It can update its firing rate based on its 
	/// pre-synaptic input sum, or it can have its firing rate set 
	/// explicitly.  The synaptic input sum is automatically updated when 
	/// pre-synaptic Neurons' firing rates change and when pre-synaptic 
	/// Connections' weights change.  
	class Neuron
	{
	public:
		VERVE_DECL Neuron(unsigned int id);

		/// Deallocates any dynamically allocated memory.  This object 
		/// should never be destroyed by the user calling 'delete' because 
		/// memory allocated within the DLL should also be destroyed within 
		/// the DLL.
		VERVE_DECL virtual ~Neuron();

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Sets the current firing rate to the given value.  Automatically 
		/// updates the input sums of all post-synaptic Neurons.
		VERVE_DECL virtual void VERVE_CALL setFiringRate(real value);

		/// Returns the current firing rate.
		VERVE_DECL virtual real VERVE_CALL getFiringRate()const;

		/// Adds to the synaptic input sum.
		VERVE_DECL virtual void VERVE_CALL addToSynapticInputSum(real delta);

		/// Updates the firing rate using a linear function of the current 
		/// synaptic input sum.  The resulting firing rate is unbounded.  
		/// Automatically updates the input sums of all post-synaptic 
		/// Neurons.
		VERVE_DECL virtual void VERVE_CALL updateFiringRateLinear();

		/// Updates the firing rate using a linear function of the current 
		/// synaptic input sum.  The resulting firing rate is bounded 
		/// within [-1, 1].
		VERVE_DECL virtual void VERVE_CALL 
			updateFiringRateLinearBoundedNegOneToOne();

		/// Updates the firing rate using a linear function of the current 
		/// synaptic input sum.  The resulting firing rate is bounded 
		/// within [0, 1].
		VERVE_DECL virtual void VERVE_CALL 
			updateFiringRateLinearBoundedZeroToOne();

		/// Updates the firing rate using a sigmoid function of the current 
		/// synaptic input sum.  The resulting firing rate bounded to the 
		/// range [0, 1].  Automatically updates the input sums of all 
		/// post-synaptic Neurons.
		VERVE_DECL virtual void VERVE_CALL updateFiringRateSigmoid();

		/// Adds a dendrite (input Connection) to the Neuron.
		VERVE_DECL virtual void VERVE_CALL addDendrite(Connection* c);

		/// Returns the number of dendrites (input Connections).
		VERVE_DECL virtual unsigned int VERVE_CALL getNumDendrites()const;

		/// Returns the dendrite (input Connection) at the given index.
		VERVE_DECL virtual Connection* VERVE_CALL getDendrite(unsigned int i);

		/// Adds an axon (output Connection) to the Neuron.
		VERVE_DECL virtual void VERVE_CALL addAxon(Connection* c);

		/// Returns the number of axons (output Connections).
		VERVE_DECL virtual unsigned int VERVE_CALL getNumAxons()const;

		/// Returns the axon (output Connection) at the given index.
		VERVE_DECL virtual Connection* VERVE_CALL getAxon(unsigned int i);

		/// Returns the identifier used when saving/loading Neurons to 
		/// and from files.
		VERVE_DECL virtual int VERVE_CALL getID()const;

		/// Normalizes the input weights to keep them within the range 
		/// [-1, 1].  This does nothing if the Neuron has fewer than 
		/// two input Connections.
		VERVE_DECL virtual void VERVE_CALL normalizeInputWeights();

	protected:
		/// An identifier used for file saving/loading.  This is a unique 
		/// ID within a Population.
		unsigned int mID;

		/// The firing rate.
		real mFiringRate;

		/// The current sum of signals entering through dendrites.
		real mSynapticInputSum;

		/// A list of the Neuron's input Connections.
		std::vector<Connection*> mDendrites;

		/// A list of the Neuron's output Connections.
		std::vector<Connection*> mAxons;
	};
}

#endif
