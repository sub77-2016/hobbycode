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

#ifndef VERVE_PROJECTION_H
#define VERVE_PROJECTION_H

#include "Globals.h"

namespace verve
{
	class Population;
	class Neuron;
	class Connection;

	/// Various methods used to set the weights of new Connections.
	enum InitialWeightMethod
	{
		/// Apply Gaussian noise using an ideal variance level.
		IDEAL_NOISE,

		/// Set new Connection weights to random values very close to 0.
		WEIGHTS_NEAR_0,

		/// Set new Connection weights to random values very close to 1.
		WEIGHTS_NEAR_1
	};

	/// An interface for a group of Connections from one group of Neurons 
	/// to another.
	class Projection
	{
	public:
		VERVE_DECL Projection();

		/// Destroys all Connections.
		VERVE_DECL virtual ~Projection();

		/// Creates Connections from one Population to another.  Clears 
		/// any existing Connections first.  Sets the inital Connection 
		/// weight method and the maximum activation sum of the presynaptic 
		/// Population (used to determine initial weight noise).
		VERVE_DECL virtual void VERVE_CALL init(Population* pop1, 
			Population* pop2, InitialWeightMethod initWeightMethod, 
			real maxInputPopActivationSum);

		/// Creates Connections from a Population to a single Neuron.  
		/// Clears any existing Connections first.  Sets the inital Connection 
		/// weight method and the maximum activation sum of the presynaptic 
		/// Population (used to determine initial weight noise).
		VERVE_DECL virtual void VERVE_CALL init(Population* pop, 
			Neuron* neuron, InitialWeightMethod initWeightMethod, 
			real maxInputPopActivationSum);

		/// Creates Connections from a single Neuron to a Population.  
		/// Clears any existing Connections first.  Sets the inital Connection 
		/// weight method and the maximum activation sum of the presynaptic 
		/// Population (used to determine initial weight noise).
		VERVE_DECL virtual void VERVE_CALL init(Neuron* neuron, 
			Population* pop, InitialWeightMethod initWeightMethod, 
			real maxInputPopActivationSum);

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Connects a new pre-synaptic Neuron to all post-synaptic 
		/// targets.  Adds noise to the new Connections.
		VERVE_DECL virtual void VERVE_CALL addPreNeuron(Neuron* neuron);

		/// Returns the number of Connections in the Projection.
		VERVE_DECL virtual unsigned int VERVE_CALL getNumConnections()const;

		/// Returns the Connection at the given index.
		VERVE_DECL virtual Connection* VERVE_CALL getConnection(
			unsigned int i);

		///// Adds noise to all Connection weights using a uniform 
		///// distribution ranging from the given minimum to the given 
		///// maximum.
		//VERVE_DECL virtual void VERVE_CALL applyUniformNoise(real min, 
		//	real max);

		///// Adds noise to all Connection weights using a Gaussian 
		///// distribution with the given variance.
		//VERVE_DECL virtual void VERVE_CALL applyGaussianNoise(real variance);

		///// Applies Gaussian noise to all trainable Connection weights 
		///// with standard deviation equal to the inverse of the square 
		///// root of the number of dendrites connected to the post-synaptic 
		///// Neuron.  This is probably the best way to initialize the 
		///// weights.
		//VERVE_DECL virtual void VERVE_CALL applyIdealInitialNoise();

	protected:
		/// Destroys all Connections.
		void clear();

		/// Sets all Connection weights to some initial value as determined 
		/// by the InitialWeightMethod.
		void setInitialConnectionWeights();

		/// Sets the Connection's weights to some initial value as determined 
		/// by the InitialWeightMethod.
		void setInitialConnectionWeight(Connection* c);

		/// Creates a set of Connections from a Neuron to a Population.  
		void createConnections(Neuron* neuron, Population* pop);

		/// Stores references to the given Population's Neurons in the 
		/// target Neurons list.
		void storeTargetNeuronReferences(Population* pop);

		/// Stores a reference to the given Neuron in the target Neurons 
		/// list.
		void storeTargetNeuronReferences(Neuron* neuron);

		/// Creates a Connection from one neuron1 to neuron2 and stores 
		/// the Connection internally.  Informs the two Neurons of the 
		/// new Connection.  
		virtual void connectNeurons(Neuron* neuron1, 
			Neuron* neuron2);

		/// A list of all Connections in the Projection.
		std::vector<Connection*> mConnections;

		/// A list of all target Neurons.  Useful for adding new Connections 
		/// later.
		std::vector<Neuron*> mTargetNeurons;

		/// The method used to set new Connection weights.
		InitialWeightMethod mInitialWeightMethod;

		/// The maximum activation sum of the presynaptic Population (used 
		/// to determine initial weight noise).
		real mMaxInputPopActivationSum;
	};
}

#endif
