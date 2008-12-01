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

#ifndef VERVE_RL_MODULE_H
#define VERVE_RL_MODULE_H

#include "Globals.h"
#include "RBFInputData.h"
#include "ActiveTDConnectionList.h"

namespace verve
{
	class Observation;
	class Population;
	class Projection;
	class RBFPopulation;
	class WinnerTakeAllPopulation;

	/// An RLModule learns from reinforcements to improve its action 
	/// selection in order to increase its future reinforcement intake.
	class RLModule
	{
	public:
		/// Sets up the RLModule to work with the given type of 
		/// Observation.  Applies initial noise to trainable Connection 
		/// weights.
		VERVE_DECL RLModule(const Observation& obs, 
			bool isDynamicRBFEnabled, unsigned int numActions);

		VERVE_DECL virtual ~RLModule();

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Updates and trains the RLModule using the given reinforcement 
		/// and corresponding Observation.  Returns the index of the next 
		/// action to perform.  The reward value SHOULD be within the range 
		/// [-1, 1], but we can be lenient since the predictive model might 
		/// output invalid reward values.
		VERVE_DECL virtual unsigned int VERVE_CALL update(
			const Observation& obs, real reinforcement);

		/// Returns the index of the next action to perform based on the 
		/// given Observation.
		VERVE_DECL virtual unsigned int VERVE_CALL updatePolicyOnly(
			const Observation& obs);

		/// Updates all step size-dependent factors using the new step size.
		VERVE_DECL virtual void VERVE_CALL changeStepSize(real newValue);

		/// Sets how fast the eligibility traces will decay.  The time 
		/// constant must be greater than zero.  Automatically updates the 
		/// eligibility trace decay factors in all TDConnections.
		VERVE_DECL virtual void VERVE_CALL setETraceTimeConstant(
			real timeConstant, real stepSize);

		/// Sets how much future rewards are discounted.  The time constant 
		/// must be greater than zero.  Automatically updates the discount 
		/// factors in all TDConnections.
		VERVE_DECL virtual void VERVE_CALL setTDDiscountTimeConstant(
			real timeConstant, real stepSize);

		/// Sets the TD learning rate for the value function and policy.  
		/// The time constant (which must be greater than zero) specifies 
		/// how many seconds it takes for the value function's prediction 
		/// errors to be reduced to 37% of their initial values.  The 
		/// policy learning multiplier combined with the value function's 
		/// learning rate determines the policy's learning rate (the 
		/// multiplier usually ranges from 1-100).
		VERVE_DECL virtual void VERVE_CALL setTDLearningRate(
			real valueFunctionTimeConstant, real policyLearningMultiplier, 
			real stepSize);

		/// Returns the current TD error.
		VERVE_DECL virtual real VERVE_CALL getTDError();

		/// Returns the latest value estimation.  This depends on 
		/// how recently the 'update' function has been called.
		VERVE_DECL virtual real VERVE_CALL getLatestValueEstimation()const;

		/// "Resets" the internal state representation to represent the given 
		/// Observation.  This is useful for situations when outside 
		/// functions (like planning) must change the state temporarily, 
		/// then reset it back to how it was before.  Note: This function 
		/// will not let the RBF state representation dynamically allocate 
		/// new RBFs.
		VERVE_DECL virtual void VERVE_CALL resetState(const Observation& obs);

		/// Computes and returns the value estimation for the given 
		/// observation.  This should not be performed regularly 
		/// as it is fairly expensive.
		VERVE_DECL virtual real VERVE_CALL computeValueEstimation(
			const Observation& obs);

		/// Outputs a data file containing estimated values for every 
		/// possible state.  The 'resolution' parameter determines how 
		/// many values to check along each continuous input dimension.  
		/// Passing in an empty filename string will automatically 
		/// generate a unique filename and save the file in the current 
		/// working directory.  This does nothing if the Agent uses no 
		/// inputs.  The output file format is:
		/// First line: the number of distinct values along each input 
		/// dimension
		/// All other lines: the inputs in each dimension and the value 
		/// of the corresponding state.
		VERVE_DECL virtual void VERVE_CALL saveValueData(
			unsigned int continuousResolution, 
			const std::string& filename = "");

		/// Outputs a data file containing the position of all RBFs in the 
		/// state representation, including discrete and continuous data.  
		/// Passing in an empty filename string will automatically 
		/// generate a unique filename and save the file in the current 
		/// working directory.  This does nothing if the Agent uses no 
		/// inputs.
		VERVE_DECL virtual void VERVE_CALL saveStateRBFData(
			const std::string& filename = "");

	protected:
		/// Adds new active TDConnections to the ActiveTDConnectionList.
		void updateActiveTDConnectionList();

		/// Trains the active TDConnections.
		void trainTDRule();

		/// Updates the critic's output (i.e. the value estimation) and 
		/// returns the current value estimation.
		real updateCriticOutput();

		/// Updates the actor's output and returns the next action index.
		unsigned int updateActorOutput();

		/// The most recent sensory input data.  We store a copy of this so 
		/// that we can, for example, compute the estimated value of a 
		/// user-supplied Observation, then set the state representation 
		/// back to the actual latest input data before the next update.
		RBFInputData mLatestInputData;

		/// The state representation Population.
		RBFPopulation* mStateRepresentation;

		/// The actor Population.
		WinnerTakeAllPopulation* mActorPopulation;

		/// The critic Population.
		Population* mCriticPopulation;

		/// A list of all Populations.
		std::vector<Population*> mAllPopulations;

		/// A dynamic list of the active value function TDConnections.
		ActiveTDConnectionList mActiveValueFunctionTDConnections;

		/// A dynamic list of the active policy TDConnections.
		ActiveTDConnectionList mActivePolicyTDConnections;

		/// Used to handle the first step differently.  This is necessary 
		/// because temporal difference learning compares values between 
		/// two steps, which isn't possible on the first step.
		bool mFirstStep;

		/// The current TD error signal.
		real mTDError;

		/// The value estimation of the state based on the old sensory 
		/// inputs.
		real mOldValueEstimation;

		/// The value estimation of the state based on the new sensory 
		/// inputs.
		real mNewValueEstimation;

		/// Time constant that determines the rate of eligibility trace 
		/// decay.
		real mETraceTimeConstant;

		/// Time constant that determines the TD discount rate.
		real mTDDiscountTimeConstant;

		/// A precomputed discount factor using the TD discount time 
		/// constant and step size.
		real mTDDiscountFactor;

		/// Time constant that determines the learning rate for the value 
		/// function.
		real mValueFunctionLearningTimeConstant;

		/// A precomputed discount factor using the value function learning 
		/// time constant and step size.
		real mValueFunctionLearningFactor;

		/// The policy's learning rate is a combination of this multiplier 
		/// and the value function's learning rate.
		real mPolicyLearningMultiplier;
	};
}

#endif
