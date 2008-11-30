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

#ifndef VERVE_CONNECTION_H
#define VERVE_CONNECTION_H

#include "Globals.h"

namespace verve
{
	class Neuron;

	/// A synaptic connection between two Neurons.
	class Connection
	{
	public:
		/// The pre-synaptic and post-synaptic Neuron pointers here must be 
		/// non-NULL.
		VERVE_DECL Connection(Neuron* preNeuron, Neuron* postNeuron);

		/// Deallocates any dynamically allocated memory.  This object 
		/// should never be destroyed by the user calling 'delete' because 
		/// memory allocated within the DLL should also be destroyed within 
		/// the DLL.
		VERVE_DECL virtual ~Connection();

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Sets the weight.  Automatically updates the input 
		/// sums of all post-synaptic Neurons.
		VERVE_DECL virtual void VERVE_CALL setWeight(real value);

		/// Returns the weight.
		VERVE_DECL virtual real VERVE_CALL getWeight()const;

		/// Adds a value to the weight.  Automatically updates the input 
		/// sums of all post-synaptic Neurons.
		VERVE_DECL virtual void VERVE_CALL addToWeight(real delta);

		/// Returns the pre-synaptic Neuron.
		VERVE_DECL virtual Neuron* VERVE_CALL getPreNeuron()const;

		/// Returns the post-synaptic Neuron.
		VERVE_DECL virtual Neuron* VERVE_CALL getPostNeuron()const;

		///// Applies Gaussian noise to the weights with standard deviation 
		///// equal to the inverse of the square root of the number of 
		///// dendrites connected to the post-synaptic Neuron.  This is 
		///// probably the best way to initialize the weight.
		//VERVE_DECL virtual void VERVE_CALL applyIdealInitialNoise();

	protected:
		/// The Connection weight.
		real mWeight;

		/// The pre-synaptic Neuron.
		Neuron* mPreNeuron;

		/// The post-synaptic Neuron.  
		Neuron* mPostNeuron;
	};
}

#endif
