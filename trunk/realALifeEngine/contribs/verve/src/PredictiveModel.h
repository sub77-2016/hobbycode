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

#ifndef VERVE_PREDICTIVE_MODEL_H
#define VERVE_PREDICTIVE_MODEL_H

#include "Globals.h"
#include "Observation.h"
#include "RBFInputData.h"

// PREDICTIVE MODEL STATE REPRESENTATION (currently, METHOD 1 is used): 
// METHOD 1. predictive model uses a state-action representation 
//		(takes obs and action inputs).  This is more standard.
// METHOD 2. predictive model just uses a state representation 
//		(takes only an obs input); since the policy eventually 
//		becomes predictable, presumably the next state will 
//		become predictable.

namespace verve
{
	class Population;
	class RBFPopulation;

	/// A PredictiveModel learns a predictive model of the environment 
	/// dynamics (transitions) from direct experience.  Given some 
	/// Observation and action, it outputs the predicted next Observation 
	/// and reward.  It is trained using prediction errors computed from 
	/// the actual next Observation and reward.
	class PredictiveModel
	{
	public:
		/// Sets up the RLModule to work with the given type of 
		/// Observation.  Applies initial noise to trainable Connection 
		/// weights.
		VERVE_DECL PredictiveModel(const Observation& obs, 
			bool isDynamicRBFEnabled, unsigned int numActions);

		VERVE_DECL virtual ~PredictiveModel();

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Predicts the current Observation and reward based on the 
		/// previous Observation and action.  This also trains the predictors 
		/// based on the given actual current Observation and actual 
		/// current reward.  On the first step the predicted Observation 
		/// and reward will simply be set equal to the actual Observation 
		/// and reward.
		VERVE_DECL virtual void VERVE_CALL predictAndTrain(
			const Observation& actualPrevObs, 
			unsigned int prevAction, const Observation& actualCurrentObs, 
			const real actualCurrentReward, Observation& predCurrentObs, 
			real& predCurrentReward, real& predUncertainty);

		/// Predicts the next Observation and reward based on the given 
		/// current Observation and action.  Long-term changes occur here 
		/// only if 'allowDynamicRBFs' is true.
		VERVE_DECL virtual void VERVE_CALL predict(
			const Observation& actualCurrentObs, 
			unsigned int currentAction, Observation& predNextObs, 
			real& predNextReward, real& predUncertainty, 
			bool allowDynamicRBFCreation);

		/// Updates all step size-dependent factors using the new step size.
		VERVE_DECL virtual void VERVE_CALL changeStepSize(real newValue);

		/// Sets the TD learning rate for the value function and policy.  
		/// The time constant (which must be greater than zero) specifies 
		/// how many seconds it takes for the value function's prediction 
		/// errors to be reduced to 37% of their initial values.  The 
		/// policy learning multiplier combined with the value function's 
		/// learning rate determines the policy's learning rate (the 
		/// multiplier usually ranges from 1-100).
		VERVE_DECL virtual void VERVE_CALL setDeltaLearningRate(
			real timeConstant, real stepSize);

		/// Returns the most recent combined mean squared error for 
		/// Observation and reward predictions.
		VERVE_DECL virtual real VERVE_CALL getPredictionMSE();

	protected:
		/// A convenient data structure used to pass data to the 
		/// state-action representation.
		RBFInputData mStateActionInputData;

		/// An array used to store the discrete Observation data used 
		/// for training.  Instead of keeping discrete data as integer 
		/// values, it must be converted to reals within [-1, 1] before 
		/// being used for training.
		real* mDiscObsTrainingData;

		/// The state-action representation Population.
		RBFPopulation* mStateActionRepresentation;

		/// The discrete Observation prediction Population.
		Population* mDiscObsPredPopulation;

		/// The continuous Observation prediction Population.
		Population* mContObsPredPopulation;

		/// The reward prediction Population.
		Population* mRewardPredPopulation;

		/// The uncertainty prediction Population.
		Population* mUncertaintyPredPopulation;

		/// A list of all Populations.
		std::vector<Population*> mAllPopulations;

		/// The latest MSE for Observation and reward predictions.
		real mLatestPredMSE;

		/// A time constant that determines the delta rule learning rate.
		real mDeltaLearningTimeConstant;

		/// A precomputed value used for the delta rule learning rate.
		real mDeltaLearningFactor;
	};
}

#endif
