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

#ifndef VERVE_RBF_POPULATION_H
#define VERVE_RBF_POPULATION_H

#include "Population.h"

namespace verve
{
	class Projection;
	struct RBFInputData;

	/// A group of RBFNeurons.  This class can take points represented as a 
	/// mixture of discrete and continuous data.  If an RBFPopulation is 
	/// created with zero dimensions (in both discrete and continuous 
	/// space), it becomes a "null" Population with a single, constantly-
	/// active Neuron, representing a state space with a single state.
	class RBFPopulation : public Population
	{
	public:
		VERVE_DECL RBFPopulation();

		VERVE_DECL virtual ~RBFPopulation();

		/// Creates the Population to fit the given RBFInputData object.
		VERVE_DECL virtual void VERVE_CALL init(
			const RBFInputData& inputData, bool isDynamic);

		/// Overridden to handle the case where we have a single, 
		/// constantly-active Neuron.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Updates all RBFNeuron firing rates.  If the RBFPopulation is 
		/// dynamic and learning is enabled, adds new RBFs when necessary.  
		/// Updates the internal list of active Neurons.
		VERVE_DECL virtual void VERVE_CALL updateFiringRatesRBF(
			const RBFInputData& inputData, bool allowDynamicRBFCreation);

		/// Returns the number of currently-active Neurons.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumActiveNeurons()const;

		/// Returns the active Neuron at the given index in the internal 
		/// active Neuron list.
		VERVE_DECL virtual Neuron* VERVE_CALL getActiveNeuron(
			unsigned int i);

		/// Computes and returns the maximum sum of activation for this 
		/// RBFPopulation.  This information is useful for scaling the 
		/// learning rate when training the output Connections to ensure 
		/// stable learning.
		VERVE_DECL virtual real VERVE_CALL computeMaxActivationSum()const;

	protected:
		/// Overridden to clear the list of output Projections.
		virtual void clear();

		/// Overridden to create an RBFNeuron.  
		virtual void createNeuron(unsigned int id);

		/// Creates a new RBFNeuron and positions it in the input space 
		/// using the given input data.
		void makeNewRBF(const RBFInputData& inputData);

		/// Gives a new RBFNeuron its axons to this RBFPopulation's targets.
		void connectNewRBFToTargets(Neuron* n);

		/// The number of RBFs per dimension in continuous space.
		unsigned int mContinuousResolution;

		/// The number of dimensions in discrete space.
		unsigned int mNumDiscreteDimensions;

		/// The number of dimensions in continuous space.
		unsigned int mNumContinuousDimensions;

		/// Determines whether the data space is initially filled with 
		/// RBFs or if we start with none and add them dynamically as 
		/// necessary.
		bool mIsDynamic;

		/// True if this is a "null" Population with a single, 
		/// constantly-active Neuron.
		bool mSingleState;

		/// The distance from the center of an RBF to 1 standard deviation 
		/// away.
		real mStdDevWidth;

		/// A list of the currently-active Neurons.  This enables us to 
		/// speed up training by focusing on just the necessary set of 
		/// Connections.
		std::vector<Neuron*> mActiveNeurons;
	};
}

#endif
