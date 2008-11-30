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

#include <fstream>

#include "RLModule.h"
#include "Observation.h"
#include "Population.h"
#include "Projection.h"
#include "RBFNeuron.h"
#include "RBFPopulation.h"
#include "TDProjection.h"
#include "TDConnection.h"
#include "WinnerTakeAllPopulation.h"

namespace verve
{
	RLModule::RLModule(const Observation& obs, bool isDynamicRBFEnabled, 
		unsigned int numActions)
	{
		// Make the stored input data match the size of the given 
		// Observation.
		mLatestInputData.init(obs);

		// Create the state representation Population.
		mStateRepresentation = new RBFPopulation();
		mStateRepresentation->init(mLatestInputData, isDynamicRBFEnabled);
		mAllPopulations.push_back(mStateRepresentation);

		// Create the actor Population.
		mActorPopulation = new WinnerTakeAllPopulation();
		mActorPopulation->init(numActions);
		mAllPopulations.push_back(mActorPopulation);

		// Create the critic Population.
		mCriticPopulation = new Population();
		mCriticPopulation->init(1);
		mAllPopulations.push_back(mCriticPopulation);

		// Create a Projection from the state representation to the actor.  
		// Start all policy Connections with zero weights to give each 
		// action an equal initial selection probability.
		mStateRepresentation->projectTD(mActorPopulation, 
			POLICY_TDCONNECTION, WEIGHTS_NEAR_0, 
			mStateRepresentation->computeMaxActivationSum());

		// Create a Projection from the state representation to the critic.  
		// Initialize weights to zero.
		mStateRepresentation->projectTD(mCriticPopulation, 
			VALUE_FUNCTION_TDCONNECTION, WEIGHTS_NEAR_0, 
			mStateRepresentation->computeMaxActivationSum());

		mFirstStep = true;
		mTDError = 0;
		mOldValueEstimation = 0;
		mNewValueEstimation = 0;
		mETraceTimeConstant = 0;
		mTDDiscountTimeConstant = 0;
		mTDDiscountFactor = 0;
		mValueFunctionLearningTimeConstant = 0;
		mValueFunctionLearningFactor = 0;
		mPolicyLearningMultiplier = 0;
	}

	RLModule::~RLModule()
	{
		// Destroy Populations, including the Neurons and Projections 
		// contained within them.
		while (!mAllPopulations.empty())
		{
			delete mAllPopulations.back();
			mAllPopulations.pop_back();
		}
	}

	void RLModule::resetShortTermMemory()
	{
		mLatestInputData.zeroInputData();

		unsigned int size = (unsigned int)mAllPopulations.size();
		for (unsigned int i = 0; i < size; ++i)
		{
			mAllPopulations[i]->resetShortTermMemory();
		}

		// Empty the lists of active TDConnections.
		mActiveValueFunctionTDConnections.clearList();
		mActivePolicyTDConnections.clearList();

		mFirstStep = true;
		mTDError = 0;
		mOldValueEstimation = 0;
		mNewValueEstimation = 0;
	}

	unsigned int RLModule::update(const Observation& obs, real reinforcement)
	{
		// Keep a copy of the latest input data.
		mLatestInputData.copyInputData(obs.getDiscreteInputData(), 
			obs.getContinuousInputData());

		// The order of events here should be functionally equivalent to the 
		// pseudocode listed on page 174 of Sutton's & Barto's book 
		// Reinforcement Learning.  Here is the modified pseudocode being 
		// implemented (V and A are the critic and actor, respectively): 
		// 
		// Increase eligibility traces using s.
		// Compute V(s).
		// Compute V(s').
		// Compute action a using A(s').
		// TD error = r' + gamma * V(s') - V(s).
		// Train A and V using TD error.
		// Decay eligibility traces.
		// Replace s with s'.
		// (Take action a, letting it affect the environment which then 
		// updates r' and s' for next time.)

		unsigned int actionIndex = 0;

		if (mFirstStep)
		{
			// If this is the very first step, we simply choose a new 
			// action based on the latest Observation.  This is to make sure 
			// we don't try to use the state representation s before it is 
			// actually valid: it won't be valid until the next update.

			// Update the state to represent s'.  We assume here that 
			// learning is enabled (which lets the state representation add 
			// new RBFs if necessary).
			mStateRepresentation->updateFiringRatesRBF(mLatestInputData, 
				true);

			// Compute A(s').
			actionIndex = updateActorOutput();

			// Now the state will correctly represent s for the next update.
			mFirstStep = false;
		}
		else
		{
			// The state at this point should represent s (which is the same 
			// as s' from the previous update).

			// Check for new active (i.e. eligible) TDConnections.  This 
			// must be called after the state and actor have been updated 
			// (in that order) because the actor's input TDConnections 
			// depend on the pre- and post-synaptic firing rates.  Also, 
			// we should not decay the eligibility traces for the new 
			// active TDConnections until after we increase them at least 
			// once so they don't get removed immediately.
			updateActiveTDConnectionList();

			// Increase eligibility traces, still using s.
			mActiveValueFunctionTDConnections.increaseETraces();
			mActivePolicyTDConnections.increaseETraces();

			// Compute V(s).
			mOldValueEstimation = updateCriticOutput();

			// Update the state to represent s'.  This will add any 
			// newly-active TDConnections to the active list and 
			// initially increase their eligibility traces.
			mStateRepresentation->updateFiringRatesRBF(mLatestInputData, 
				true);

			// Compute V(s') and A(s').
			mNewValueEstimation = updateCriticOutput();
			actionIndex = updateActorOutput();

			// TD error = r' + gamma * V(s') - V(s).
			mTDError = reinforcement + mTDDiscountFactor * 
				mNewValueEstimation - mOldValueEstimation;

			// Train A and V using TD error.  Note that the current 
			// eligibility traces are still from state s.
			trainTDRule();

			// Decay eligibility traces.
			mActiveValueFunctionTDConnections.decayETraces();
			mActivePolicyTDConnections.decayETraces();
		}

		// The state representation (currently s') will implicitly become 
		// s the next time this function is called.  Before this is called 
		// again, we assume the Agent will take action a, letting it affect 
		// the environment, which then updates r' and s' for the next 
		// update.

		return actionIndex;
	}

	unsigned int RLModule::updatePolicyOnly(const Observation& obs)
	{
		// We simply choose a new action based on the given Observation.

		// Keep a copy of the latest input data.
		mLatestInputData.copyInputData(obs.getDiscreteInputData(), 
			obs.getContinuousInputData());

		// Update the state to represent s'.
		mStateRepresentation->updateFiringRatesRBF(mLatestInputData, false);

		// Compute and return A(s').
		return updateActorOutput();
	}

	void RLModule::changeStepSize(real newValue)
	{
		setETraceTimeConstant(mETraceTimeConstant, newValue);
		setTDDiscountTimeConstant(mTDDiscountTimeConstant, newValue);
		setTDLearningRate(mValueFunctionLearningTimeConstant, 
			mPolicyLearningMultiplier, newValue);
	}

	void RLModule::setETraceTimeConstant(real timeConstant, real stepSize)
	{
		mETraceTimeConstant = timeConstant;
		real ETraceDecayFactor = globals::calcDecayConstant(
			mETraceTimeConstant, stepSize);

		if (mStateRepresentation)
		{
			mStateRepresentation->setPostETraceDecayFactors(
				ETraceDecayFactor);
		}
	}

	void RLModule::setTDDiscountTimeConstant(real timeConstant, real stepSize)
	{
		mTDDiscountTimeConstant = timeConstant;
		mTDDiscountFactor = globals::calcDecayConstant(
			mTDDiscountTimeConstant, stepSize);

		if (mStateRepresentation)
		{
			mStateRepresentation->setPostTDDiscountFactors(
				mTDDiscountFactor);
		}
	}

	void RLModule::setTDLearningRate(real valueFunctionTimeConstant, 
		real policyLearningMultiplier, real stepSize)
	{
		mValueFunctionLearningTimeConstant = valueFunctionTimeConstant;
		mPolicyLearningMultiplier = policyLearningMultiplier;
		mValueFunctionLearningFactor = 1 - globals::calcDecayConstant(
			mValueFunctionLearningTimeConstant, stepSize);

		// The learning factor should be normalized as follows: 
		// 
		// learning factor = learning factor / # of active features
		// 
		// This method allows us to change the number of active features 
		// in the state representation without making learning unstable.  
		// Since we're using an RBF state representation, the number of 
		// active features is equal to the total sum of RBF activation.
		mValueFunctionLearningFactor = mValueFunctionLearningFactor / 
			mStateRepresentation->computeMaxActivationSum();
	}

	real RLModule::getTDError()
	{
		return mTDError;
	}

	void RLModule::resetState(const Observation& obs)
	{
		// Keep a copy of the latest input data.
		mLatestInputData.copyInputData(obs.getDiscreteInputData(), 
			obs.getContinuousInputData());

		mStateRepresentation->updateFiringRatesRBF(mLatestInputData, 
			false);
	}

	real RLModule::computeValueEstimation(const Observation& obs)
	{
		// Note: We don't want to enable any learning here because this 
		// function should have no residual effects.

		// Use this input data structure to send data to the state 
		// representation.
		RBFInputData tempInputData;
		tempInputData.init(obs);

		// Temporarily update the state representation with the given 
		// Observation (with learning disabled).
		mStateRepresentation->updateFiringRatesRBF(tempInputData, false);

		real valueEstimation = updateCriticOutput();

		// Put the state back to what it was before this function was 
		// called.
		mStateRepresentation->updateFiringRatesRBF(mLatestInputData, 
			false);

		return valueEstimation;
	}

	void RLModule::saveValueData(unsigned int continuousResolution, 
		const std::string& filename)
	{
		if (mLatestInputData.numDiscInputs == 0 
			&& mLatestInputData.numContInputs == 0)
		{
			// This Agent has no inputs.
			return;
		}

		// Check if we need to auto generate a unique filename.
		std::string nameStr = filename;
		if (nameStr.empty())
		{
			static unsigned int count = 0;
			char newName[64];
			sprintf(newName, "agentValueData%d.dat", count);
			nameStr = newName;
			++count;
		}

		std::ofstream file(nameStr.c_str());

		// Output a '#' for the header line.  '#' lines are ignored by 
		// gnuplot.
		file << "# ";

		// For the header, output the number of distinct points being 
		// checked along every input dimension.
		for (unsigned int i = 0; i < mLatestInputData.numDiscInputs; ++i)
		{
			file << mLatestInputData.discNumOptionsData[i] << " ";
		}

		for (unsigned int i = 0; i < mLatestInputData.numContInputs; ++i)
		{
			file << continuousResolution << " ";
		}

		file << std::endl;

		// Iterate over every possible combination of inputs.
		RBFInputData inputdata;
		inputdata.init(mLatestInputData.numDiscInputs, 
			mLatestInputData.discNumOptionsData, 
			mLatestInputData.discInputData, 
			mLatestInputData.numContInputs, continuousResolution, 
			mLatestInputData.contCircularData, 
			mLatestInputData.contInputData);
		unsigned numStates = inputdata.computeNumUniqueStates(
			continuousResolution);
		for (unsigned int i = 0; i < numStates; ++i)
		{
			inputdata.setToUniqueState(i, numStates, continuousResolution);

			// Print the state input data.
			for (unsigned int i = 0; i < mLatestInputData.numDiscInputs; ++i)
			{
				file << inputdata.discInputData[i] << " ";
			}

			for (unsigned int i = 0; i < mLatestInputData.numContInputs; ++i)
			{
				file << inputdata.contInputData[i] << " ";
			}

			// Compute and print the estimated value.  Temporarily update 
			// the state representation with this data (with learning 
			// disabled because this function should have no residual 
			// effects).
			mStateRepresentation->updateFiringRatesRBF(inputdata, false);
			file << updateCriticOutput() << std::endl;
		}

		file.close();

		// Put the state back to what it was before this function was 
		// called.
		mStateRepresentation->updateFiringRatesRBF(mLatestInputData, 
			false);
	}

	void RLModule::saveStateRBFData(const std::string& filename)
	{
		if (mLatestInputData.numDiscInputs == 0 
			&& mLatestInputData.numContInputs == 0)
		{
			// This Agent has no inputs.
			return;
		}

		// Check if we need to auto generate a unique filename.
		std::string nameStr = filename;
		if (nameStr.empty())
		{
			static unsigned int count = 0;
			char newName[64];
			sprintf(newName, "agentStateRBFData%d.dat", count);
			nameStr = newName;
			++count;
		}

		std::ofstream file(nameStr.c_str());

		// Save each RBF position on a separate line with discrete data 
		// before continuous data.
		unsigned int numRBFs = mStateRepresentation->getNumNeurons();
		for (unsigned int i = 0; i < numRBFs; ++i)
		{
			RBFNeuron* n = static_cast<RBFNeuron*>(
				mStateRepresentation->getNeuron(i));

			unsigned int numDiscreteDimensions = 
				n->getNumDiscreteDimensions();
			for (unsigned int dim = 0; dim < numDiscreteDimensions; ++dim)
			{
				file << n->getDiscretePosition()[dim] << " ";
			}

			unsigned int numContinuousDimensions = 
				n->getNumContinuousDimensions();
			for (unsigned int dim = 0; dim < numContinuousDimensions; ++dim)
			{
				file << n->getContinuousPosition()[dim] << " ";
			}

			file << std::endl;
		}

		file.close();
	}

	real RLModule::getLatestValueEstimation()const
	{
		return mNewValueEstimation;
	}

	void RLModule::updateActiveTDConnectionList()
	{
		// This checks for new active TDConnections based on the current 
		// list of active Neurons.

		unsigned int numActiveNeurons = 
			mStateRepresentation->getNumActiveNeurons();
		for (unsigned int n = 0; n < numActiveNeurons; ++n)
		{
			Neuron* activeNeuron = 
				mStateRepresentation->getActiveNeuron(n);

			// Look at the axons from the active Neuron.
			unsigned int numAxons = activeNeuron->getNumAxons();
			for (unsigned int a = 0; a < numAxons; ++a)
			{
				// Assume here that the axons are TDConnections.  This 
				// will always be true since we only add Neurons to the 
				// active list that have TDConnection axons.
				TDConnection* axon = static_cast<TDConnection*>(
					activeNeuron->getAxon(a));

				// Ignore TDConnections already in the list.
				if (axon->isInActiveList())
				{
					continue;
				}

				// If the TDConnection is eligible (according to its own 
				// method for determining eligibility), we will add it to 
				// the active TDConnection list.
				switch (axon->getTDConnectionType())
				{
					case VALUE_FUNCTION_TDCONNECTION:
						mActiveValueFunctionTDConnections.
							addNewActiveConnection(axon);
						break;
					case POLICY_TDCONNECTION:
						// We already know the pre-synaptic Neuron is active, 
						// so we'll check the post-synaptic Neuron.  This 
						// assumes the post-synaptic Neuron never has a 
						// negative firing rate.
						if (axon->getPostNeuron()->getFiringRate() > 0)
						{
							mActivePolicyTDConnections.
								addNewActiveConnection(axon);
						}
						break;
					default:
						assert(false);
				}
			}
		}
	}

	void RLModule::trainTDRule()
	{
		mActiveValueFunctionTDConnections.trainConnections(
			mValueFunctionLearningFactor * mTDError);
		mActivePolicyTDConnections.trainConnections(
			mValueFunctionLearningFactor * mPolicyLearningMultiplier * 
			mTDError);
	}

	real RLModule::updateCriticOutput()
	{
		mCriticPopulation->updateFiringRatesLinear();
		return mCriticPopulation->getNeuron(0)->getFiringRate();
	}

	unsigned int RLModule::updateActorOutput()
	{
		mActorPopulation->updateFiringRatesWTA();
		int actionIndex = mActorPopulation->getActiveOutput();
		assert(-1 != actionIndex);
		return actionIndex;
	}
}
