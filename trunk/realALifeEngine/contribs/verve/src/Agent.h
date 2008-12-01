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

#ifndef VERVE_AGENT_H
#define VERVE_AGENT_H

#include "Globals.h"
#include "AgentDescriptor.h"
#include "Observation.h"

namespace verve
{
	class Observation;
	class PredictiveModel;
	class RLModule;

	/// A data structure containing Agent data that should be useful for 
	/// debugging.
	struct AgentDebugData
	{
		AgentDebugData()
		{
			reset();
		}

		void reset()
		{
			reward = 0;
			valueEstimation = 0;
			TDError = 0;
			curiosityReward = 0;
			predictionUncertaintyEstimation = 0;
			modelPredictionError = 0;
			planLength = 0;
		}

		/// The external reward signal.
		real reward;

		/// The value function's value estimation.
		real valueEstimation;

		/// The temporal difference error.
		real TDError;

		/// The intrinsic curiosity reward signal.
		real curiosityReward;

		/// The estimated uncertainty about the model's prediction.
		real predictionUncertaintyEstimation;

		/// The error in the model's prediction.
		real modelPredictionError;

		/// The planning trajectory length (number of steps).
		unsigned int planLength;
	};

	/// An Agent is an autonomous entity that learns from direct 
	/// with its environment.  
	class Agent
	{
	public:
		/// Creates an Agent using the given AgentDescriptor.  Adds initial 
		/// noise to the trainable weights.  Never use this to create an 
		/// Agent dynamically (i.e. never call "new Agent").  Instead, use 
		/// the global factory functions.  This ensures that memory is 
		/// allocated from the correct heap.
		VERVE_DECL Agent(const AgentDescriptor& desc);

		// TODO: VERVE_DECL Agent(const Agent& a);

		// TODO: VERVE_DECL Agent(const std::string& filename);

		/// Note that "delete Agent" should never be called on a 
		/// dynamically-allocated Agent.  Instead, use the destroy 
		/// function.  This ensures that memory is deallocated from the 
		/// correct heap.
		VERVE_DECL virtual ~Agent();

		/// Deallocates a dynamically-allocated Agent.  Use this instead of 
		/// "delete Agent" to ensure that memory is deallocated from the 
		/// correct heap.
		VERVE_DECL virtual void VERVE_CALL destroy();

		/// Resets temporary dynamics without affecting learned parameters.
		VERVE_DECL virtual void VERVE_CALL resetShortTermMemory();

		/// Gives the Agent reinforcement for the current state, the 
		/// current observation (i.e. sensory input data from the current 
		/// state), and how much time has elapsed since the previous 
		/// update.  Allows the Agent to learn (if learning is enabled).  
		/// Returns the index of the action to perform.  The reward value 
		/// must be within the range [-1, 1] (this is to ensure that the 
		/// reward magnitude is not used to affect the TD learning rate).  
		/// It is best to pass in the same dt each time this is called 
		/// (when the dt changes between successive calls, several things 
		/// must be recomputed internally).
		VERVE_DECL virtual unsigned int VERVE_CALL update(
			real reinforcement, const Observation& obs, real dt);

		/// Returns the number of discrete sensors.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumDiscreteSensors()const;

		/// Returns the number of continuous sensors.
		VERVE_DECL virtual unsigned int VERVE_CALL 
			getNumContinuousSensors()const;

		/// Sets how fast the eligibility traces will decay.  The time 
		/// constant must be greater than zero.
		VERVE_DECL virtual void VERVE_CALL setETraceTimeConstant(
			real timeConstant);

		/// Sets how much future rewards are discounted.  The time constant 
		/// must be greater than zero.
		VERVE_DECL virtual void VERVE_CALL setTDDiscountTimeConstant(
			real timeConstant);

		/// Sets the TD learning rate for the value function and policy.  
		/// The time constant (which must be greater than zero) specifies 
		/// how many seconds it takes for the value function's prediction 
		/// errors to be reduced to 37% of their initial values.  The 
		/// policy learning multiplier combined with the value function's 
		/// learning rate determines the policy's learning rate (the 
		/// multiplier usually ranges from 1-100).
		VERVE_DECL virtual void VERVE_CALL setTDLearningRate(
			real valueFunctionTimeConstant, real policyLearningMultiplier);

		/// Sets the learning rate for the predictive model.  The time 
		/// constant (which must be greater than zero) specifies how many 
		/// seconds it takes for the prediction errors to be reduced to 
		/// 37% of their initial values.  This does nothing if this Agent 
		/// was not constructed with a predictive model.
		VERVE_DECL virtual void VERVE_CALL setModelLearningRate(
			real timeConstant);

		/// Enables and disables learning.  Once the Agent performs 
		/// adequately, learning can be disabled to improve runtime 
		/// performance.
		VERVE_DECL virtual void VERVE_CALL setLearningEnabled(
			bool enabled);

		/// Returns the age of the Agent in seconds.  Note that the age 
		/// is only incremented when the Agent is learning.
		VERVE_DECL virtual long unsigned int VERVE_CALL getAge()const;

		/// Returns the age of the Agent as a string containing the hours, 
		/// minutes, and seconds.  Note that the age is only incremented 
		/// when the Agent is learning.
		VERVE_DECL virtual std::string VERVE_CALL getAgeString()const;

		///// Returns the most recent TD error.
		//VERVE_DECL virtual real VERVE_CALL getTDError()const;

		///// Returns the most recent mean squared error from the predictive 
		///// model.  Returns zero if this Agent was not constructed with 
		///// a predictive model.
		//VERVE_DECL virtual real VERVE_CALL getModelMSE()const;

		///// Returns the length of the most recent planning sequence 
		///// (in number of steps).
		//VERVE_DECL virtual unsigned int VERVE_CALL getLastPlanLength()const;

		/// Computes and returns the value estimation for the given 
		/// observation.  This should not be performed regularly 
		/// as it is fairly expensive.
		VERVE_DECL virtual real VERVE_CALL computeValueEstimation(
			const Observation& obs);

		/// Returns a pointer to the Agent's descriptor.
		VERVE_DECL virtual const AgentDescriptor* VERVE_CALL 
			getDescriptor()const;

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

		///// Loads data from the given Verve XML file into the Agent.  
		///// Destroys any data already present in the Agent.
		//virtual void VERVE_CALL internal_load(const std::string& filename);

		///// Saves the Agent to a Verve XML file of the given filename.  
		///// Passing in an empty filename string will automatically 
		///// generate a unique filename and save the file in the current 
		///// working directory.
		//virtual void VERVE_CALL save(const std::string& filename = "");

		/// Returns the most recent data for debugging purposes.
		VERVE_DECL virtual const AgentDebugData& VERVE_CALL getDebugData()const;

	protected:
		/// Sets the size of the time steps used during each simulation 
		/// step.  
		void setStepSize(real value);

		/// Performs a single planning sequence which trains the RLModule.  
		/// This proceeds until either the prediction uncertainty is too 
		/// high or the sequence length is too long.
		void planningSequence(const Observation& predCurrentObs, 
			real predCurrentReward, real initialUncertainty);

		/// Computes a curiosity reward as a function of the uncertainty 
		/// estimation.
		real computeCuriosityReward(real uncertaintyEstimation);

		/// Increases the Agent's age by one time step.
		void incrementAge();

		/// A saved copy of the AgentDescriptor used to create this Agent.
		AgentDescriptor mDescriptor;

		/// The main reinforcement learning component.
		RLModule* mRLModule;

		/// The predictive model component.
		PredictiveModel* mPredictiveModel;

		/// Used to handle the first step differently.  This is necessary 
		/// because the Agent is trained using the state representation 
		/// from the previous step.
		bool mFirstStep;

		/// A stored copy of the most recent action index.
		unsigned int mActionIndex;

		/// A copy of the previous actual Observation.  This must be stored 
		/// across time steps.
		Observation mActualPrevObs;

		/// An allocated Observation, mainly used for convenience.  This 
		/// does not need to be valid across time steps.
		Observation mPredCurrentObs;

		/// An allocated Observation, mainly used for convenience.  This 
		/// does not need to be valid across time steps.
		Observation mTempPlanningObs;

		/// Determines whether the Agent learns.
		bool mLearningEnabled;

		/// The current step size being used.
		real mStepSize;

		/// The hours component of the Agent's age.
		long unsigned int mAgeHours;

		/// The minutes component of the Agent's age.
		unsigned int mAgeMinutes;

		/// The seconds component of the Agent's age.
		real mAgeSeconds;

		///// The length of the most recent planning sequence (in number of 
		///// steps).
		//unsigned int mLastPlanningSequenceLength;

		/// A copy of some of the latest data useful for debugging.
		AgentDebugData mDebugData;
	};
}

#endif
