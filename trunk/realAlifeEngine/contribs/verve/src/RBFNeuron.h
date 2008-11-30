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

#ifndef VERVE_RBF_NEURON_H
#define VERVE_RBF_NEURON_H

#include "Neuron.h"

namespace verve
{
	struct RBFInputData;

	/// Different values used as a coarse representation of how much a 
	/// data point activates the RBF.  This is useful for keeping track 
	/// of active RBFNeurons and deciding when to create new RBFs.
	enum RBFActivationCode
	{
		/// The data point is close enough to give the RBF high activation, 
		/// so we can avoid creating any new RBFs.
		HIGH_ACTIVATION,

		/// The data point is close enough to affect the RBF's activation, 
		/// but a new RBF might need to be created.
		LOW_ACTIVATION,

		/// The data point is far enough away from the RBF that it does 
		/// not affect its activation at all.  We might need to create a 
		/// new RBF.
		NO_ACTIVATION
	};

	/// A "radial basis function" Neuron.  Each RBFNeuron stores its 
	/// position in both continuous and discrete space.  This allows 
	/// RBFNeurons to respond to a mixture of continuous and discrete 
	/// input data.  (This may sound a bit confusing compared to normal 
	/// radial basis functions.  Basically, the distance from the RBF 
	/// to an input data point is computed in continuous space using 
	/// the Euclidean distance, and the distance along a discrete 
	/// dimension is either zero or infinite, depending on whether 
	/// the discrete values match.)  
	class RBFNeuron : public Neuron
	{
	public:
		VERVE_DECL RBFNeuron(unsigned int id);

		VERVE_DECL virtual ~RBFNeuron();

		/// Initializes the RBFNeuron.  This sets its 'stdDevWidth' (i.e. 
		/// the distance of one standard deviation from the RBF center), 
		/// the 'newRBFThreshold' (points beyond this distance from the 
		/// RBF center might need a new RBF to be created), number of 
		/// dimensions being used in both discrete and continuous 
		/// space, and the RBF's position in discrete and continuous space.  
		VERVE_DECL virtual void VERVE_CALL init(real stdDevWidth, 
			real newRBFThreshold, const RBFInputData& inputData);

		/// Updates the firing rate based on the distance to the given 
		/// data point.  The dimensionality of the given point must match 
		/// that of the RBF's position, both in discrete and continuous 
		/// space.  The returned activation code can be used for keeping 
		/// track of active RBFNeurons and deciding when to create new 
		/// RBFs.  This assumes that at least one input dimension exists, 
		/// either discrete or continuous.
		VERVE_DECL virtual RBFActivationCode VERVE_CALL updateFiringRate(
			const RBFInputData& inputData);

		/// Returns the number of discrete dimensions in the RBF space.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumDiscreteDimensions()const;

		/// Returns the number of continuous dimensions in the RBF space.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumContinuousDimensions()const;

		/// Returns a constant pointer to the RBF's discrete position data.
		VERVE_DECL virtual const unsigned int* VERVE_CALL 
			getDiscretePosition()const;

		/// Returns a constant pointer to the RBF's continuous position 
		/// data.
		VERVE_DECL virtual const real* VERVE_CALL 
			getContinuousPosition()const;

	protected:
		/// The number of dimensions in the discrete position.
		unsigned int mNumDiscreteDimensions;

		/// The number of dimensions in the continuous position.
		unsigned int mNumContinuousDimensions;

		/// Array representing the RBF position in discrete space.
		unsigned int* mDiscretePosition;

		/// Array representing the RBF position in continuous space.
		real* mContinuousPosition;

		/// A constant precomputed value used in the Gaussian basis 
		/// function (the inverse of the denominator).
		real mRBFDenominatorInverse;

		/// The threshold distance (squared for faster computations) that 
		/// determines how close a data point must be to affect the 
		/// RBFNeuron's firing rate.
		real mActivationThresholdDistanceSquared;

		/// The threshold distance (squared for faster computations) that 
		/// determines how far a data point must be from the RBF before we 
		/// consider creating a new RBF.
		real mNewRBFThresholdDistanceSquared;
	};
}

#endif
