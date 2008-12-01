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

#include "RBFNeuron.h"
#include "RBFInputData.h"

namespace verve
{
	RBFNeuron::RBFNeuron(unsigned int id)
	: Neuron(id)
	{
		//mType = NEURON_TYPE_RBF;
		mNumDiscreteDimensions = 0;
		mNumContinuousDimensions = 0;
		mDiscretePosition = NULL;
		mContinuousPosition = NULL;
		mRBFDenominatorInverse = 0;
		mActivationThresholdDistanceSquared = 0;
		mNewRBFThresholdDistanceSquared = 0;
	}

	RBFNeuron::~RBFNeuron()
	{
		delete [] mDiscretePosition;
		delete [] mContinuousPosition;
	}

	void RBFNeuron::init(real stdDevWidth, real newRBFThreshold, 
		const RBFInputData& inputData)
	{
		mNumDiscreteDimensions = inputData.numDiscInputs;
		mNumContinuousDimensions = inputData.numContInputs;

		if (mDiscretePosition)
		{
			delete [] mDiscretePosition;
		}

		if (0 == mNumDiscreteDimensions)
		{
			mDiscretePosition = NULL;
		}
		else
		{
			mDiscretePosition = new unsigned int[mNumDiscreteDimensions];
			for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
			{
				mDiscretePosition[i] = inputData.discInputData[i];
			}
		}

		if (mContinuousPosition)
		{
			delete [] mContinuousPosition;
		}

		if (0 == mNumContinuousDimensions)
		{
			mContinuousPosition = NULL;
		}
		else
		{
			mContinuousPosition = new real[mNumContinuousDimensions];
			for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
			{
				mContinuousPosition[i] = inputData.contInputData[i];
			}
		}

		// This constant is the inverse of the denominator in the Gaussian 
		// basis function.
		mRBFDenominatorInverse = 1 / (2 * stdDevWidth * stdDevWidth);

		// According to the 'empirical rule', 99.7% of the data are within 
		// three standard deviations from the mean, so we'll 
		// let the activation threshold equal (3 * stdDevWidth), giving us a 
		// squared distance threshold of (3 * stdDevWidth)^2 = 
		// 9 * stdDevWidth * stdDevWidth.
		//mActivationThresholdDistanceSquared = 9 * stdDevWidth * stdDevWidth;
		// Update: The threshold distance (3 * stdDevWidth) yielded 
		// significant artifacts (e.g. as seen by visualizing the Agent's 
		// value function).  Instead, we'll use (4 * stdDevWidth).
		mActivationThresholdDistanceSquared = 16 * stdDevWidth * stdDevWidth;

		// Store the squared distance for faster computations later.
		mNewRBFThresholdDistanceSquared = newRBFThreshold * newRBFThreshold;
	}

	RBFActivationCode RBFNeuron::updateFiringRate(
		const RBFInputData& inputData)
	{
		// Check the distance between this RBF and the given point along 
		// each dimension.  As soon as we can determine that the data 
		// point is not close enough to affect this RBFNeuron's activation, 
		// return.

		// Compute the distance in discrete space.  This distance is 
		// defined as zero when the values match and infinite when they 
		// don't match.
		for (unsigned int i = 0; i < mNumDiscreteDimensions; ++i)
		{
			// If the position doesn't match along every single discrete 
			// dimension, quit early with no activation.
			if (mDiscretePosition[i] != inputData.discInputData[i])
			{
				setFiringRate(0);
				return NO_ACTIVATION;
			}
		}

		// If we've gotten to this point, the data point must match the 
		// RBF in discrete space.  For the special case when we only have 
		// discrete inputs and no continuous ones, we can set the 
		// activation high and quit.
		if (0 == mNumContinuousDimensions)
		{
			setFiringRate(1);
			return HIGH_ACTIVATION;
		}

		// Now compute the distance in continuous space by looping over 
		// the continuous dimensions.
		real distanceSquared = 0;
		for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
		{
			// IDEA: If we represented RBF positions using a vector of 
			// input Connections (and the input data as pre-synaptic Neuron 
			// firing rates), we could delete Connections over time that 
			// never get used.  This would help avoid having to compare 
			// the data point to the RBF position in potentially every 
			// input dimension.

			// IDEA: Closely-related inputs might need to be grouped into 
			// separate arrays (e.g. vision, somatosensory, etc.).  Each 
			// of these arrays should have a separate set of RBFs with the 
			// dimensionality of the RBF array equal to that of the 
			// number of values in the particular input array.  For now, 
			// assume all inputs use the same RBF array.

			// The following process computes the RBFNeuron activation 
			// level based on the Euclidean distance between the RBF and 
			// the data point.

			real distanceInThisDimension = inputData.contInputData[i] - 
				mContinuousPosition[i];

			// Deal with circular input ranges, if necessary.
			if (inputData.contCircularData[i])
			{
				// Note that the maximum input range is 2.  If the position 
				// in the ith dimensions is more than half of this range 
				// away (i.e. 1 unit away) from the data point,  we 
				// temporarily shift the RBF position in this dimensions 
				// to make it wrap around the ends of the range.

				if (distanceInThisDimension > 1)
				{
					// Recompute distance with wrapped weight value.
					distanceInThisDimension -= 2;
				}
				else if (distanceInThisDimension < -1)
				{
					// Recompute distance with wrapped weight value.
					distanceInThisDimension += 2;
				}
			}

			// Add to the squared distance.
			distanceSquared += 
				(distanceInThisDimension * distanceInThisDimension);

			// As soon as we know the distance is beyond the activation 
			// distance threshold, we can quit early and skip the remaining 
			// dimensions.
			if (distanceSquared > mActivationThresholdDistanceSquared)
			{
				setFiringRate(0);
				return NO_ACTIVATION;
			}
		}

		// At this point we know that the point must be close enough to the 
		// RBF to affect its activation level.  We have computed the sum of 
		// the squared distances along each continuous dimension.  Now 
		// update the RBFNeuron's activation using a Gaussian-shaped basis 
		// function.
		setFiringRate(
			globals::exp(-distanceSquared * mRBFDenominatorInverse));


		//// TESTING: Self-adapting centers; move the RBF closer to the 
		//// input point.  Note that RBFs with NO_ACTIVATION get ignored.  
		//// TODO: disable this if learning is disabled.
		//real adaptationLearningRate = (real)0.00001;

		//// 1. Move the RBF in proportion to a Gaussian 
		//// curve, which decreases with the distance from the input 
		//// point?
		//if (distanceSquared > 0)
		//{
		//	for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
		//	{
		//		real directionInThisDimension = inputData.contInputData[i] - 
		//			mContinuousPosition[i];

		//		// Handle circular dimensions.
		//		if (inputData.contCircularData[i])
		//		{
		//			if (directionInThisDimension > 1)
		//			{
		//				// Recompute distance with wrapped weight value.
		//				directionInThisDimension -= 2;
		//			}
		//			else if (directionInThisDimension < -1)
		//			{
		//				// Recompute distance with wrapped weight value.
		//				directionInThisDimension += 2;
		//			}
		//		}

		//		// Normalize the direction vector.
		//		directionInThisDimension = directionInThisDimension / 
		//			globals::sqrt(distanceSquared);

		//		// Adaptation influence in each dimension is proportional 
		//		// to the unit's Gaussian activation.
		//		real influence = getFiringRate() * directionInThisDimension;
		//		mContinuousPosition[i] += adaptationLearningRate * influence;

		//		// Adjust for circular dimensions.
		//		if (mContinuousPosition[i] < -1)
		//		{
		//			mContinuousPosition[i] += 2;
		//		}
		//		else if (mContinuousPosition[i] > 1)
		//		{
		//			mContinuousPosition[i] -= 2;
		//		}
		//	}
		//}

		//for (unsigned int i = 0; i < mNumContinuousDimensions; ++i)
		//{
		//	//// 2. Do we move the RBF in proportion to its distance from the 
		//	//// input point?
		//	//real distanceInThisDimension = inputData.contInputData[i] - 
		//	//	mContinuousPosition[i];
		//	//mContinuousPosition[i] += ((verve::real)0.00001 * 
		//	//	distanceInThisDimension);

		//	//// 3. ...or combine the distance function (y=x) and the Gaussian 
		//	//// function, giving us a function that has high influence at 
		//	//// medium distances and small influence at small and large 
		//	//// distances.
		//	//real distanceInThisDimension = inputData.contInputData[i] - 
		//	//	mContinuousPosition[i];
		//	//real influence = distanceInThisDimension * 
		//	//	globals::exp(-(distanceInThisDimension * 
		//	//	distanceInThisDimension) * mRBFDenominatorInverse);
		//	//mContinuousPosition[i] += (verve::real)0.0001 * influence;

		//	// Adjust for circular inputs.
		//	if (mContinuousPosition[i] < -1)
		//	{
		//		mContinuousPosition[i] += 2;
		//	}
		//	else if (mContinuousPosition[i] > 1)
		//	{
		//		mContinuousPosition[i] -= 2;
		//	}
		//}


		if (distanceSquared > mNewRBFThresholdDistanceSquared)
		{
			// The point is close enough to affect the RBFNeuron's 
			// activation, but far enough away to consider creating a new 
			// RBF.
			return LOW_ACTIVATION;
		}
		else
		{
			// The point is close enough that we won't consider creating a 
			// new RBF.
			return HIGH_ACTIVATION;
		}
	}

	unsigned int RBFNeuron::getNumDiscreteDimensions()const
	{
		return mNumDiscreteDimensions;
	}

	unsigned int RBFNeuron::getNumContinuousDimensions()const
	{
		return mNumContinuousDimensions;
	}

	const unsigned int* RBFNeuron::getDiscretePosition()const
	{
		return mDiscretePosition;
	}

	const real* RBFNeuron::getContinuousPosition()const
	{
		return mContinuousPosition;
	}
}
