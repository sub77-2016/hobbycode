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

#ifndef VERVE_POPULATION_H
#define VERVE_POPULATION_H

#include "Globals.h"
#include "TDConnection.h"
#include "Projection.h"

namespace verve
{
	class Neuron;

	/// An interface for a group of Neurons of similar function.
	class Population
	{
	public:
		VERVE_DECL Population();

		/// Destroys all Neurons in the Population.
		VERVE_DECL virtual ~Population();

		/// Creates the given number of Neurons for the Population.  Clears 
		/// any existing Neurons first.  This can be called multiple times 
		/// during the life of the Population.
		VERVE_DECL virtual void VERVE_CALL init(unsigned int numNeurons);

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Creates a new output Projection from this Population to the 
		/// given Population.  Sets the Projection's inital Connection 
		/// weight method and the maximum activation sum of the presynaptic 
		/// Population (used to determine initial weight noise).
		VERVE_DECL virtual void VERVE_CALL project(Population* pop, 
			InitialWeightMethod initWeightMethod, 
			real maxInputPopActivationSum);

		/// Creates a TDProjection from this Population to the given one.  
		/// The new TDProjection will use the given type of TDConnections.  
		/// Sets the Projection's inital Connection weight method and the 
		/// maximum activation sum of the presynaptic Population (used to 
		/// determine initial weight noise).
		VERVE_DECL virtual void VERVE_CALL projectTD(Population* pop, 
			TDConnectionType type, 
			InitialWeightMethod initWeightMethod, 
			real maxInputPopActivationSum);

		/// Updates the firing rates of the Neurons using linear 
		/// activation.
		VERVE_DECL virtual void VERVE_CALL updateFiringRatesLinear();

		/// Updates the firing rates of the Neurons using linear 
		/// activation.  The firing rates are bounded to [-1, 1].
		VERVE_DECL virtual void VERVE_CALL 
			updateFiringRatesLinearBoundedNegOneToOne();

		/// Updates the firing rates of the Neurons using linear 
		/// activation.  The firing rates are bounded to [0, 1].
		VERVE_DECL virtual void VERVE_CALL 
			updateFiringRatesLinearBoundedZeroToOne();

		/// Updates the firing rates of the Neurons using sigmoid 
		/// activation.
		VERVE_DECL virtual void VERVE_CALL updateFiringRatesSigmoid();

		/// Trains the input Connection weights using a simple delta rule 
		/// based on the errors between the Population's outputs and the 
		/// given actual outputs.  Assumes the Neurons are using a linear 
		/// activation function.  The size of the given array of actual 
		/// output data must match that of the Population.  Returns the 
		/// mean squared error.
		VERVE_DECL virtual real VERVE_CALL trainPreDeltaRuleLinear(
			const real* actualOutputs, real learningFactor);

		/// Returns the number of Neurons in the Population.
		VERVE_DECL virtual unsigned int VERVE_CALL getNumNeurons()const;

		/// Returns the Neuron at the given index.
		VERVE_DECL virtual Neuron* VERVE_CALL getNeuron(unsigned int i);

		/// Normalizes the weights of all Neurons' input Connections to 
		/// keep them within the range [-1, 1].
		VERVE_DECL virtual void VERVE_CALL normalizeInputWeights();

		/// Sets the eligibility trace decay factor for all output 
		/// Projections (which are assumed to be TDProjections).
		VERVE_DECL virtual void VERVE_CALL setPostETraceDecayFactors(
			real value);

		/// Sets the TD discount factor for all output Projections (which 
		//  are assumed to be TDProjections).
		VERVE_DECL virtual void VERVE_CALL setPostTDDiscountFactors(
			real value);

	protected:
		/// Destroys all Neurons.
		virtual void clear();

		/// Creates a new Neuron with the given ID value.
		virtual void createNeuron(unsigned int id);

		/// A list of all Neurons in the Population.
		std::vector<Neuron*> mNeurons;

		/// A list of all output Projections.
		std::vector<Projection*> mOutputProjections;
	};
}

#endif
